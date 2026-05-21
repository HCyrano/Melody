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

#ifndef RXSEARCH_HPP
#define RXSEARCH_HPP

#include "RXBBPatterns.hpp"
#include "RXHashTable.hpp"
#include "OsObjects.hpp"

class RXBestMove {

    public :
    
	unsigned char position;
	int depth;
	int score;
	int selectivity;
	double tElapsed;
	
	unsigned long long nodes;
	
	void to_COsMoveListItem(COsMoveListItem& mli);
	
};


class RXSearch {
	
    public :
	
	enum t_client {kPrivate, kGGSMode, kIOStd};
	
	//parameters Engine
	t_client clientMode;
	
	unsigned int idEngine;
	int nThreads;
	RXHashTable* htable;
	RXHashTable* main_PV;
	RXHashTable* expected_PV;
	

	//int depth_incr;

	bool search_on_opponent_time;
	
	
	//parameters times
	bool dependent_time;

	int tRemaining;
	
	//parameters search
	RXBBPatterns sBoard;
    
	int alpha;
	int beta;
	
	int depth;
    int selectivity;

	//parameters best move
	RXBestMove bestMove;	
	
};





#endif
