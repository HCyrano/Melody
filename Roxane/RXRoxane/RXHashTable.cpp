/*
 *  RXHashTable.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 18/05/05.
 *  Copyleft 2005-2025 personnel.
 *
 */
#include <iostream>
#include <new>
#include <sstream>
#include <algorithm>

#include "RXHashTable.hpp"
#include "RXEngine.hpp"

//RXHashValue::RXHashValue(unsigned long long packed) {
//	
//	lower = static_cast<short>(packed & 0x000000000000FFFFULL);
//	packed >>= 16;
//	upper = static_cast<short>(packed & 0x000000000000FFFFULL);
//	packed >>= 16;
//	move = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//	packed >>= 8;	
//	selectivity = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//	packed >>= 8;	
//	depth = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//	packed >>= 8;	
//	date = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//	
//}

RXHashValue::RXHashValue(uint64_t packed) {
    std::memcpy(this, &packed, sizeof(uint64_t));
}


RXHashTable::RXHashTable(unsigned int nBT) : table(0), nBitsTable(nBT),  _shared(true) {

	_offsetTable[HASH_SHARED] = 0;
	_offsetTable[HASH_BLACK] = 0;
	_offsetTable[HASH_WHITE] = 1<<(nBitsTable-1);
	
	_maskTable[HASH_SHARED] = (1<<nBitsTable)-1;
	_maskTable[HASH_BLACK] = (1<<(nBitsTable-1))-1;
	_maskTable[HASH_WHITE] = (1<<(nBitsTable-1))-1;
	
	table.resize(1UL<<nBitsTable);
		 
	date[0] = date[1] = 0;
    
    
}


void RXHashTable::shared(const bool flag) {
	_shared = flag;
	
	if (date[1]>date[0])
		date[0] = date[1];
	else
		date[1] = date[0];
		
}

bool RXHashTable::is_shared() const {
	return _shared;
}


void RXHashTable::reset() {
    

	_shared = true;
	
	const unsigned long capacity = 1UL<<nBitsTable;
	
	for(unsigned long i = 0; i<capacity; i++) {
		table[i].deepest.packed = 0x0ULL;
		table[i].newest.packed = 0x0ULL;
	}
	
	date[BLACK] = date[WHITE] = 0;
}


