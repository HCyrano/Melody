// Copyleft 2001 Chris Welty
//	All Rights Reserved

#include <sstream>


#include "OsMessage.hpp"
#include "ggsstream.hpp"


///////////////////////////////////
// CMsgOs
///////////////////////////////////
/*
COsGame* CMsgOs::PGame(const string& idg) const {
	map<string,COsGame>& idToGame = pgs->idToGame;
	if (idToGame.find(idg)==idToGame.end())
		return nullptr;
	else
		return &(idToGame[idg]);
}
*/
COsGame* CMsgOs::PGame(const std::string& idg) const {
    const std::map<std::string,COsGame>& idToGame = pgs->idToGame;
    auto it = idToGame.find(idg);
    if (it == idToGame.end())
        return nullptr;
    else
        return const_cast<COsGame*>(&(it->second));
}
/*
COsMatch* CMsgOs::PMatch(const string& idm) const {
	map<string,COsMatch>& idToMatch = pgs->idToMatch;
	if (idToMatch.find(idm)==idToMatch.end())
		return nullptr;
	else
		return &(idToMatch[idm]);
}
*/
COsMatch* CMsgOs::PMatch(const std::string& idm) const {
    const std::map<std::string,COsMatch>& idToMatch = pgs->idToMatch;
    auto it = idToMatch.find(idm);
    if (it == idToMatch.end())
        return nullptr;
    else
        return const_cast<COsMatch*>(&(it->second));
}
/*
COsRequest* CMsgOs::PRequest(const string& idr) const {
	map<string,COsRequest>& idToRequest = pgs->idToRequest;
	if (idToRequest.find(idr)==idToRequest.end())
		return nullptr;
	else
		return &(idToRequest[idr]);
}
*/
COsRequest* CMsgOs::PRequest(const std::string& idr) const {
    const std::map<std::string,COsRequest>& idToRequest = pgs->idToRequest;
    auto it = idToRequest.find(idr);
    if (it == idToRequest.end())
        return nullptr;
    else
        return const_cast<COsRequest*>(&(it->second));
}

///////////////////////////////////
// CMsgOsAbortRequest
///////////////////////////////////

void CMsgOsAbortRequest::In(std::istream& is) {
	std::string sDummy;

	is >> idg >> sLogin >> std::ws;
	std::getline(is, sDummy);

    if (sDummy != "is asking")
        std::cerr << "[GGS] Warning: AbortRequest unexpected text: '" << sDummy << "'" << std::endl;

}

void CMsgOsAbortRequest::Handle() {
	pgs->HandleOsAbortRequest(this);
}

///////////////////////////////////
// CMsgOsComment
///////////////////////////////////

CMsgOsComment::CMsgOsComment(const std::string& aidg) {
	idg=aidg;
}

// /os: .44 A 1720 n3: test from me
void CMsgOsComment::In(std::istream& is) {
	std::string sPretext;

	std::getline(is, sPretext, ':');
	std::istringstream isPretext(sPretext.c_str());
	isPretext >> cTo >> pi;
	is >> std::ws;
	std::getline(is, sComment);
}

void CMsgOsComment::Handle() {
	pgs->HandleOsComment(this);
}

///////////////////////////////////
// CMsgOsEnd
///////////////////////////////////

// /os: end .2.0 ( pamphlet vs. ant ) +46.00
void CMsgOsEnd::In(std::istream& is) {
	char c;
	std::string s;

	is >> idg >> c;
    if (c != '(') { std::cerr << "[GGS] Warning: CMsgOsEnd malformed, expected '('" << std::endl; return; }
	is >> sPlayers[0] >> s >> sPlayers[1] >> c;
    if (s != "vs.") { std::cerr << "[GGS] Warning: CMsgOsEnd malformed, expected 'vs.'" << std::endl; return; }
    if (c != ')') { std::cerr << "[GGS] Warning: CMsgOsEnd malformed, expected ')'" << std::endl; return; }
	is >> result;
}

void CMsgOsEnd::Handle() {
    
 	pgs->HandleOsEnd(this);
}

///////////////////////////////////
// CMsgOsErr
///////////////////////////////////

// /os: error .25 corrupt move
// /os: watch + ERR not found: .3

