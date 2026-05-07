// Copyleft 2001 Chris Welty
//	All Rights Reserved

#include <sstream>
#include <iomanip>

#include "ggsstream.hpp"
#include "GGSMessage.hpp"




///////////////////////////////////
// CMsg
///////////////////////////////////

void CMsg::In(std::istream& is) {
}

///////////////////////////////////
// CMsgGGSAlias
///////////////////////////////////

void CMsgGGSAlias::In(std::istream& is) {
	char c;
	CGGSAlias alias;

    is >> nAlias1 >> c >> nAlias2;
    if (c != '/') {
        std::cerr << "[GGS] Warning: malformed alias message, expected '/'" << std::endl;
        return;
    }
    
    while (is >> alias)
        valiases.push_back(alias);

    if (nAlias2 != (int)valiases.size()) {
        std::cerr << "[GGS] Warning: alias count mismatch, expected "
                  << nAlias2 << " got " << valiases.size() << std::endl;
    }
}

void CMsgGGSAlias::Handle() {
	pgs->HandleGGSAlias(this);
}

///////////////////////////////////
// CMsgGGSDisconnect
///////////////////////////////////

void CMsgGGSDisconnect::Handle() {
	pgs->HandleGGSDisconnect();
}

///////////////////////////////////
// CMsgGGSErr
///////////////////////////////////

void CMsgGGSErr::In(std::istream& is) {
    std::string sLine;
	getline(is, sLine);
	if (sLine.find("not recognized")!=std::string::npos)
		err=kErrCommandNotRecognized;
	else
		err=kErrUnknown;
}

void CMsgGGSErr::Handle() {
	pgs->HandleGGSErr(this);
}

///////////////////////////////////
// CMsgGGSFinger
///////////////////////////////////

void CMsgGGSFinger::In(std::istream& is) {
    std::string sLine, sKey, sValue;

	is >> std::ws;

	// first section, key : value
	while (getline(is, sLine)) {
		if (!is)
			break;
		if (sLine.find(':')==std::string::npos)
			break;

        std::istringstream isLine(sLine.c_str());

		// get key and strip terminal spaces
		getline(isLine, sKey, ':');
        
        size_t pos = sKey.find_last_not_of(' ');
        if (pos != std::string::npos)
            sKey.resize(pos + 1);
        else
            sKey.clear();  // Que des espaces
        
		// get value. No value means this is the separator row
		//	between the first section and the second section
		isLine >> std::ws;
        std::getline(isLine, sValue);

		// insert (key, value) pair
        std::map<std::string,std::string>::iterator i=keyToValue.find(sKey);
		if (i==keyToValue.end())
			keyToValue[sKey]=sValue;
		else {
			(*i).second+="\n";
			(*i).second+=sValue;
		}
	}

}

void CMsgGGSFinger::Handle() {
	pgs->HandleGGSFinger(this);
}

///////////////////////////////////
// CMsgGGSHelp
///////////////////////////////////

void CMsgGGSHelp::In(std::istream& is) {
    std::ostringstream oss;
    oss << is.rdbuf();
    sText = oss.str();
}

void CMsgGGSHelp::Handle() {
	pgs->HandleGGSHelp(this);
}

///////////////////////////////////
// CMsgGGSLogin
///////////////////////////////////

void CMsgGGSLogin::Handle() {
	pgs->HandleGGSLogin();
}

///////////////////////////////////
// CMsgGGSTell
///////////////////////////////////

void CMsgGGSTell::In(std::istream& is) {
    std::ostringstream oss;
    oss << is.rdbuf();
    sText = oss.str();
}

void CMsgGGSTell::Handle() {
	pgs->HandleGGSTell(this);
}

///////////////////////////////////
// CMsgGGSUnknown
///////////////////////////////////

void CMsgGGSUnknown::In(std::istream& is) {
    // Read until end of stream
    std::ostringstream oss;
    oss << is.rdbuf();
    sText = oss.str();
}

void CMsgGGSUnknown::Handle() {
	pgs->HandleGGSUnknown(this);
}

///////////////////////////////////
// CMsgGGSUserDelta
///////////////////////////////////

CMsgGGSUserDelta::CMsgGGSUserDelta(bool afPlus) {
	fPlus=afPlus;
}

void CMsgGGSUserDelta::In(std::istream& is) {
	is >> sLogin;
}

void CMsgGGSUserDelta::Handle() {
	pgs->HandleGGSUserDelta(this);
}

///////////////////////////////////
// CMsgGGSWho
///////////////////////////////////

void CMsgGGSWho::In(std::istream& is) {
	CGGSWhoUser wu;

	is >> nUsers;
	is.ignore(1000, '\n');
	while (is >> wu) {
		wus.push_back(wu);
	}

    if ((int)wus.size() != nUsers) {
        std::cerr << "[GGS] Warning: who user count mismatch, expected "
                  << nUsers << " got " << wus.size() << std::endl;
    }
}

void CMsgGGSWho::Handle() {
	pgs->HandleGGSWho(this);
}

