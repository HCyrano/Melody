// Copyleft 2001 Chris Welty
//	All Rights Reserved

#ifndef ODK_GGSOBJECTS_HPP
#define ODK_GGSOBJECTS_HPP

#include <iostream>
#include <string>
#include <vector>



class CGGSAlias {
public:
	std::string sAlias, sExpansion;

	void In(std::istream& is);
/*
	bool operator<(const CGGSAlias& b) const { return this<&b ; }
	bool operator==(const CGGSAlias& b) const { return this==&b; }
*/
    
    bool operator<(const CGGSAlias& b) const {
        return sAlias < b.sAlias;
    }

    bool operator==(const CGGSAlias& b) const {
        return sAlias == b.sAlias && sExpansion == b.sExpansion;
    }
};

inline std::istream& operator>>(std::istream& is, CGGSAlias& alias) {alias.In(is); return is; }

class CGGSWhoUser {
public:
	std::string sLogin;
	char cRegistered;
	std::string sIdle, sOnline, sIPAddr, sHostName;

	void In(std::istream& is);

	bool operator<(const CGGSWhoUser& b) const;
	bool operator==(const CGGSWhoUser& b) const { return sLogin==b.sLogin; }
private:
	int RegisteredSortOrder() const;
};

inline std::istream& operator>>(std::istream& is, CGGSWhoUser& wu) { wu.In(is); return is; }

#endif	//ODK_GGSOBJECTS_HPP