void CMsgOsErr::In(std::istream& is) {
	std::string sText;
	std::getline(is, sText);
	if (sText=="your request doesn't fit the opponent's formula.")
		err=kErrRequestDoesntFitFormula;
	else if (sText=="board type")
		err=kErrIllegalBoardType;
	else if (sText=="rank <type> [login]")
		err=kErrBadRankMessage;
	else if (sText.find("corrupt move")!=std::string::npos)
		err=kErrCorruptMove;
	else if (sText=="Your open value is too low for new matches.")
		err=kErrOpenTooLow;
	else if (sText=="rated variable mismatch.")
		err=kErrRatedMismatch;
	else if (sText=="Rated/unrated mismatch")
		err=kErrRatedMismatch;
	else if (sText=="you are not registered.")
		err=kErrUnregistered;
	else if (sText.find("illegal move")!=std::string::npos)
		err=kErrIllegalMove;
	else if (sText.find("not found")!=std::string::npos)
		err=kErrUserNotFound;
	else if (sText=="Player is not accepting new matches.")
		err=kErrNotAcceptingMatches;
	else if (sText=="only one color preference allowed")
		err=kErrOneColorPreference;
	else if (sText.find("watch + ERR not found:")==0 ||
			sText.find("watch - ERR not found:")==0)
		err=kErrWatchNotFound;
	else
		err=kErrUnknown;
}

void CMsgOsErr::Handle() {
	pgs->HandleOsErr(this);
}

///////////////////////////////////
// CMsgOsFatalTimeout
///////////////////////////////////

void CMsgOsFatalTimeout::In(std::istream& is) {
	is >> idg >> sLogin >> std::ws;
}

void CMsgOsFatalTimeout::Handle() {
	pgs->HandleOsFatalTimeout(this);
}

///////////////////////////////////
// CMsgOsFinger
///////////////////////////////////

/*
/os: finger n2
|login  : n2
|dblen  : 100.0 = 1,052 / 1,052
|level  : 2
|open   : 1
|client : +
|trust  : +
|rated  : +
|bell   : -r -p -w -n -ns -nn -nt -ni -nr -nw -ta -to -tp
|vt100  : -
|hear   : +
|play[recv]   : 
|stored (-) : 0
|notify (+) : * 
|track  (-) : 
|ignore (-) : 
|request(-) :  : 
|watch  (+) : 
|accept : 
|decline: 
|Type  Rating@StDev      Inactive       AScore    Win   Draw   Loss
|10    1720.0@350.0= ----------- +@350.0  +0.0      0 [recv]     0      0
|12    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|14    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|4     1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|6     1720.0@350.0= ----------- +[recv]@350.0  +0.0      0      0      0
|6a    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|6r    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|8     1418.5@ 97.9=     00:01:30+@ 97.9 -18.8     12      1     32
|88    1720.0@[recv]350.0= ----------- +@350.0  +0.0      0      0      0
|8a    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
|8r    1720.0@350.0= ----------- +@350.0  +0.0      0      0      0
*/

void CMsgOsFinger::In(std::istream& is) {
	std::string sLine, sKey, sValue;
	COsFingerRating fr;

	is >> sLogin >> std::ws;

	// first section, key : value
	while (std::getline(is, sLine)) {
		if (!is)
			break;
		if (sLine.find(':')==std::string::npos)
			break;

		std::istringstream isLine(sLine.c_str());

		// get key and strip terminal spaces
		std::getline(isLine, sKey, ':');
        
        // get key and strip terminal spaces
        std::getline(isLine, sKey, ':');
        size_t pos = sKey.find_last_not_of(' ');
        if (pos != std::string::npos)
            sKey.resize(pos + 1);
        else
            sKey.clear();  // Only spaces
        
		// get value. No value means this is the separator row
		//	between the first section and the second section
		isLine >> std::ws;
		std::getline(isLine, sValue);

		// insert (key, value) pair
        if (keyToValue.find(sKey) != keyToValue.end())
            std::cerr << "[GGS] Warning: duplicate finger key='" << sKey << "', overwriting" << std::endl;
        keyToValue[sKey]=sValue;
	}

	// second section, rating info
	while (std::getline(is, sLine)) {
		std::istringstream isLine(sLine.c_str());
		isLine >> fr;
		frs.push_back(fr);
	}
}

void CMsgOsFinger::Handle() {
	pgs->HandleOsFinger(this);
}

