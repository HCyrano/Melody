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
