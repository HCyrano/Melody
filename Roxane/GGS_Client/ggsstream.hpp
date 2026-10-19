/*
 * Copyright (C) 2001  Chris Welty
 * Copyright (C) 2026  bruno Causse (modified)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef ODK_GGSSTREAM_HPP
#define ODK_GGSSTREAM_HPP

#include <set>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <cstdint>


#include "sockbuf.hpp"
#include "GGSMessage.hpp"
#include "OsMessage.hpp"
#include "OsObjects.hpp"

class CMsg;
class COs;

class ggsstream : public std::iostream {
public:
    // construction/destruction
    ggsstream();
    virtual ~ggsstream();
    
    // Connection, disconnection
    virtual int Connect(const std::string& sServer, uint16_t nPort);
    virtual int Disconnect();
    
    // login, logout
    virtual int Login(const char* sLogin, const char* sPwd);
    virtual int Logout();
    virtual const std::string& GetLogin() const;
    virtual const std::string& GetPassword() const;
    
    // turn stream data into messages
    virtual void Process();
    
    // Information
    virtual bool IsConnected() const;
    virtual bool IsLoggedIn() const;
    virtual bool HasOthelloServer() const;
    
    // return an error description
    static const char* ErrText(int err);
    enum {
        kErrUnknown = 0x8200,
        kErrBadPassword,
        kErrLoggedIn,
        kErrLoggedOut,
        kErrConnected,
        kErrNotConnected,
        kErrNoStreambuf,
        kErrUserCancelled,
        kErrInvalidArg
    };
    
    // Handle messages relating to persistent data
    void BaseGGSLogin		();
    void BaseGGSUserDelta	(const CMsgGGSUserDelta* pmsg);
    void BaseGGSDisconnect	();
    
    void BaseOsEnd			(const CMsgOsEnd* pmsg);
    
    void BaseOsGameOver		(const std::string& idg);
    
    void BaseOsJoin			(const CMsgOsJoin* pmsg);
    void BaseOsLogin		();
    void BaseOsLogout		();
    void BaseOsMatch		(const CMsgOsMatch* pmsg);
    void BaseOsMatchDelta	(const CMsgOsMatchDelta* pmsg);
    void BaseOsRequestDelta	(const CMsgOsRequestDelta* pmsg);
    void BaseOsUpdate		(const CMsgOsUpdate* pmsg);
    
    // Handle messages
    virtual void HandleGGS				(const CMsg* pmsg);
    virtual void HandleGGSAlias			(const CMsgGGSAlias* pmsg);
    virtual void HandleGGSDisconnect	();
    virtual void HandleGGSErr			(const CMsgGGSErr* pmsg);
    virtual void HandleGGSFinger		(const CMsgGGSFinger* pmsg);
    virtual void HandleGGSHelp			(const CMsgGGSHelp* pmsg);
    virtual void HandleGGSLogin			();
    virtual void HandleGGSTell			(const CMsgGGSTell* pmsg);
    virtual void HandleGGSUnknown		(const CMsgGGSUnknown* pmsg);
    virtual void HandleGGSUserDelta		(const CMsgGGSUserDelta* pmsg);
    virtual void HandleGGSWho			(const CMsgGGSWho* pmsg);
    
    virtual void HandleOs				(const CMsgOs* pmsg);
    virtual void HandleOsAbortRequest	(const CMsgOsAbortRequest* pmsg);
    virtual void HandleOsComment		(const CMsgOsComment* pmsg);
    virtual void HandleOsEnd			(const CMsgOsEnd* pmsg);
    virtual void HandleOsErr			(const CMsgOsErr* pmsg);
    virtual void HandleOsFatalTimeout	(const CMsgOsFatalTimeout* pmsg);
    virtual void HandleOsFinger			(const CMsgOsFinger* pmsg);
    virtual void HandleOsGameOver       (const CMsgOsMatchDelta* pmsg, const std::string& idg);
    virtual void HandleOsHistory		(const CMsgOsHistory* pmsg);
    virtual void HandleOsJoin			(const CMsgOsJoin* pmsg);
    virtual void HandleOsLogin			();
    virtual void HandleOsLogout			();
    virtual void HandleOsLook			(const CMsgOsLook* pmsg);
    virtual void HandleOsMatch			(const CMsgOsMatch* pmsg);
    virtual void HandleOsMatchDelta		(const CMsgOsMatchDelta* pmsg);
    virtual void HandleOsRank			(const CMsgOsRank* pmsg);
    virtual void HandleOsRatingUpdate	(const CMsgOsRatingUpdate* pmsg);
    virtual void HandleOsRequestDelta	(const CMsgOsRequestDelta* pmsg);
    virtual void HandleOsStored			(const CMsgOsStored* pmsg);
    virtual void HandleOsTimeout		(const CMsgOsTimeout* pmsg);
    virtual void HandleOsTop			(const CMsgOsTop* pmsg);
    virtual void HandleOsTrustViolation	(const CMsgOsTrustViolation* pmsg);
    virtual void HandleOsUndoRequest	(const CMsgOsUndoRequest* pmsg);
    virtual void HandleOsUnknown		(const CMsgOsUnknown* pmsg);
    virtual void HandleOsUpdate			(const CMsgOsUpdate* pmsg);
    virtual void HandleOsWatch			(const CMsgOsWatch* pmsg);
    virtual void HandleOsWatchDelta		(const CMsgOsWatchDelta* pmsg);
    virtual void HandleOsWho			(const CMsgOsWho* pmsg);
    
    // persistent
    std::map<std::string,COsGame> idToGame;
    std::map<std::string,COsMatch> idToMatch;
    std::map<std::string,COsRequest> idToRequest;
    
    COsGame* PGame(const std::string& idg);
    
    // New methods for auto-reconnection
    void EnableAutoReconnect(bool enable = true, int maxRetries = 5, int delayMs = 2000);
    void DisableAutoReconnect();
    bool IsAutoReconnectEnabled() const { return fAutoReconnect; }
    
    //for test reconnection
    //virtual void ForceDisconnect();
    
protected:
    virtual int await(const char* sAwait);
    
    // helper function for BaseOsMatchDelta
    virtual void EndGame(const CMsgOsMatchDelta* pmsg, const std::string& idg);
    
    // turn stream data into messages
    virtual void ProcessLine(std::string& sLine);
    virtual void ProcessMessage();
    
    // parse messages
    virtual CMsg* GetMsgType(std::istream& is);
    virtual CMsg* GetMsgTypeOs(std::istream& is);
    virtual CMsg* GetMsgTypeGGS(std::istream& is);
    
    // post messages
    virtual void Post(CMsg* pmsg);
    
    // Virtual callbacks - can be overridden in a subclass
    virtual void OnReconnecting(int attempt, int maxAttempts);
    virtual void OnReconnected();
    virtual void OnReconnectFailed();
    virtual void ForceDisconnect();
    
    
    bool fLoggedIn, fHasOs;
    std::string sLogin, sPassword;
    
private:
    

    std::string sMsg;
    sockbuf *psockbuf;
    
    bool fAutoReconnect = false;
    int nMaxRetries = 5;
    uint32_t nReconnectDelayMs = 2000;
    int nCurrentRetry = 0;
    
    std::string sLastServer;
    uint32_t nLastPort = 0;
    
    bool TryReconnect();
        
    // Membres pour le pulsateur (Heartbeat)
    std::atomic<bool> stopHeartbeat{false};
    std::mutex mtx;                 // Pour la condition_variable
    std::condition_variable cv;      // Pour l'attente intelligente
    std::thread heartbeatThread;
    void HeartbeatLoop();
    void StopHeartbeat();
};



#endif	//ODK_GGSSTREAM_HPP