///////////////////////////////////
// CMsgOsJoin
///////////////////////////////////

void CMsgOsJoin::In(std::istream& is) {
	is >> idg >> std::ws;
	is >> game;
}

void CMsgOsJoin::Handle() {
	pgs->HandleOsJoin(this);
}

///////////////////////////////////
// CMsgOsLook
///////////////////////////////////

void CMsgOsLook::In(std::istream& is) {
	is >> nGames;
    if (nGames != 1 && nGames != 2) {
        std::cerr << "[GGS] Warning: CMsgOsLook unexpected nGames=" << nGames << std::endl;
        return;
    }
	games.reserve(nGames);
	COsGame game;
	int i;
	for (i=0; i<nGames; i++) {
		is >> game;
		games.push_back(game);
	}
}

void CMsgOsLook::Handle() {
	pgs->HandleOsLook(this);
}

///////////////////////////////////
// CMsgOsHistory
///////////////////////////////////

/*
/os: history 8 nasai
|.48723   25 Mar 2001 03:28:00 1780 nasai    2144 OO7        -4.0 8
|.48724   25 Mar 2001 03:38:33 1780 nasai    2143 OO7       -64.0 8
|.48522   27 Mar 2001 07:34:08 1777 nasai    2350 booklet   -64.0 8
|.49128   27 Mar 2001 07:40:57 1776 nasai    2351 booklet   -64.0 8
|.49134   27 Mar 2001 07:50:42 1775 nasai    2352 booklet   -64.0 8
|.49135   27 Mar 2001 07:52:58 1774 nasai    2352 booklet   -64.0 8
|.49138   27 Mar 2001 07:59:14 1774 nasai    2353 booklet   -64.0 8
|.49141   27 Mar 2001 08:03:04 1773 nasai    2354 booklet   -64.0 8
*/

void CMsgOsHistory::In(std::istream& is) {
	is >> n >> sLogin;
	COsHistoryItem hi;
	while (is >> hi)
		his.push_back(hi);
    if ((int)his.size() != n)
        std::cerr << "[GGS] Warning: history count mismatch, expected " << n << " got " << his.size() << std::endl;}

void CMsgOsHistory::Handle() {
	pgs->HandleOsHistory(this);
}

///////////////////////////////////
// CMsgOsMatch
///////////////////////////////////

/*
/os: match 2/2
| .14       8  R 1174 YA       1503 ant+     1
|  .9   s8r20  R 2574 lynx     2570 kitty    2
*/

void CMsgOsMatch::In(std::istream& is) {
	char c;

	is >> n1 >> c >> n2;
    if (c != '/') { std::cerr << "[GGS] Warning: CMsgOsMatch malformed, expected '/'" << std::endl; return; }

	COsMatch match;
	while (match.In(is))
		matches.push_back(match);

    if ((int)matches.size() != n2)
        std::cerr << "[GGS] Warning: match count mismatch, expected " << n2 << " got " << matches.size() << std::endl;
}

void CMsgOsMatch::Handle() {
	pgs->HandleOsMatch(this);
}

///////////////////////////////////
// CMsgOsMatchDelta
///////////////////////////////////

CMsgOsMatchDelta::CMsgOsMatchDelta(bool afPlus) {
	fPlus=afPlus;
}

// /os: - match .2 1833 nasai 2342 booklet 8 R nasai left
void CMsgOsMatchDelta::In(std::istream& is) {
	match.InDelta(is);
	if (!fPlus)
		is >> result;
}

void CMsgOsMatchDelta::Handle() {
	pgs->HandleOsMatchDelta(this);
}

/*
void CMsgOsMatchDelta::UpdateOs() {
	// if this is a '+ match' message, add the match to the match list
	// if this is a '- match' message, delete the match and any games
	if (fPlus) {
		// add the match
		std::map<string,COsMatch>& idToMatch=Os().idToMatch;
		idToMatch.erase(match.idm);
	}
	else {
		// delete the match
		std::map<string,COsMatch>& idToMatch=Os().idToMatch;
		idToMatch.erase(match.idm);

		// Delete the games. We don't know if we were wathching it,
		//	or if it's synchro, so we just delete all possible game ids.
		std::map<string,COsGame>& idToGame=Os().idToGame;
		idToGame.erase(match.idm);

		std::string idg=match.idm+".0";
		idToGame.erase(idg);

		idg=match.idm+".1";
		idToGame.erase(idg);
	}
}
*/

