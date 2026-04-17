// Copyleft 2001 Chris Welty
//	All Rights Reserved


#include <string>
#include <sstream>
#include <cstring>  // Pour memcmp
#include <chrono>
#include <thread>

#include "types.hpp"
#include "ggsstream.hpp"
#include "GGSMessage.hpp"
#include "OsMessage.hpp"



ggsstream::ggsstream() : std::iostream(nullptr) {
    fLoggedIn=fHasOs=false;
    psockbuf=nullptr;
}

ggsstream::~ggsstream() {
    
    //if (IsLoggedIn()) Logout();
    Disconnect();
    
}

void ggsstream::HeartbeatLoop() {
    while (!stopHeartbeat) {
        // --- L'attente intelligente ---
        {
            std::unique_lock<std::mutex> lock(mtx);
            // On attend 60s, mais on se réveille IMMÉDIATEMENT si stopHeartbeat change
            cv.wait_for(lock, std::chrono::seconds(60), [this] {
                return stopHeartbeat.load();
            });
        }
        
        // Si on s'est réveillé parce que stopHeartbeat est vrai, on sort direct
        if (stopHeartbeat)
            break;
        
        if (IsConnected() && IsLoggedIn()) {
            //            std::lock_guard<std::mutex> lock(mtx);
            //            if (!stopHeartbeat && psockbuf != nullptr) {  // ← DOUBLE CHECK
            
            //std::cout << "[CLIENT] Sending heartbeat pulse..." << std::endl;
            
            this->clear(); // Reset des flags d'état
            (*this) << "t /os continue\n";
            this->flush();
            
            // Si le socket est mort, flush() lèvera le failbit.
            // La boucle while(get(c)) dans Process() s'arrêtera,
            // déclenchant votre fAutoReconnect.
            if (this->fail()) {
                std::cout << "[CLIENT] Heartbeat failed (Socket closed)" << std::endl;
                // ATTENTION rien d'autre, pas d'appel à StopHeartbeat()
            }
        }
    }
    //    }
}

void ggsstream::StopHeartbeat() {
    stopHeartbeat = true;
    cv.notify_all(); // Réveil instantané du thread qui dort dans wait_for
    
    
    if (heartbeatThread.joinable()) {
        heartbeatThread.join();            // ← garanti safe maintenant
    }
    
}

void ggsstream::EnableAutoReconnect(bool enable, int maxRetries, int delayMs) {
    fAutoReconnect = enable;
    nMaxRetries = maxRetries;
    nReconnectDelayMs = delayMs;
    nCurrentRetry = 0;
}

void ggsstream::DisableAutoReconnect() {
    fAutoReconnect = false;
}

int ggsstream::Connect(const std::string& sServer, int nPort) {
    
    // 1. ARRÊTER le heartbeat avant toute chose pour libérer le socket
    StopHeartbeat();
    
    // Reset the iostream state (clears eofbit, failbit, etc.)
    // Essential for the Process() loop to restart.
    this->clear();
    
    if(IsConnected()) {
        return kErrConnected;
    }
    
    
    // Save for reconnection
    sLastServer = sServer;
    nLastPort = nPort;
    
    int err = kErrUnknown;
    
    psockbuf = new sockbuf();
    if (psockbuf) {
        err = psockbuf->connect(sServer, nPort);
        if (!err) {
            init(psockbuf);
        } else {
            std::cerr << "[ERROR] Connection failed: " << ErrText(err) << std::endl;
            delete psockbuf;
            psockbuf = nullptr;
        }
    }
    
    if (!err) {
        
        // Relance du pulsateur
        stopHeartbeat = false;
        if (heartbeatThread.joinable()) heartbeatThread.join();
        heartbeatThread = std::thread(&ggsstream::HeartbeatLoop, this);
        std::cout << "[STREAM]  ✓ HeartbeatLoop started - sends keepalive every 60s" << std::endl;
        std::cout << "[STREAM] ═══════════════════════════════════════\n" << std::endl;
        
        
    }
    
    return err;
}

