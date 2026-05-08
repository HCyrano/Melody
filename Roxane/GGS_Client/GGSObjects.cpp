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