///////////////////////////////////
// CMsgOsRank
///////////////////////////////////

void CMsgOsRank::In(std::istream& is) {
	std::string sInactive, sAScore, sWin, sDraw, sLoss;
	COsRankData rd;

	is >> rating >> sInactive >> sAScore >> sWin >> sDraw >> sLoss >> n >> std::ws;
	while (is >> rd) {
		rds.push_back(rd);
	}
}

void CMsgOsRank::Handle() {
	pgs->HandleOsRank(this);
}

///////////////////////////////////
// CMsgOsRatingUpdate
///////////////////////////////////

// /os: rating_update .21
// |pamphlet 1720.00 @ 350.00  +316.06 -> 2036.06 @ 254.20
// |ant      1697.62 @ 113.02   -43.23 -> 1654.39 @ 110.44


void CMsgOsRatingUpdate::In(std::istream& is) {
	std::string s;

	is >> idm;
	is >> sPlayers[0] >> rOlds[0] >> dDeltas[0] >> s >> rNews[0];
    if (s != "->") { std::cerr << "[GGS] Warning: CMsgOsRatingUpdate malformed, expected '->'" << std::endl; return; }
	is >> sPlayers[1] >> rOlds[1] >> dDeltas[1] >> s >> rNews[1];
    if (s != "->") { std::cerr << "[GGS] Warning: CMsgOsRatingUpdate malformed, expected '->'" << std::endl; return; }
}

void CMsgOsRatingUpdate::Handle() {
	pgs->HandleOsRatingUpdate(this);
}

///////////////////////////////////
// CMsgOsRequestDelta
///////////////////////////////////

CMsgOsRequestDelta::CMsgOsRequestDelta(bool afPlus) {
	fPlus=afPlus;
}

bool CMsgOsRequestDelta::IAmChallenged() const {
	return request.pis[1].sName==pgs->GetLogin();
}

bool CMsgOsRequestDelta::IAmChallenging() const {
	return request.pis[0].sName==pgs->GetLogin();
}

