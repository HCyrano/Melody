/*
 *  RXSearch.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 07/10/09.
 *  Copyleft 2009-2025 personnel.
 *
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