// New method: Attempts to reconnect
bool ggsstream::TryReconnect() {
    
    if (!fAutoReconnect || sLastServer.empty()) {
        return false;
    }
    
    // ═══════════════════════════════════════════════════════════
    // IMPORTANT: Clean up before attempting reconnection
    // ═══════════════════════════════════════════════════════════
    Disconnect();
    
    
    nCurrentRetry = 0;
    int currentWorkDelay = nReconnectDelayMs;
    
    while (nCurrentRetry < nMaxRetries) {
        nCurrentRetry++;
        
        // ═══════════════════════════════════════════════════════════
        // CALLBACK #1 : Before each attempt
        // ═══════════════════════════════════════════════════════════
        OnReconnecting(nCurrentRetry, nMaxRetries);
        
        // Wait between attempts (except the first)
        if (nCurrentRetry > 1) {
            
            std::cout << "[RECONNECT] Waiting " << currentWorkDelay / 1000 << "s before next attempt..." << std::endl;
            
            std::this_thread::sleep_for(
                                        std::chrono::milliseconds(currentWorkDelay)
                                        );
            
            // On double le délai pour l'itération SUIVANTE
            // Suite : 5s -> 10s -> 20s -> 40s -> 80s...
            if (currentWorkDelay < (3600*1000)) { // On plafonne à 1h max par sécurité
                currentWorkDelay *= 2;
            }
        }
        
        // Try connection
        int err = Connect(sLastServer, nLastPort);
        if (!err) {
            // Connection OK → Try login
            if (!sLogin.empty() && !sPassword.empty()) {
                err = Login(sLogin.c_str(), sPassword.c_str());
                if (!err) {
                    // ═══════════════════════════════════════════════════
                    // CALLBACK #2 : Success!
                    // ═══════════════════════════════════════════════════
                    OnReconnected();
                    return true;
                }
                // Login failed → Disconnect and retry
                Disconnect();
            } else {
                OnReconnected();
                return true;
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════
    // CALLBACK #3 : Total failure
    // ═══════════════════════════════════════════════════════════
    OnReconnectFailed();
    return false;
}

// Called BEFORE each reconnection attempt
void ggsstream::OnReconnecting(int attempt, int maxAttempts) {
    std::cout << "[RECONNECT] Attempting reconnection "
    << attempt << "/" << maxAttempts << "..." << std::endl;
}

// Called when reconnection SUCCEEDS
void ggsstream::OnReconnected() {
    std::cout << "[RECONNECT] Successfully reconnected to " << sLastServer << "\n" << std::endl;
    
    // You can add actions here:
    // - Send commands to the server
    // - Log the event
    // - Update statistics
    
    // Example: Re-send setup commands
    (*this) << "ve -ack\n";
    (*this) << "notify + /os\n";
    flush();
}

// Called when ALL retries have failed
void ggsstream::OnReconnectFailed() {
    std::cerr << "[RECONNECT] Failed to reconnect after " << nMaxRetries
    << " attempts." << std::endl;
    
    // You can add actions here:
    // - Log the error
    // - Send a notification
    // - Save state before quitting
}

void ggsstream::ForceDisconnect() {
    
    Disconnect();
    
}


int ggsstream::Disconnect() {
    
    // Un streambuf vide et inoffensif, jamais supprimé
    // 1. On définit la structure avec un nom local
    struct NullBuf : std::streambuf {};

    // 2. On l'alloue via un pointeur statique pour éviter le warning de destructeur
    static std::streambuf* sNullBuf = new NullBuf();
    
    //stopHeartbeat() special: débloque flush() dans heartbeat
    stopHeartbeat = true;
    cv.notify_all();
    if (psockbuf) psockbuf->disconnect(); // débloque flush() dans heartbeat
    if (heartbeatThread.joinable()) heartbeatThread.join(); // safe();
    
    // Vérifier AVANT de modifier
    if (!psockbuf) {
        return 0;  // Déjà déconnecté
    }
    
    // Maintenant on peut tout nettoyer
    fLoggedIn = false;
    
    setstate(std::ios::eofbit);
    
    delete psockbuf;
    psockbuf = nullptr;
    
    init(sNullBuf);          // stream toujours valide, mais inactif
    clear(std::ios::eofbit);
    return 0;
    
}

// return 0 if no error
// 1 if socket err (e.g. connection timed out)
int ggsstream::Login(const char* sName, const char* sPwd) {
    int err=0;
    
    if (fLoggedIn) {
        err = kErrLoggedIn;
    }
    
    // await login prompt
    if (!err) {
        sLogin=sName;
        sPassword = sPwd;
        err = await("login");
    }
    
    // send login, await password prompt
    if (!err) {
        (*this) << sLogin << "\n";
        flush();
        err = await("password");
    }
    
    // send password, await response
    if (!err) {
        (*this) << sPassword << "\n";
        flush();
        err = await("\n");
    }
    
    // check to see if password was accepted
    if (!err) {
        char c;
        c=peek();
        switch(c) {
            case ':':
                err = kErrBadPassword;
                break;
            case 'R':
                fLoggedIn=true;
                break;
            default:
                err = kErrUnknown;
        }
    }
    
    // send fake "Login" message
    if (!err) {
        CMsgGGSLogin *pmsg= new CMsgGGSLogin;
        if (pmsg) {
            pmsg->pgs=this;
            Post(pmsg);
        }
    }
    
    return err;
}

int ggsstream::Logout() {
    if (fLoggedIn==false) {
        return kErrLoggedOut;
    }
    else {
        (*this) << "quit\n";
        flush();
        fLoggedIn=false;
        return 0;
    }
}


int ggsstream::await(const char* sAwait) {
    
    if (!sAwait || !*sAwait) return kErrInvalidArg;
    
    const size_t awaitLen = strlen(sAwait);
    
    std::string sLine;
    sLine.reserve(1024); // Pré-alloue
    
    char c;
    while (get(c)) {
        sLine.push_back(c);
        
        // Vérification seulement quand on a assez de caractères
        if (sLine.size() >= awaitLen) {
            // Cherche uniquement dans les derniers awaitLen caractères
            const char* tail = sLine.c_str() + sLine.size() - awaitLen;
            if (memcmp(tail, sAwait, awaitLen) == 0) {
                return 0;
            }
        }
        
        // OPTIMISATION : Si la chaîne devient trop longue, on garde
        // seulement la fin pour ne pas saturer la mémoire.
        if (sLine.size() > 4096) {
            // On garde juste assez pour ne pas rater le début du prochain sAwait
            sLine.erase(0, sLine.size() - awaitLen);
        }
    }
    
    
    // EOF ou erreur (ne fonctionne pas)
    if (psockbuf)
        return psockbuf->Err();
    else {
        return kErrNoStreambuf;
    }
    
}

const std::string& ggsstream::GetLogin() const {
    return sLogin;
}

const std::string& ggsstream::GetPassword() const {
    return sPassword;
}


// process incoming data from GGS. 'is' is a socket connection.
// strip bells and '|' at the beginning of lines. Once we have an
// entire message(terminated by "READY" on its own line), call Parse()
void ggsstream::Process() {
    
    bool keepRunning = true;
    
    while (keepRunning) {
        std::string sLine;
        sLine.reserve(256);
        bool fHasCR = false;
        
        char c;
        
        // Boucle de lecture principale
        while (get(c)) {
            switch(c) {
                case '\a':
                    break;
                case '\r':
                    ProcessLine(sLine);
                    break;
                case '\n':
                    if (!fHasCR)
                        ProcessLine(sLine);
                    break;
                default:
                    sLine.push_back(c);
            }
            fHasCR = (c == '\r');
        }
        
        // Si on sort du while(get(c)), c'est qu'il y a eu une déconnexion ou une erreur
        bool wasLoggedIn = fLoggedIn;
        
        // Notification de déconnexion
        CMsg* pmsg = new CMsgGGSDisconnect;
        if(pmsg) {
            pmsg->pgs = this;
            Post(pmsg);
        }
        
        // Tentative de reconnexion
        if (fAutoReconnect && wasLoggedIn) {
            if (TryReconnect()) {
                // Reconnexion réussie : la boucle "while(keepRunning)"
                // recommence et entre à nouveau dans "while(get(c))"
                //std::cout << "[DEBUG] Re-entering main loop after successful reconnect." << std::endl;
            } else {
                // Échec total après toutes les tentatives
                keepRunning = false;
            }
        } else {
            // Pas d'auto-reconnect ou utilisateur non loggé : on quitte
            keepRunning = false;
        }
    }
}

void ggsstream::ProcessLine(std::string& sLine){
    
    // Log de la ligne reçue
    if (!sLine.empty()) {
        std::cout << "[RECV] " << sLine << std::endl;
    }
    
    if (sLine=="READY")
        ProcessMessage();
    else {
        const char* pLine = sLine.c_str();
        
        if (!sMsg.empty()) {
            // GGS sometimes sends 2 messages without a READY
            if (pLine[0]==':') {
                ProcessMessage();
            }
            else {
                sMsg.push_back('\n');
                
                // GGS sends '|' at the beginning of all tell lines
                // except the first, strip them
                if (pLine[0]=='|')
                    pLine++;
            }
        }
        
        sMsg.append(pLine);
    }
    
    sLine.clear();        // Garde la capacité
    
}

// Gets a message. Calls GetMsgType() to create the new message;
//	the user must delete() the message, perhaps in the
//	Post() routine

void ggsstream::ProcessMessage() {
    std::istringstream is(sMsg.c_str());
    CMsg *pmsg;
    
    pmsg=GetMsgType(is);
    
    if (pmsg) {
        pmsg->pgs=this;
        pmsg->sRawText=sMsg;
        pmsg->In(is);
        Post(pmsg);
    }
    
    sMsg="";
}

void ggsstream::Post(CMsg* pmsg) {
    pmsg->Handle();
    delete pmsg;
}

CMsg* ggsstream::GetMsgType(std::istream& is) {
    std::string sFrom;
    CMsg *pmsg;
    
    is >> sFrom >> std::ws;
    
    if (sFrom.empty())
        pmsg=nullptr;
    else {
        // direct messages end in ':', channel messages don't
        if (sFrom.end()[-1]==':')
            sFrom.resize(sFrom.size()-1);
        
        if (sFrom=="/os")
            pmsg=GetMsgTypeOs(is);
        else if (sFrom=="")
            pmsg=GetMsgTypeGGS(is);
        else {
            pmsg = new CMsgGGSTell;
        }
        if (pmsg)
            pmsg->sFrom=sFrom;
    }
    
    return pmsg;
}

CMsg* ggsstream::GetMsgTypeOs(std::istream& is) {
    CMsg* pmsg=nullptr;
    
    std::string sMsgType;
    is >> sMsgType >> std::ws;
    
    if (sMsgType[0]=='.')
        pmsg=new CMsgOsComment(sMsgType);
    else if (sMsgType=="abort")
        pmsg=new CMsgOsAbortRequest;
    else if (sMsgType=="end")
        pmsg=new CMsgOsEnd;
    else if (sMsgType=="ERR")
        pmsg=new CMsgOsErr;
    else if (sMsgType=="fatal-timeout")
        pmsg=new CMsgOsFatalTimeout;
    else if (sMsgType=="finger")
        pmsg=new CMsgOsFinger;
    else if (sMsgType=="history") {
        if (is.peek()=='E')
            pmsg=new CMsgOsErr;
        else
            pmsg=new CMsgOsHistory;
    }
    else if (sMsgType=="illegal")
        pmsg=new CMsgOsErr;
    else if (sMsgType=="join")
        pmsg=new CMsgOsJoin;
    else if (sMsgType=="look")
        pmsg=new CMsgOsLook;
    else if (sMsgType=="match")
        pmsg=new CMsgOsMatch;
    else if (sMsgType=="rank")
        pmsg=new CMsgOsRank;
    else if (sMsgType=="rating_update")
        pmsg=new CMsgOsRatingUpdate;
    else if (sMsgType=="stored")
        pmsg=new CMsgOsStored;
    else if (sMsgType=="timeout")
        pmsg=new CMsgOsTimeout;
    else if (sMsgType=="top")
        pmsg=new CMsgOsTop;
    else if (sMsgType=="trust-violation")
        pmsg=new CMsgOsTrustViolation;
    else if (sMsgType=="undo")
        pmsg=new CMsgOsUndoRequest;
    else if (sMsgType=="update")
        pmsg=new CMsgOsUpdate;
    else if (sMsgType=="watch") {
        is >> std::ws;
        char c=is.peek();
        if (c=='+' || c=='-')
            pmsg=new CMsgOsErr;
        else
            pmsg=new CMsgOsWatch;
    }
    else if (sMsgType=="who")
        pmsg=new CMsgOsWho;
    else if (sMsgType=="+" || sMsgType=="-") {
        bool fPlus= sMsgType=="+";
        is >> std::ws;
        if (is.peek()=='.')
            pmsg=new CMsgOsRequestDelta(fPlus);
        else {
            is >> sMsgType;
            if (sMsgType=="match")
                pmsg=new CMsgOsMatchDelta(fPlus);
            else {
                std::string sLogin=sMsgType;
                is >> sMsgType;
                if (sMsgType=="watch") {
                    pmsg=new CMsgOsWatchDelta(fPlus, sLogin);
                }
                else{
                    // os: + booklet stored
                    pmsg=new CMsgOsUnknown("--");
                }
            }
        }
    }
    else
        pmsg=new CMsgOsUnknown(sMsgType);
    
    return pmsg;
}

CMsg* ggsstream::GetMsgTypeGGS(std::istream& is) {
    CMsg* pmsg=nullptr;
    
    std::string sMsgType;
    is >> sMsgType;
    
    if (sMsgType=="alias")
        pmsg=new CMsgGGSAlias;
    else if (sMsgType=="ERR")
        pmsg=new CMsgGGSErr;
    else if (sMsgType=="finger")
        pmsg= new CMsgGGSFinger;
    else if (sMsgType=="help")
        pmsg= new CMsgGGSHelp;
    else if (sMsgType=="who")
        pmsg=new CMsgGGSWho;
    else if (sMsgType=="+" || sMsgType=="-") {
        bool fPlus= sMsgType=="+";
        pmsg=new CMsgGGSUserDelta(fPlus);
    }
    else
        pmsg=new CMsgGGSUnknown;
    
    return pmsg;
}

const char* ggsstream::ErrText(int err) {
    
    if (err == 0x8100 || (err >= 0x8600 && err <= 0x860F)) return sockbuf::ErrText(err);
    
    switch(err) {
        case kErrBadPassword:
            return "Your password is invalid, or someone has already chosen that login";
        case kErrLoggedIn:
            return "You are already logged into GGS";
        case kErrLoggedOut:
            return "You have already logged out of GGS";
        case kErrUnknown:
            return "Unknown GGS error";
        case kErrInvalidArg:
            return "Invalid argument";
        case kErrConnected:
            return "Connection is already established.";
        case kErrNotConnected:
            return "You are not connected to the server.";
        case kErrNoStreambuf:
            return "Internal error: No stream buffer available.";
        case kErrUserCancelled:
            return "Operation cancelled by the user.";
            
        default:
            return "(No text available for this error)";
    }
}

bool ggsstream::IsConnected() const {
    return psockbuf!=nullptr;
}

bool ggsstream::IsLoggedIn() const {
    return fLoggedIn;
}

bool ggsstream::HasOthelloServer() const {
    return fHasOs;
}

void ggsstream::BaseGGSDisconnect() {
    idToGame.clear();
    idToMatch.clear();
    idToRequest.clear();
    fLoggedIn=fHasOs=false;
    //sLogin.erase(); ne pas supprimer
}

void ggsstream::BaseGGSLogin() {
    
    // required commands for ODK to work:
    (*this) << "ve -ack\n"		    // turn off GGS Parser comments
    << "notify + /os\n"; 	// tell us when /os comes up/goes down
    
    flush();
}

void ggsstream::BaseGGSUserDelta(const CMsgGGSUserDelta* pmsg) {
    if (pmsg->sLogin=="/os" && pmsg->fPlus!=fHasOs) {
        fHasOs=pmsg->fPlus;
        if (pmsg->fPlus)
            HandleOsLogin();
        else
            HandleOsLogout();
    }
}

void ggsstream::BaseOsEnd(const CMsgOsEnd* pmsg) {
    // end messages occur at the end of some synch games
    //	to let you know the result
    
    COsGame* pgame=PGame(pmsg->idg);
    if (pgame) {
        //update result
        if (!pgame->mt.fSynch) {
            std::cerr << "[GGS] Warning: 'end' message received for non-synch game id="
                      << pmsg->idg << std::endl;
            return;
        }
        pgame->SetResult(pmsg->result, pmsg->sPlayers);
    }
}

void ggsstream::BaseOsGameOver(const std::string& idg) {
    idToGame.erase(idg);
}

// we get a "Join" message (and the whole game is sent)
//	when we join the game,  when komi is set in a game,
//	and when a move is undone in a game.
void ggsstream::BaseOsJoin(const CMsgOsJoin* pmsg) {
    //map<string,COsGame>::iterator i=idToGame.find(pmsg->idg);
    idToGame[pmsg->idg]=pmsg->game;
}

void ggsstream::BaseOsLogin() {
    // required commands for ODK to work:
    (*this) << "tell /os client +\n";   // get compact messages
    flush();
    
    fHasOs=true;
}

void ggsstream::BaseOsLogout() {
    idToGame.clear();
    idToMatch.clear();
    idToRequest.clear();
    fHasOs=false;
}

void ggsstream::BaseOsMatch(const CMsgOsMatch* pmsg) {
    idToMatch.clear();
    std::vector<COsMatch>::const_iterator i;
    
    for (i=pmsg->matches.begin(); i!=pmsg->matches.end(); i++)
        idToMatch[i->idm]=*i;
}

// helper function for BaseOsMatchDelta
void ggsstream::EndGame(const CMsgOsMatchDelta* pmsg, const std::string& idg) {
    COsGame* pgame=PGame(idg);
    if (pgame) {
        // synch games with normal termination should have
        //	gotten an "End" Message.
        // Other finished games should have the result set in the update message.
        // timeout games should not have the result yet.
        
        if (pgame->result.status==COsResult::kUnfinished)
            pgame->SetResult(pmsg->result, pmsg->match.pis);
        HandleOsGameOver(pmsg, idg);
        //HandleOsGameOver(idg);
    }
}

void ggsstream::BaseOsMatchDelta(const CMsgOsMatchDelta* pmsg) {
    std::map<std::string,COsMatch>::iterator i=idToMatch.find(pmsg->match.idm);
    if (pmsg->fPlus) {
        if (i != idToMatch.end()) {
            std::cerr << "[GGS] Warning: duplicate match id="
            << pmsg->match.idm << ", overwriting" << std::endl;
        }
        idToMatch[pmsg->match.idm]=pmsg->match;
    }
    else {
        if (i!=idToMatch.end()) {
            
            if (pmsg->match.mt.fSynch) {
                EndGame(pmsg, pmsg->match.idm+".0");
                EndGame(pmsg, pmsg->match.idm+".1");
            }
            else {
                EndGame(pmsg, pmsg->match.idm);
            }
            idToMatch.erase(i);
        }
    }
}

// delete the request if we have it. We might not, e.g. if we've just logged in
void ggsstream::BaseOsRequestDelta(const CMsgOsRequestDelta* pmsg) {
    std::map<std::string,COsRequest>::iterator i=idToRequest.find(pmsg->idr);
    if (pmsg->fPlus) {
        if (i != idToRequest.end()) {
            std::cerr << "[GGS] Warning: duplicate request id="
            << pmsg->idr << ", overwriting" << std::endl;
        }
        idToRequest[pmsg->idr]=pmsg->request;
    }
    else {
        if (i!=idToRequest.end())
            idToRequest.erase(i);
    }
}

void ggsstream::BaseOsUpdate(const CMsgOsUpdate* pmsg) {
    // update the game if it exists. Due to lag, we might still
    //	be getting updates for games we've stopped watching
    std::map<std::string,COsGame>::iterator i=idToGame.find(pmsg->idg);
    if (i!=idToGame.end())
        idToGame[pmsg->idg].Update(pmsg->mli);
}

///////////////////////////////////////
// Overridable handlers
///////////////////////////////////////

void ggsstream::HandleGGS(const CMsg* pmsg) {
    std::cout << pmsg->sRawText << std::endl;
}

void ggsstream::HandleGGSAlias(const CMsgGGSAlias* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSDisconnect() {
    BaseGGSDisconnect();
}

void ggsstream::HandleGGSErr(const CMsgGGSErr* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSFinger(const CMsgGGSFinger* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSHelp(const CMsgGGSHelp* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSLogin() {
    BaseGGSLogin();
}

void ggsstream::HandleGGSTell(const CMsgGGSTell* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSUnknown(const CMsgGGSUnknown* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSUserDelta(const CMsgGGSUserDelta* pmsg) {
    BaseGGSUserDelta(pmsg);
    HandleGGS(pmsg);
}

void ggsstream::HandleGGSWho(const CMsgGGSWho* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleOs(const CMsgOs* pmsg) {
    HandleGGS(pmsg);
}

void ggsstream::HandleOsAbortRequest(const CMsgOsAbortRequest* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsComment(const CMsgOsComment* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsEnd(const CMsgOsEnd* pmsg) {
    
    BaseOsEnd(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsErr(const CMsgOsErr* pmsg) {
    
    if (pmsg->err == CMsgOsErr::kErrRequestDoesntFitFormula) {
        // Send a "continue" message
        (*this) << "t /os continue" << "\n";
        flush();
    }
    
    HandleOs(pmsg);
}

void ggsstream::HandleOsFatalTimeout(const CMsgOsFatalTimeout* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsFinger(const CMsgOsFinger* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsGameOver(const CMsgOsMatchDelta* pmsg, const std::string& idg) {
    BaseOsGameOver(idg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsHistory(const CMsgOsHistory* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsJoin(const CMsgOsJoin* pmsg) {
    BaseOsJoin(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsLogin() {
    BaseOsLogin();
}

void ggsstream::HandleOsLogout() {
    BaseOsLogout();
}

void ggsstream::HandleOsLook(const CMsgOsLook* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsMatch(const CMsgOsMatch* pmsg) {
    BaseOsMatch(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsMatchDelta(const CMsgOsMatchDelta* pmsg) {
    BaseOsMatchDelta(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsRank(const CMsgOsRank* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsRatingUpdate(const CMsgOsRatingUpdate* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsRequestDelta(const CMsgOsRequestDelta* pmsg) {
    BaseOsRequestDelta(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsStored(const CMsgOsStored* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsTimeout(const CMsgOsTimeout* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsTop(const CMsgOsTop* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsTrustViolation(const CMsgOsTrustViolation* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsUndoRequest(const CMsgOsUndoRequest* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsUnknown(const CMsgOsUnknown* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsUpdate(const CMsgOsUpdate* pmsg) {
    BaseOsUpdate(pmsg);
    HandleOs(pmsg);
}

void ggsstream::HandleOsWatch(const CMsgOsWatch* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsWatchDelta(const CMsgOsWatchDelta* pmsg) {
    HandleOs(pmsg);
}

void ggsstream::HandleOsWho(const CMsgOsWho* pmsg) {
    // Otherwise the param is unused
    HandleOs(pmsg);
}

COsGame* ggsstream::PGame(const std::string& idg) {
    std::map<std::string, COsGame>::iterator i;
    
    i = idToGame.find(idg);
    if (i==idToGame.end())
        return nullptr;
    else
        return &((*i).second);
}