bool CMsgOsRequestDelta::RequireBoardSize(int n) const {
	bool fOK=request.mt.bt.n==n;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I only play on an " << n << "x" << n << " board\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireKomi(bool fKomi) const {
	bool fOK=request.mt.fKomi==fKomi;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I " << (fKomi?"only":"don't") << " play komi games\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireAnti(bool fAnti) const {
	bool fOK=request.mt.fAnti==fAnti;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I " << (fAnti?"only":"don't") << " play anti games\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireRand(bool fRand) const {
	bool fOK=request.mt.fRand==fRand;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I " << (fRand?"only":"don't") << " play rand games\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireMaxRandDiscs(int nRandDiscs) const {
	bool fOK= (request.mt.nRandDiscs<=nRandDiscs);
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I only play rand games with " << nRandDiscs << " discs maximum\n";
	return fOK;
}
bool CMsgOsRequestDelta::RequireMinRandDiscs(int nRandDiscs) const {
	bool fOK= (request.mt.nRandDiscs>=nRandDiscs);
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I only play rand games with " << nRandDiscs << " discs minimum\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireRated(bool fRated) const {
	bool fOK=request.fRated==fRated;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I " << (fRated?"only":"don't") << " play rated games\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireSynch(bool fSynch) const {
	bool fOK=request.mt.fSynch==fSynch;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I " << (fSynch?"only":"don't") << " play synch games\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireEqualClocks() const {
	bool fOK=request.cks[0]==request.cks[1];
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " I only play games with equal clocks\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireMaxOpponentClock(const COsClock& ck) const {
	bool fOK=request.cks[0]<=ck;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " Your clock can be at most " << ck << "\n";
	return fOK;
}

bool CMsgOsRequestDelta::RequireMinMyClock(const COsClock& ck) const {
	bool fOK=request.cks[1]>=ck;
	if (!fOK)
		(*pgs) << "t " << request.pis[0].sName << " My time must be at least " << ck << "\n";
	return fOK;
}

// /os: +  .19 1735.1 pamphlet 15:00//02:00        8 U 1345.6 ant

void CMsgOsRequestDelta::In(std::istream& is) {
	is >> idr >> request;
}

void CMsgOsRequestDelta::Handle() {
	pgs->HandleOsRequestDelta(this);
}

// /os: stored 2 pamphlet
// |.42590   23 Feb 2001 23:17:39 pamphlet patzer   s8r16:l
// |.42661   24 Feb 2001 18:08:41 pamphlet ant      s8r16:l
void CMsgOsStored::In(std::istream& is) {
	COsStoredMatch sm;
	is >> nStored >> sLogin;
	while (is >> sm) {
		sms.push_back(sm);
	}
    if ((int)sms.size() != nStored)
        std::cerr << "[GGS] Warning: stored count mismatch, expected " << nStored << " got " << sms.size() << std::endl;
}

void CMsgOsStored::Handle() {
	pgs->HandleOsStored(this);
}

///////////////////////////////////
// CMsgOsTimeout
///////////////////////////////////

void CMsgOsTimeout::In(std::istream& is) {
	is >> idg >> sLogin >> std::ws;
}

void CMsgOsTimeout::Handle() {
	pgs->HandleOsTimeout(this);
}

/*
/os: top       1480.3@353.7      Inactive       AScore    Win   Draw   Loss 1308
|    1 leaflet  2103.9@ 33.8=   1.07:08:24+@ 31.3  +4.5    551     65    395
|    2 OO7      2096.5@ 33.4=     03:39:07+@ 33.1  +7.2  18943   1432  12866
|    3 HrtBrev  [recv]2496.5@119.2= 303.07:25:27+@ 69.4  +3.5    888     83    477
|    4 piglet   2441.4@114.4= 641.23:07:55+@ 31.1  +8.7   2987    132    629
|    5 zeclipse 2534.2@137.1=2037.01:42:33+@ 48.0 +10.2    237     26     81
|    6 slarin   2489.5@132.8= 708.13:1[recv]2:33+@ 71.7 +11.1    118     11     12
|    7 hannibal 2547.2@147.3= 644.01:58:35+@ 97.8  +8.8    473     52    133
|    8 booklet  2328.2@107.3= 316.04:20:44+@ 44.6  +5.7    350     47    121
|    9 scorpion 1945.6@ 32.0=   1.07:11:41+@ 29.3 +13.6  215[recv]67   1052  11066
|   10 Baboo    2325.6@116.0=  82.22:30:32+@ 91.0  +6.5    141     11     21
|   11 turtle   2338.0@115.0= 537.15:14:12+@ 41.9  +8.2   2015    320    518
|   12 scorp+   2009.6@ 51.4=   1.07:10:25+@ 49.8 +12.6  12298    809   7658
|   [recv]13 pvs+     2321.8@118.3= 371.08:47:33+@ 62.3  +6.7    148     10     19
|   14 rapsac   2425.5@142.0=1545.01:39:42+@ 68.7  +5.5    198     21    123
|   15 eclipse  2444.5@148.5=1441.22:22:07+@ 83.0  +5.4    619     71    394
|   16 doronko  2398.8@141[recv].5= 519.07:30:15+@ 93.2  -0.3    937    175    817
*/

void CMsgOsTop::In(std::istream& is) {
	std::string sInactive, sAScore, sWin, sDraw, sLoss;
	COsRankData rd;

	is >> rating >> sInactive >> sAScore >> sWin >> sDraw >> sLoss >> n >> std::ws;
	while (is >> rd) {
		rds.push_back(rd);
	}
}

void CMsgOsTop::Handle() {
	pgs->HandleOsTop(this);
}

// /os: trust-violation .56 pamphlet (*) delta= 4 + 0.7 secs
void CMsgOsTrustViolation::In(std::istream& is) {
	char c1, c2;
	std::string sDelta, sSecs;

	is >> idg >> sLogin >> c1 >> cColor >> c2;
    if (c1 != '(') { std::cerr << "[GGS] Warning: TrustViolation malformed, expected '('" << std::endl; return; }
    if (cColor != COsBoard::BLACK && cColor != COsBoard::WHITE && cColor != COsBoard::UNKNOWN) {
        std::cerr << "[GGS] Warning: TrustViolation unexpected color='" << cColor << "'" << std::endl; return;
    }
    if (c2 != ')') { std::cerr << "[GGS] Warning: TrustViolation malformed, expected ')'" << std::endl; return; }

	is >> sDelta >> delta1 >> c1 >> delta2 >> sSecs;
    if (sDelta != "delta=") { std::cerr << "[GGS] Warning: TrustViolation malformed, expected 'delta='" << std::endl; return; }
    if (sSecs != "secs") { std::cerr << "[GGS] Warning: TrustViolation malformed, expected 'secs'" << std::endl; return; }
}

void CMsgOsTrustViolation::Handle() {
	pgs->HandleOsTrustViolation(this);
}

///////////////////////////////////
// CMsgOsUndoRequest
///////////////////////////////////

void CMsgOsUndoRequest::In(std::istream& is) {
	std::string sDummy;

	is >> idg >> sLogin >> std::ws;
	std::getline(is, sDummy);

    if (sDummy != "is asking")
        std::cerr << "[GGS] Warning: UndoRequest unexpected text: '" << sDummy << "'" << std::endl;
}

void CMsgOsUndoRequest::Handle() {
	pgs->HandleOsUndoRequest(this);
}

///////////////////////////////////
// CMsgOsUnknown
///////////////////////////////////

CMsgOsUnknown::CMsgOsUnknown(const std::string& asMsgType) {
	sMsgType=asMsgType;
}

void CMsgOsUnknown::In(std::istream& is) {
    // Read all remaining stream content
    std::ostringstream oss;
    oss << is.rdbuf();
    sText = oss.str();
}

void CMsgOsUnknown::Handle() {
	pgs->HandleOsUnknown(this);
}

///////////////////////////////////
// CMsgOsUpdate
///////////////////////////////////

void CMsgOsUpdate::In(std::istream& is) {
	is >> idg >> mli;
}

void CMsgOsUpdate::Handle() {
	pgs->HandleOsUpdate(this);
	//GetMoveIfNeeded(PGame(idg), *pgs, idg);
}

///////////////////////////////////
// CMsgOsWatch
///////////////////////////////////

// /os: watch 1 : .41(1)
void CMsgOsWatch::In(std::istream& is) {
	char c1, c2;
	std::string idm;
	int nWatchers;

	is >> nMatches >> c1;
    if (c1 != ':') { std::cerr << "[GGS] Warning: CMsgOsWatch malformed, expected ':'" << std::endl; return; }

	while (is >> idm >> c1 >> nWatchers >> c2) {
        if (c1 != '(') { std::cerr << "[GGS] Warning: CMsgOsWatch malformed, expected '('" << std::endl; break; }
        if (c2 != ')') { std::cerr << "[GGS] Warning: CMsgOsWatch malformed, expected ')'" << std::endl; break; }
        if (idToNWatchers.find(idm) != idToNWatchers.end())
            std::cerr << "[GGS] Warning: duplicate watch id='" << idm << "', overwriting" << std::endl;
		idToNWatchers[idm]=nWatchers;
	}
}

void CMsgOsWatch::Handle() {
	pgs->HandleOsWatch(this);
}

///////////////////////////////////
// CMsgOsWatchChange
///////////////////////////////////

CMsgOsWatchDelta::CMsgOsWatchDelta(bool afPlus, const std::string& asLogin) {
	fPlus=afPlus;
	sLogin=asLogin;
}

// /os:  + n3 watch .44
void CMsgOsWatchDelta::In(std::istream& is) {
	is >> idm;
}

void CMsgOsWatchDelta::Handle() {
	pgs->HandleOsWatchDelta(this);
}

///////////////////////////////////
// CMsgOsWho
///////////////////////////////////

void CMsgOsWho::Handle() {
	pgs->HandleOsWho(this);
}

/*
/os: who    26      change:    win    draw    loss         match(es)
|tit4tat  + 1145.7@301.5 ->   +14.3   -12.1   -38.4 @ 79.6 
|n2       + 1384.1@ 80.5 
|ant      + 1426.2@ 50.8 ->   +39.2    +4.2   -30.8 @ 78.5 
etc.
*/

void CMsgOsWho::In(std::istream& is) {
	std::string s;
	is >> n;
	std::getline(is, s);

	COsWhoItem wi;
	while (is >> wi) {
		wis.push_back(wi);
	}

	// This assert fails because GGS sends the wrong n
	//	assert(wis.size()==n);
}
