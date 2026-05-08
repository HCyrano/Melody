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

#ifndef ODK_GGSMESSAGE_HPP
#define ODK_GGSMESSAGE_HPP

#include <map>

#include "GGSObjects.hpp"

// base message class

class ggsstream;

class CMsg {
public:
	// handle the message
	virtual void Handle()=0;
	virtual void In(std::istream& is);
    
    virtual ~CMsg() = default;

	ggsstream* pgs;
	std::string sFrom;
	std::string sRawText;
};

class CMsgGGSAlias: public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	int nAlias1, nAlias2;
	std::vector<CGGSAlias> valiases;
};

class CMsgGGSErr: public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	enum { kErrUnknown=0x8400, kErrCommandNotRecognized } err;
};

class CMsgGGSFinger: public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	std::map<std::string, std::string> keyToValue;
};

class CMsgGGSHelp: public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	std::string sText;
};

class CMsgGGSUnknown: public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	std::string sMsgType;
	std::string sText;
};

class CMsgGGSUserDelta: public CMsg {
public:
	CMsgGGSUserDelta(bool fPlus);

	virtual void Handle();
	void In(std::istream& is);

	bool fPlus;
	std::string sLogin;
};

class CMsgGGSTell : public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	std::string sText;
};

class CMsgGGSWho : public CMsg {
public:
	virtual void Handle();
	void In(std::istream& is);

	int nUsers;
	std::vector<CGGSWhoUser> wus;
};

// Fake messages

class CMsgGGSDisconnect : public CMsg {
public:
	virtual void Handle();
};

class CMsgGGSLogin : public CMsg {
public:
	virtual void Handle();
};

#endif // ODK_GGSMESSAGE_HPP