void RXHashTable::update(const unsigned long long hash_code, const t_hash type_hashtable, const unsigned char selectivity, const unsigned char depth, const int alpha, const int beta, const int score, const char move) {

	RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	
	RXHashRecord& deepest = entry.deepest;
	
	const unsigned long long deepest_lock     = deepest.lock;
	const unsigned long long deepest_packed	  = deepest.packed;
	const unsigned long long deepest_hashcode = deepest_lock ^ deepest_packed;
	
	RXHashValue deepest_value(deepest_packed);
	
	const unsigned int base_date = date[type_hashtable == HASH_WHITE? WHITE:BLACK];
    unsigned int _date = base_date;
    
	if(alpha < score && score < beta)
		++_date; //bonus for exact score
	
	
	/* try to update deepest entry */
	if (hash_code == deepest_hashcode && selectivity == deepest_value.selectivity  && depth == deepest_value.depth) {
        
		
		if (score < beta && score < deepest_value.upper) 
			deepest_value.upper = static_cast<short>(score);
        if (score > alpha) {
            deepest_value.move = move;
            
            if(score > deepest_value.lower)
                deepest_value.lower = static_cast<short>(score);
        }

        /* control if lower>upper : Instability */
		if(deepest_value.lower>deepest_value.upper) {

 			if(score<beta)
				deepest_value.upper = static_cast<short>(score);
			else
				deepest_value.upper = MAX_SCORE;
			
			if(score>alpha)
				deepest_value.lower = static_cast<short>(score);
			else
				deepest_value.lower = -MAX_SCORE;
            
            deepest_value.move = move;
            
		} else if(deepest_value.lower == deepest_value.upper) {
            if(_date == base_date)
                ++_date;
        }
		
		deepest_value.date = _date;
        		
		deepest.packed = deepest_value.wide_2_compact();
		deepest.lock   = hash_code ^ deepest.packed;
		
	} else {
		
		RXHashRecord& newest = entry.newest;
		
		const unsigned long long newest_packed   = newest.packed;
		const unsigned long long newest_hashcode = newest.lock ^ newest_packed;
		RXHashValue newest_value(newest_packed);
		
		
		/* else try to update newest entry */
		if (hash_code == newest_hashcode && selectivity == newest_value.selectivity  && depth == newest_value.depth) {
			
			if (score < beta && score < newest_value.upper)
				newest_value.upper = static_cast<short>(score);
            if (score > alpha) {
                newest_value.move =  move;
                
                if(score > newest_value.lower)
                    newest_value.lower =  static_cast<short>(score);
            }

            /* control if lower>upper : Instability */
			if(newest_value.lower>newest_value.upper) {

				if(score<beta)
					newest_value.upper = static_cast<short>(score);
				else
					newest_value.upper = MAX_SCORE;
				
				if(score>alpha)
					newest_value.lower = static_cast<short>(score);
				else
					newest_value.lower = -MAX_SCORE;
                
                newest_value.move =  move;

			} else if(newest_value.lower == newest_value.upper) {
                if(_date == base_date)
                    ++_date;
            }

			newest_value.date = _date;
            
            //implementation 2025-08-02 (en test)
            if(newest_value.date > deepest_value.date) {
                
                //copy
                newest.lock   = deepest_lock;
                newest.packed = deepest_packed;
                
                deepest.packed = newest_value.wide_2_compact();
                deepest.lock   = hash_code ^ deepest.packed;

            } else {
                
                newest.packed = newest_value.wide_2_compact();
                newest.lock   = hash_code ^ newest.packed;
            }

			
			/* else try to add to deepest entry */
		} else if (deepest_hashcode == hash_code ||  deepest_value.date <  _date ||
													(deepest_value.date == _date && ( deepest_value.depth < depth ||
													(deepest_value.depth == depth && deepest_value.selectivity < selectivity)))) { // priority
			
			if(deepest_hashcode != hash_code &&	(newest_hashcode == hash_code ||  newest_value.date <  deepest_value.date ||
																			  	 (newest_value.date == deepest_value.date && (newest_value.depth <  deepest_value.depth ||
																			  	 (newest_value.depth == deepest_value.depth && newest_value.selectivity <= deepest_value.selectivity))))) {
					
				//copy
				newest.lock   = deepest_lock;
				newest.packed = deepest_packed;
			}
			
			deepest_value.depth = depth;
			deepest_value.date = _date;
			deepest_value.selectivity = selectivity;
			deepest_value.lower = -MAX_SCORE;
			deepest_value.upper = +MAX_SCORE;
			if (score < beta) deepest_value.upper = static_cast<short>(score);
			if (score > alpha) deepest_value.lower = static_cast<short>(score);
            deepest_value.move =  move;

			deepest.packed = deepest_value.wide_2_compact();
			deepest.lock   = hash_code ^ deepest.packed;
			
			
			/* else add to newest entry */
		} else {
			
			newest_value.depth = depth;
			newest_value.date = _date;
			newest_value.selectivity = selectivity;
			newest_value.lower = -MAX_SCORE;
			newest_value.upper = +MAX_SCORE;
			if (score < beta) newest_value.upper = static_cast<short>(score);
			if (score > alpha) newest_value.lower = static_cast<short>(score);
            newest_value.move =  move;

			newest.packed = newest_value.wide_2_compact();
			newest.lock   = hash_code ^ newest.packed;
			
			
		}
	}
	
}

