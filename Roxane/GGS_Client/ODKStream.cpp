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

#include <iostream>
#include <unistd.h>

#include "ODKStream.hpp"
#include "GGSMessage.hpp"
#include "OsMessage.hpp"
#include "RXRoxane.hpp"
#include "types.hpp"

// Display message in console
void CODKStream::HandleGGS(const CMsg* pmsg) {
    std::cout << pmsg->sRawText << "\n";
}

void CODKStream::HandleGGSLogin() {
	BaseGGSLogin();
	(*this) << "mso\n";
	flush();
}

void CODKStream::HandleGGSTell(const CMsgGGSTell* pmsg) {
    std::cout << pmsg->sFrom << " " << pmsg->sText << "\n";
	
    //join
	/**********************************************************************/
	if(	(pmsg->sFrom=="romano"
		 || pmsg->sFrom=="ohr"
		 || pmsg->sFrom=="delorme") && ((pmsg->sText.substr(0,11) == "t /td join ") || (pmsg->sText.substr(0,14) == "tell /td join "))) {
		
		(*this) << pmsg->sText << "\n";
		flush();
	}
	/**********************************************************************/
	
	if(pmsg->sFrom=="HCyrano") {
		if (pmsg->sText=="quit") {
            if(pComputer != nullptr)
                pComputer->resume();
			Logout();
		} else {
			(*this) << pmsg->sText << "\n";
			flush();
		}
	}
}

void CODKStream::HandleGGSUnknown(const CMsgGGSUnknown* pmsg) {
    std::cout << "Unknown GGS message: \n";
	HandleGGS(pmsg);
}

void CODKStream::HandleOsJoin(const CMsgOsJoin* pmsg) {
	ggsstream::BaseOsJoin(pmsg);
	GetMoveIfNeeded(pmsg->idg);
}

void CODKStream::HandleOsLogin() {
	BaseOsLogin();
	(*this) << "ts trust +\n"
			<< "tell /os open 1\n"; /* open 0 for tournament */
	flush();
}

// handler
// this code is executed when a game change state
// even if the match terminates abnormally (e.g. one player leaves).
void CODKStream::HandleOsMatchDelta(const CMsgOsMatchDelta* pmsg) {
 
    //if my game
    if (pmsg->match.IsPlaying(GetLogin()) && pComputer!=nullptr) {
        
        if (pmsg->fPlus) { // this code is executed when game begins

            //vide les hash
            pComputer->resume();

        } else { // this code is executed when game ends
            
            pComputer->resume();
        }

            
     }
 
	BaseOsMatchDelta(pmsg);
}


//message .end
void CODKStream::HandleOsEnd(const CMsgOsEnd *pmsg) {
    
    BaseOsEnd(pmsg);
}

void CODKStream::HandleOsTimeout(const CMsgOsTimeout* pmsg){
    
    std::cout  << "timeout: " << pmsg->idg << " " << pmsg->sLogin  << std::endl;
    
    //COsGame* pgame=PGame(pmsg->idg);
    //Adjournes [.match]
    if(pmsg->sLogin == GetLogin()) {
        (*this) << "t /os break " << pmsg->idg << "\n";
        flush();
        BaseOsGameOver(pmsg->idg);
    }
    

}

void CODKStream::HandleOsFatalTimeout(const CMsgOsFatalTimeout* pmsg) {

    std::cout  << "fatal-timeout: " << pmsg->idg << " " << pmsg->sLogin  << std::endl;

    COsGame* pgame=PGame(pmsg->idg);
    if (pgame!=nullptr && pComputer!=nullptr)
        pComputer->stop_engine(pgame);
 
    // If it's our timeout
    if (pmsg->sLogin == GetLogin()) {
        // Resign the game
        (*this) << "t /os resign " << pmsg->idg << "\n";
        flush();
        BaseOsGameOver(pmsg->idg);
        
        std::cout << "[FATAL] Forcing disconnect to trigger reconnect..." << std::endl;
        
        ForceDisconnect();

        // while(get(c)) will exit immediately
        // → TryReconnect() will be triggered
    }

}



  // Example handler from Roxane:
void CODKStream::HandleOsRequestDelta(const CMsgOsRequestDelta* pmsg) {
	BaseOsRequestDelta(pmsg);

	if (pmsg->fPlus && pmsg->IAmChallenged()) {
		if (pmsg->RequireBoardSize(8) && pmsg->RequireKomi(false) && pmsg->RequireAnti(false) &&
			pmsg->RequireRated(true) && pmsg->RequireSynch(true) &&
			pmsg->RequireRand(true) && pmsg->RequireMaxRandDiscs(24) && pmsg->RequireMinRandDiscs(14) &&
			pmsg->RequireMaxOpponentClock(COsClock(30*60,0,2*60)) &&
			pmsg->RequireMinMyClock(COsClock(60,0,0)))
			(*this) << "t /os accept " << pmsg->idr << "\n";
		else
			(*this) << "t /os decline " << pmsg->idr << "\n";

		flush();
	}
}



void CODKStream::HandleOsGameOver(const CMsgOsMatchDelta* pmsg,const std::string& idg) {
    BaseOsGameOver(idg);
}


void CODKStream::HandleOsUnknown(const CMsgOsUnknown* pmsg) {
    std::cout << "Unknown /os message: ";
	HandleOs(pmsg);
}

void CODKStream::HandleOsUpdate(const CMsgOsUpdate* pmsg) {
	BaseOsUpdate(pmsg);
	GetMoveIfNeeded(pmsg->idg);
}

// helper function for join and update messages
void CODKStream::GetMoveIfNeeded(const std::string& idg) {
        
	COsGame* pgame=PGame(idg);
	if (pgame!=nullptr) {
	

		bool fMyMove=pgame->ToMove(GetLogin());
		//COsMoveListItem mli;

		if (fMyMove  && pComputer!=nullptr) {
		
			pComputer->get_move(idg, pgame);
		
		}
	}
}

void CODKStream::SendMove(const std::string& idg, COsMoveListItem& mli) {
	(*this) << "tell /os play " << idg << " " << mli << "\n";
	flush();

}

void CODKStream::SendMsg(const std::string& msg) {

	if(IsConnected()) {
		(*this) << "tell ." << GetLogin() << " " << msg << std::endl;
		flush();
	} else {
		std::cout << msg << std::endl;
	}

}
