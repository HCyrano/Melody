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

#ifndef RXMOVE_HPP
#define RXMOVE_HPP

#include <iostream>
#include <string>

#include "RXConstantes.hpp"
#include "RXPattern.hpp"


class RXMove {
	
	public :

    unsigned char position;
    unsigned long long square;
    unsigned long long flipped;
    int score;
    char selectivity;
    char depth;
    
    RXMove* next;
    
    RXPattern* pattern;
    RXPattern* undo_pattern;

		RXMove() : position(static_cast<char>(NOMOVE)), square(0ULL), flipped(0ULL), score(0), next(nullptr), pattern(nullptr), undo_pattern(nullptr) {
			pattern = new RXPattern();
		};
		
		RXMove(const RXMove& src) {
			//std::cout << "constructeur de copie RXMove" << std::endl;
			pattern = new RXPattern();
			
			*pattern = *(src.pattern); //copy

		};
		
		RXMove& operator=(const RXMove& src) {
			//std::cout << "affectation par copie de RXMove" << std::endl;
			if(this != &src) {

				*pattern = *(src.pattern); //copy

			}
			return *this;
		};

			
		~RXMove() {
			//std::cout << "destructeur RXMove" << std::endl;
			delete pattern;
		};

		
		//static std::string COsMove_to_coord(COsMove& move);		
		//static void index_to_COsMove(int index, COsMove& move);
		static std::string index_to_coord(int index);
		static int coord_to_index(std::string coord);
        static unsigned long long random_pick_bit_in_legalmoves(const unsigned long long legal_moves);

		void sort_bestmove(const unsigned int bestmove);
		void sort_by_score();
    
        RXMove* pick_next_promising_move();

		//debug
		friend std::ostream& operator<<(std::ostream& os, RXMove* list);

};

__attribute__((always_inline))
inline void RXMove::sort_bestmove(unsigned int bestmove) {

	RXMove* previous;
	for(RXMove* iter = (previous = this)->next; iter != nullptr; iter = (previous = iter)->next)
		if(iter->position == static_cast<char>(bestmove)) {
			previous->next = iter->next;
			iter->next = this->next;
			this->next = iter;
			break;
		}
}

// from smallest to biggest
__attribute__((always_inline))
inline void RXMove::sort_by_score() {
	RXMove *best, *previousBest, *previous;
	
	for(RXMove* iter = this; iter->next != nullptr; iter = iter->next){
		previousBest = iter;
		for(previous = previousBest->next; previous->next != nullptr; previous =previous->next)
			if(previousBest->next->score>previous->next->score)
				previousBest = previous;
				
		if(previousBest != iter) {
			best = previousBest->next;
			previousBest->next = best->next;
			best->next = iter->next;
			iter->next = best;
		}
	}		
}

__attribute__((always_inline))
inline RXMove* RXMove::pick_next_promising_move() {
    
    RXMove* previous_move = this;
    RXMove* move = previous_move->next;
    
    RXMove* previous_iter = move;
    for(RXMove* iter = previous_iter->next ; iter != nullptr; iter = (previous_iter = iter)->next) {
        if(iter->score < move->score) {
            move = iter;
            previous_move = previous_iter;
        }
    }
    
    if(previous_move != this) {
        //move to front
        previous_move->next = move->next;
        move->next = this->next;
        this->next = move;
    }
    
    return move;

}


#endif