void RXHashTable::update(const unsigned long long hash_code, const t_hash type_hashtable, const unsigned char selectivity, const unsigned char depth, const int alpha, const int score, const char move) {


	RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	
	RXHashRecord& deepest = entry.deepest;
	
	const unsigned long long deepest_lock     = deepest.lock;
	const unsigned long long deepest_packed   = deepest.packed;
	const unsigned long long deepest_hashcode = deepest_lock ^ deepest_packed;
	
	RXHashValue deepest_value(deepest_packed);
	
	unsigned int _date = date[type_hashtable == HASH_WHITE? WHITE:BLACK];

	
	/* try to update deepest entry */
	if (hash_code == deepest_hashcode && selectivity == deepest_value.selectivity  && depth == deepest_value.depth) {

        if (score > alpha) {
            deepest_value.move =  move;
            
            if(score > deepest_value.lower)
                deepest_value.lower =  static_cast<short>(score);

        } else if(score < deepest_value.upper) {
            deepest_value.upper = static_cast<short>(score);
        }

        /* control if lower>upper : Instability */
        if(deepest_value.lower > deepest_value.upper) {

            if(score>alpha) {
                deepest_value.lower = static_cast<short>(score);
                deepest_value.upper = MAX_SCORE;
            } else {
                deepest_value.lower = -MAX_SCORE;
                deepest_value.upper = static_cast<short>(score);
            }
            
            deepest_value.move =  move;

        } else if(deepest_value.lower == deepest_value.upper) {
            ++_date;
        }
        
        deepest_value.date = _date;
						
		deepest.packed = deepest_value.wide_2_compact();
		deepest.lock   = hash_code ^ deepest.packed;
		
	} else {
		
		RXHashRecord& newest = entry.newest;
		
		const unsigned long long newest_packed   = newest.packed;
		const unsigned long long newest_hashcode = newest.lock ^ newest_packed;
		RXHashValue newest_value(newest_packed);
		
		
		/* else try to update newest entry */
        if (hash_code == newest_hashcode && selectivity == newest_value.selectivity  && depth == newest_value.depth) {
            
            if (score > alpha) {
                newest_value.move =  move;
                
                if(score > newest_value.lower)
                    newest_value.lower =  static_cast<short>(score);

            } else if(score < newest_value.upper) {
                newest_value.upper = static_cast<short>(score);
            }

            /* control if lower>upper : Instability */
            if(newest_value.lower > newest_value.upper) {
                
                 if(score>alpha) {
                    newest_value.lower = static_cast<short>(score);
                    newest_value.upper = MAX_SCORE;
                } else {
                    newest_value.lower = -MAX_SCORE;
                    newest_value.upper = static_cast<short>(score);
                }

                newest_value.move =  move;

            } else if(newest_value.lower == newest_value.upper) {
                ++_date;
            }

            newest_value.date = _date;
            
            //implementation 2025-08-02 (en test)
            if(newest_value.date > deepest_value.date) {
                
                //copy
                newest.lock   = deepest_lock;
                newest.packed = deepest_packed;
                
                deepest.packed = newest_value.wide_2_compact();
                deepest.lock   = hash_code ^ deepest.packed;

            } else {
                
                newest.packed = newest_value.wide_2_compact();
                newest.lock   = hash_code ^ newest.packed;
            }

			/* else try to add to deepest entry */
		} else if (deepest_hashcode == hash_code ||  deepest_value.date <  _date ||
													(deepest_value.date == _date && (deepest_value.depth < depth ||
													(deepest_value.depth == depth && deepest_value.selectivity < selectivity)))) { // priority
        
    
            
            
			if(deepest_hashcode != hash_code &&	(newest_hashcode == hash_code ||  newest_value.date <  deepest_value.date ||
																			  	 (newest_value.date == deepest_value.date && (newest_value.depth <  deepest_value.depth ||
																			  	 (newest_value.depth == deepest_value.depth && newest_value.selectivity <= deepest_value.selectivity))))) {
				
				//copy
				newest.lock   = deepest_lock;
				newest.packed = deepest_packed;
			}
			
			deepest_value.depth = depth;
			deepest_value.date = _date;
			deepest_value.selectivity = selectivity;
			if (score > alpha) {
				deepest_value.upper = MAX_SCORE;
				deepest_value.lower = static_cast<short>(score);
			} else {
				deepest_value.upper = static_cast<short>(score);
				deepest_value.lower = -MAX_SCORE;
			}
			deepest_value.move =  move;
			
			deepest.packed = deepest_value.wide_2_compact();
			deepest.lock   = hash_code ^ deepest.packed;
			
			
			/* else add to newest entry */
		} else {
			
			newest_value.depth = depth;
			newest_value.date = _date;
			newest_value.selectivity = selectivity;
			if (score > alpha) {
				newest_value.upper = MAX_SCORE;
				newest_value.lower = static_cast<short>(score);
			} else {
				newest_value.upper = static_cast<short>(score);
				newest_value.lower = -MAX_SCORE;
			}
			newest_value.move =  move;
			
			newest.packed = newest_value.wide_2_compact();
			newest.lock   = hash_code ^ newest.packed;
			
			
		}
	}	
	
}



std::string RXHashTable::line2String(RXBitBoard& board, const int depth, const t_hash type_hashtable) const {
    
    std::vector<unsigned char> pv;
	mainVariation(pv, board, type_hashtable, depth);
    
    std::ostringstream buffer;
    bool player = false;
    for(auto it : pv) {

        std::string coord = RXMove::index_to_coord(it);
        if(player)
            std::transform(coord.begin(), coord.end(), coord.begin(), ::tolower);
        buffer << coord << ' ';

        player = !player;
        
    }
    
	return buffer.str();
	
}

