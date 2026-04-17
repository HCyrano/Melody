// Copyleft 2001 Chris Welty
//	All Rights Reserved

#include <sstream>


#include "types.hpp"
#include "GGSObjects.hpp"

void CGGSAlias::In(std::istream& is) {
	is >> sAlias >> std::ws;
	getline(is, sExpansion);
}

void CGGSWhoUser::In(std::istream& is) {
    std::string sLine;
    getline(is, sLine);
    std::istringstream isl(sLine);
    isl >> sLogin >> cRegistered >> sIdle >> sOnline >> sIPAddr >> sHostName;
}

int CGGSWhoUser::RegisteredSortOrder() const {
	switch(cRegistered) {
	case 'h':
		return 0;
	case 'p':
		return 2;
	case '.':
		return 3;
	default:
		return 1;
	}
}

// funky ordering is used by Lion
bool CGGSWhoUser::operator<(const CGGSWhoUser& b) const {
	if (RegisteredSortOrder()!=b.RegisteredSortOrder())
		return RegisteredSortOrder()<b.RegisteredSortOrder();
	else
		return sLogin<b.sLogin;
}
