/*
 * Copyright (C) 2026  Bruno Causse
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

#include "RXSearch.hpp"

void RXBestMove::to_COsMoveListItem(COsMoveListItem& mli) {
	
	if (position == PASS) {
		mli.mv.fPass = true;
		mli.mv.row = -1;
		mli.mv.col = -1;
		
	} else {
		mli.mv.fPass = false;
		mli.mv.row = 7 - static_cast<int>(floor(position/8));
		mli.mv.col = 7 - position%8;
	}
	
	mli.dEval = static_cast<double>(score);
	mli.tElapsed = tElapsed;
	
}