void RXHashTable::mainVariation(std::vector<unsigned char>& pv, RXBitBoard& board, const t_hash type_hashtable, const int depth) const {
	
	if(depth>0) {
		RXHashValue entry;
		if(get(board, type_hashtable, entry) && entry.move != NOMOVE) {
            
            pv.push_back(entry.move);
            
			if(entry.move == PASS) {
				board.do_pass();
				mainVariation(pv, board, type_hashtable, depth-1);
				board.do_pass();
			} else {
				RXMove& move = _move[board.n_empty][type_hashtable==HASH_WHITE? WHITE:BLACK]; //multithread, for shared use BLACK
				((board).*(board.generate_flips[entry.move]))(move);
				board.do_move(move);
				mainVariation(pv, board, type_hashtable, depth-1);
				board.undo_move(move);
			}
            
		} else {
            pv.push_back(NOMOVE);
			mainVariation(pv, board, type_hashtable, depth-1);
		}
	}
}



void RXHashTable::copyPV(RXHashTable* from_hash, const t_hash from_type_hash, RXBitBoard& board, const t_hash to_type_hash) {
	
	const RXHashRecord* from_Record = from_hash->get_record(board, from_type_hash);						// source
	if(from_Record != nullptr) {
		
		int square = from_Record->get_move();
		
		if(square != NOMOVE) {
			
			RXHashRecord& to_Record = table[_offsetTable[to_type_hash] |(static_cast<unsigned int>(board.hashcode()>>32) & _maskTable[to_type_hash])].deepest;	//dest
			
			if( to_Record.lock  != from_Record->lock) {
				to_Record.lock   = from_Record->lock;
				to_Record.packed = from_Record->packed;
				
			}
			
			if(square == PASS) {
				board.do_pass();
				copyPV(from_hash, from_type_hash, board, to_type_hash);
				board.do_pass();
			} else {
				RXMove& move = _move[board.n_empty][to_type_hash]; //SHARED 3/02/2025
				((board).*(board.generate_flips[square]))(move);
				board.do_move(move);
				copyPV(from_hash, from_type_hash, board, to_type_hash);
				board.undo_move(move);
			}
		}
	}
	
}




void RXHashTable::copyPV_shared_to_color(RXBitBoard& board, const int color) {

//	std::cout << "copyPV_shared_to_color" << std::endl;
//	std::cout << "source : " << line2String(board, 12, HASH_SHARED) << std::endl;

	if(color == BLACK) {
		copyPV(board, HASH_SHARED, HASH_BLACK);
	} else { //player == WHITE
		copyPV(board, HASH_SHARED, HASH_WHITE);
	}

//	std::cout << "dest   : " << line2String(board, 12, (color==BLACK? HASH_BLACK:HASH_WHITE)) << std::endl;

}

void RXHashTable::copyPV_color_to_shared (RXBitBoard& board, const int color) {

//	std::cout << "copyPV_color_to_shared" << std::endl;
//	std::cout << "source : " << line2String(board, 12, (color==BLACK? HASH_BLACK:HASH_WHITE)) << std::endl;
	
	if(color == BLACK) {
		copyPV(board, HASH_BLACK, HASH_SHARED);
	} else { //player == WHITE
		copyPV(board, HASH_WHITE, HASH_SHARED);
	}
	
//	std::cout << "dest   : " << line2String(board, 12, HASH_SHARED) << std::endl;

}

/* *********** Attention ***********/
/*         Aucun moteur actif      */
/* *********** Attention ***********/

void RXHashTable::copyPV(RXBitBoard& board, const t_hash from_hashtable, const t_hash to_hashtable) {
	
	
	const RXHashRecord* from_Record = get_record(board, from_hashtable);						// source
	if(from_Record != nullptr) {
		
		int square = from_Record->get_move();
		
		if(square != NOMOVE) {
			
			RXHashRecord& to_Record = table[_offsetTable[to_hashtable] |(static_cast<unsigned int>(board.hashcode()>>32) & _maskTable[to_hashtable])].deepest;	//dest
			
			if( to_Record.lock  != from_Record->lock) {
				to_Record.lock   = from_Record->lock;
				to_Record.packed = from_Record->packed;
				
			}
			
			if(square == PASS) {
				board.do_pass();
				copyPV(board, from_hashtable, to_hashtable);
				board.do_pass();
			} else {
				RXMove& move = _move[board.n_empty][to_hashtable]; //SHARED 03/02/2025
				((board).*(board.generate_flips[square]))(move);
				board.do_move(move);
				copyPV(board, from_hashtable, to_hashtable);
				board.undo_move(move);
			}
		}
	}
}

// *********** Attention ***********
//         Aucun moteur actif
// *********** Attention **********/

void RXHashTable::mergePV(RXBitBoard& board) {
	
//	std::cout << "merge PV" << std::endl;
	
	const RXHashRecord* from_RecordBlack = get_record(board, HASH_BLACK);		// source black
	const RXHashRecord* from_RecordWhite = get_record(board, HASH_WHITE); 		// source white
	
	//entry choice
	const RXHashRecord* from_Record = nullptr;
	if(from_RecordBlack != nullptr && from_RecordBlack->get_move() != NOMOVE) {
		from_Record = from_RecordBlack;
		if(from_RecordWhite!=nullptr && from_RecordWhite->get_move() != NOMOVE) {
			
			if(from_RecordWhite->get_depth() > from_RecordBlack->get_depth())
				from_Record = from_RecordWhite;
			else if((from_RecordWhite->get_depth() == from_RecordBlack->get_depth()) && from_RecordWhite->get_selectivity() > from_RecordBlack->get_selectivity())
				from_Record = from_RecordWhite;
			
		}
		
	} else if(from_RecordWhite != nullptr && from_RecordWhite->get_move() != NOMOVE) {
		from_Record = from_RecordWhite;
	}
	
	
	if(from_Record != nullptr) {
		
		RXHashRecord& to_Record = table[(static_cast<unsigned int>(board.hashcode()>>32) & _maskTable[HASH_SHARED])].deepest; 		//dest
		
		if( to_Record.lock  != from_Record->lock) {
			to_Record.lock   = from_Record->lock;
			to_Record.packed = from_Record->packed;
		}
		
		int square = from_Record->get_move();
		
		if( square == PASS) {
			board.do_pass();
			mergePV(board);
			board.do_pass();
		} else {
			RXMove& move = _move[board.n_empty][SHARED];						//shared
			((board).*(board.generate_flips[square]))(move);
			board.do_move(move);
			mergePV(board);
			board.undo_move(move);
		}
	}
}

void RXHashTable::protectPV(RXBitBoard& board) {

	t_hash type_hashtable = HASH_SHARED;
		
	if(!_shared) {
		if (board.player == BLACK)
			type_hashtable = HASH_BLACK;
		else
			type_hashtable = HASH_WHITE;
			
	}
	
//	std::cout << "protectPV" << std::endl;
//	std::cout << "source : " << line2String(board, 12, type_hashtable) << std::endl;
	
	protectPV(board, type_hashtable);
	
//	std::cout << "dest   : " << line2String(board, 12, type_hashtable) << std::endl;

}


void RXHashTable::protectPV(RXBitBoard& board, const t_hash	type_hashtable, const bool passed) {
	
    const unsigned long long hash_code = board.hashcode();
	
	RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	RXHashRecord& deepest = entry.deepest;
	RXHashRecord& newest  = entry.newest;
	
	
	if(hash_code == (newest.lock ^ newest.packed)) { //si newest
		//Swap deepest/newest
		const unsigned long long lock   = deepest.lock;
		const unsigned long long packed = deepest.packed;
		
		deepest.lock   = newest.lock;
		deepest.packed = newest.packed;
		
		newest.lock   = lock;
		newest.packed = packed;
		
	}
	
	if(hash_code == (deepest.lock ^ deepest.packed)) {
		
		int square = deepest.get_move();
		
		if(square != NOMOVE) {
			
			//update date
			deepest.packed = ((static_cast<unsigned long long>(date[type_hashtable == HASH_WHITE? WHITE:BLACK]) + 1)<<56) | (deepest.packed & 0x00FFFFFFFFFFFFFFULL);
			deepest.lock   = hash_code ^ deepest.packed;
			
			if(square == PASS) {
				if(!passed) {
					board.do_pass();
					protectPV(board, type_hashtable, true);
					board.do_pass();	
				}
			} else {
				RXMove& move = _move[board.n_empty][type_hashtable == HASH_WHITE? WHITE:BLACK]; //multithread BLACK or SHARED
				((board).*(board.generate_flips[square]))(move);
				board.do_move(move);
				protectPV(board, type_hashtable, false);
				board.undo_move(move);
			}
		}
	}
}


