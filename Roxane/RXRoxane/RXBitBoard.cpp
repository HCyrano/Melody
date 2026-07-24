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


#include "RXBitBoard.hpp"
#include "RXTools.hpp"

#if ARCH == ARCH_X86_AVX2
    #include "RXBBDoFlips_AVX2.cpp"
    #include "RXBBCountFlips_AVX2.cpp"
#elif ARCH == ARCH_ARM_NEON
    #include "RXBBDoFlips_NEON.cpp"
    #include "RXBBCountFlips_NEON.cpp"
#else
    #error "Unsupported architecture — define ARCH in RXSetting.hpp"
#endif




const unsigned long long RXBitBoard::NEIGHBOR[] = { 
 0X0000000000000302ULL, 0X0000000000000604ULL, 0X0000000000000E0AULL, 0X0000000000001C14ULL, 0X0000000000003828ULL, 0X0000000000007050ULL, 0X0000000000006020ULL, 0X000000000000C040ULL,
 0X0000000000030200ULL, 0X0000000000060400ULL, 0X00000000000E0A00ULL, 0X00000000001C1400ULL, 0X0000000000382800ULL, 0X0000000000705000ULL, 0X0000000000602000ULL, 0X0000000000C04000ULL,
 0X0000000003020300ULL, 0X0000000006040600ULL, 0X000000000E0A0E00ULL, 0X000000001C141C00ULL, 0X0000000038283800ULL, 0X0000000070507000ULL, 0X0000000060206000ULL, 0X00000000C040C000ULL,
 0X0000000302030000ULL, 0X0000000604060000ULL, 0X0000000E0A0E0000ULL, 0X0000001C141C0000ULL, 0X0000003828380000ULL, 0X0000007050700000ULL, 0X0000006020600000ULL, 0X000000C040C00000ULL,
 0X0000030203000000ULL, 0X0000060406000000ULL, 0X00000E0A0E000000ULL, 0X00001C141C000000ULL, 0X0000382838000000ULL, 0X0000705070000000ULL, 0X0000602060000000ULL, 0X0000C040C0000000ULL,
 0X0003020300000000ULL, 0X0006040600000000ULL, 0X000E0A0E00000000ULL, 0X001C141C00000000ULL, 0X0038283800000000ULL, 0X0070507000000000ULL, 0X0060206000000000ULL, 0X00C040C000000000ULL,
 0X0002030000000000ULL, 0X0004060000000000ULL, 0X000A0E0000000000ULL, 0X00141C0000000000ULL, 0X0028380000000000ULL, 0X0050700000000000ULL, 0X0020600000000000ULL, 0X0040C00000000000ULL,
 0X0203000000000000ULL, 0X0406000000000000ULL, 0X0A0E000000000000ULL, 0X141C000000000000ULL, 0X2838000000000000ULL, 0X5070000000000000ULL, 0X2060000000000000ULL, 0X40C0000000000000ULL
};

/* order JWC */
const int RXBitBoard::PRESORTED_POSITION[] = {
	A1, A8, H1, H8,						// Corner
	A3, A6, C1, C8, F1, F8, H3, H6,		// A 
	C3, C6, F3, F6,						// D
	A4, A5, D1, D8, E1, E8, H4, H5,		// B
	C4, C5, D3, D6, E3, E6, F4, F5,		// E
	B4, B5, D2, D7, E2, E7, G4, G5,		// G
	B3, B6, C2, C7, F2, F7, G3, G6,		// F
	A2, A7, B1, B8, G1, G8, H2, H7,		// C
	B2, B7, G2, G7						// X
};


/** quadrant id to move mask */
const unsigned long long RXBitBoard::QUADRANT_MASK[] = {
    0x0000000000000000, 0x000000000F0F0F0F, 0x00000000F0F0F0F0, 0x00000000FFFFFFFF,
    0x0F0F0F0F00000000, 0x0F0F0F0F0F0F0F0F, 0x0F0F0F0FF0F0F0F0, 0x0F0F0F0FFFFFFFFF,
    0xF0F0F0F000000000, 0xF0F0F0F00F0F0F0F, 0xF0F0F0F0F0F0F0F0, 0xF0F0F0F0FFFFFFFF,
    0xFFFFFFFF00000000, 0xFFFFFFFF0F0F0F0F, 0xFFFFFFFFF0F0F0F0, 0xFFFFFFFFFFFFFFFF
};


/*! a quadrant id for each square */
const int RXBitBoard::QUADRANT_SHIFT[] = {
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 1, 1, 1,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3,
	2, 2, 2, 2, 3, 3, 3, 3
};

/*! a quadrant id for each square */
const int RXBitBoard::QUADRANT_ID[] = {
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 1, 2, 2, 2, 2,
    1, 1, 1, 1, 2, 2, 2, 2,
    4, 4, 4, 4, 8, 8, 8, 8,
    4, 4, 4, 4, 8, 8, 8, 8,
    4, 4, 4, 4, 8, 8, 8, 8,
    4, 4, 4, 4, 8, 8, 8, 8,
};


unsigned char RXBitBoard::EDGE_STABILITY[256*256]; //unsigned char



//init edge stability table (edax)

int RXBitBoard::find_edge_stable(const int old_P, const int old_O, int stable) {
    int P, O, x, y;
    const int E = ~(old_P | old_O); // empties

    stable &= old_P; // mask stable squares with remaining player squares.
    if (!stable || E == 0) return stable;

    for (x = 0; x < 8; ++x) {
        if (E & 1<<x) { //is x an empty square ?
            O = old_O;
            P = old_P | 1<<x; // player plays on it

            
            if (x > 1) { // flip left discs
                for (y = x - 1; y > 0 && (O & 1<<y); --y) ;
                if (P & 1<<y) {
                    for (y = x - 1; y > 0 && (O & 1<<y); --y) {
                        O ^= 1<<y; P ^= 1<<y;
                    }
                }
            }
            if (x < 6) { // flip right discs
                for (y = x + 1; y < 8 && (O & 1<<y); ++y) ;
                if (P & 1<<y) {
                    for (y = x + 1; y < 8 && (O & 1<<y); ++y) {
                        O ^= 1<<y; P ^= 1<<y;
                    }
                }
            }
            stable = find_edge_stable(P, O, stable); // next move
            if (!stable) return stable;

            P = old_P;
            O = old_O | 1<<x; // opponent plays on it

            if (x > 1) {
                for (y = x - 1; y > 0 && (P & 1<<y); --y) ;
                if (O & 1<<y) {
                    for (y = x - 1; y > 0 && (P & 1<<y); --y) {
                        O ^= 1<<y; P ^= 1<<y;
                    }
                }
            }
            if (x < 6) {
                for (y = x + 1; y < 8 && (P & 1<<y); ++y) ;
                if (O & 1<<y) {
                    for (y = x + 1; y < 8 && (P & 1<<y); ++y) {
                        O ^= 1<<y; P ^= 1<<y;
                    }
                }
            }
            stable = find_edge_stable(P, O, stable); // next move
            if (!stable) return stable;
        }
    }

    return stable;
}

/**
 * @brief Initialize the edge stability tables.
 */
void RXBitBoard::edge_stability_init() {

    for (int P = 0; P < 256; ++P)
        for (int O = 0; O < 256; ++O) {
            if (P & O) { // illegal positions
                EDGE_STABILITY[P * 256 + O] = 0;
            } else {
                EDGE_STABILITY[P * 256 + O] = find_edge_stable(P, O, P);
            }
        }
}

void RXBitBoard::static_init() {
    edge_stability_init();
}


#if ARCH == ARCH_ARM_NEON


////not very efficient
//int RXBitBoard::count_potential_moves(const unsigned long long P, const unsigned long long O) {
//        
//    //use tab for speed
//    static const uint64x2_t mask[] = {  {0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL},
//                                        {0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL}};
//    
//    static const int64x2_t shift[] = {{ 1, 8}, {-1,-8}, { 7, 9}, {-7,-9}};
//    
//    const uint64x2_t oo = vdupq_n_u64(O);
//    
//    uint64x2_t hv = vandq_u64(oo, mask[0]);
//    hv = vorrq_u64(vshlq_u64(hv, shift[0]),vshlq_u64(hv, shift[1])) ;
//    
//    uint64x2_t dg = vandq_u64(oo,  mask[1]);
//    dg = vorrq_u64(vshlq_u64(dg, shift[2]),vshlq_u64(dg, shift[3])) ;
//    
//    hv = vorrq_u64(hv, dg);
//    
//    uint64x1_t lo = vget_low_u64(hv);
//    uint64x1_t hi = vget_high_u64(hv);
//    uint64x1_t combined = vorr_u64(lo, hi);
//    unsigned long long result = vget_lane_u64(combined, 0) & ~(P | O);
//        
//    return std::popcount(result);
//
//}

void RXBitBoard::dual_potential_mobility(const unsigned long long P, const unsigned long long O, int &p_pmob, int &o_pmob) {
    uint64x2_t opp_pair = {O, P};
    uint64x2_t occupied = vdupq_n_u64(P | O);
    
    // Masques
    uint64x2_t m_lr = vdupq_n_u64(0x7E7E7E7E7E7E7E7EULL);
    uint64x2_t m_tb = vdupq_n_u64(0x00FFFFFFFFFFFF00ULL);
    uint64x2_t m_dg = vdupq_n_u64(0x007E7E7E7E7E7E00ULL);

    // Calcul des zones intérieures
    uint64x2_t in_lr = vandq_u64(opp_pair, m_lr);
    uint64x2_t in_tb = vandq_u64(opp_pair, m_tb);
    uint64x2_t in_dg = vandq_u64(opp_pair, m_dg);

    // Shifts et Or (On accumule les potentiels)
    uint64x2_t pot = vorrq_u64(vshlq_n_u64(in_lr, 1), vshrq_n_u64(in_lr, 1));
    pot = vorrq_u64(pot, vorrq_u64(vshlq_n_u64(in_tb, 8), vshrq_n_u64(in_tb, 8)));
    pot = vorrq_u64(pot, vorrq_u64(vshlq_n_u64(in_dg, 7), vshrq_n_u64(in_dg, 7)));
    pot = vorrq_u64(pot, vorrq_u64(vshlq_n_u64(in_dg, 9), vshrq_n_u64(in_dg, 9)));

    // Utilisation de BIC (Bit Clear) pour remplacer AND NOT
    // vbicq_u64 n'existe pas non plus, on utilise vbicq_u8
    uint8x16_t final_pot_v = vbicq_u8(vreinterpretq_u8_u64(pot), vreinterpretq_u8_u64(occupied));
    uint64x2_t final_pot = vreinterpretq_u64_u8(final_pot_v);

    // Extraction et Popcount
    p_pmob = std::popcount(vgetq_lane_u64(final_pot, 0));
    o_pmob = std::popcount(vgetq_lane_u64(final_pot, 1));
}



#else



#endif



RXBitBoard::RXBitBoard(): player(BLACK), n_empty(60), n_nodes(0), parity(0xF){
    
    //start position
    discs[BLACK] = 0X000000810000000ULL;
    discs[WHITE] = 0X000001008000000ULL;
    
    /* create emptiesList */
    RXSquareList* iEmpties = empties_list;      //empties[0]
    iEmpties->position = NOMOVE;                //sentinel
    iEmpties->previous = nullptr;                  //nullptr
    iEmpties->next = iEmpties + 1;
    iEmpties = iEmpties->next;
    
    const unsigned long long occupied_squares = discs[BLACK] | discs[WHITE];
    
    for(int i = 0; i<60; i++) {
        if(((occupied_squares) & (0x1ULL<<PRESORTED_POSITION[i])) == 0) {
            iEmpties->position = PRESORTED_POSITION[i];
            iEmpties->previous = iEmpties - 1;
            iEmpties->next = iEmpties + 1 ;
            
            position_to_empties[PRESORTED_POSITION[i]] = iEmpties;
            iEmpties = iEmpties->next;
        }
    }
    iEmpties->position = NOMOVE;                //sentinel
    iEmpties->previous = iEmpties - 1;
    iEmpties->next = 0;                         //nullptr
    
        

}

void RXBitBoard::reset() {
    
    player = BLACK;
    n_empty = 60;
    n_nodes = 0;
    parity = 0xF;
    
    //start position
    discs[BLACK] = 0X000000810000000ULL;
    discs[WHITE] = 0X000001008000000ULL;
    
    /* create emptiesList */
    RXSquareList* iEmpties = empties_list;      //empties[0]
    iEmpties->position = NOMOVE;                //sentinel
    iEmpties->previous = nullptr;                  //nullptr
    iEmpties->next = iEmpties + 1;
    iEmpties = iEmpties->next;
    
    const unsigned long long occupied_squares = discs[BLACK] | discs[WHITE];
    
    for(int i = 0; i<60; i++) {
        if(((occupied_squares) & (0x1ULL<<PRESORTED_POSITION[i])) == 0) {
            iEmpties->position = PRESORTED_POSITION[i];
            iEmpties->previous = iEmpties - 1;
            iEmpties->next = iEmpties + 1 ;
            
            position_to_empties[PRESORTED_POSITION[i]] = iEmpties;
            iEmpties = iEmpties->next;
        }
    }
    iEmpties->position = NOMOVE;                //sentinel
    iEmpties->previous = iEmpties - 1;
    iEmpties->next = 0;                         //nullptr
    

}



//RXBitBoard::RXBitBoard(const RXBitBoard& src) {
//	
//	discs[BLACK] = src.discs[BLACK];
//	discs[WHITE] = src.discs[WHITE];
//	
//	player = src.player;
//	n_empty = src.n_empty;
//    parity = src.parity;
//	n_nodes = src.n_nodes;
//	
//	/* create emptiesList */
//	RXSquareList* iEmpties = empties_list;	//empties[0]
//	iEmpties->position = NOMOVE;			//sentinel
//	iEmpties->previous = nullptr;				//nullptr
//	iEmpties->next = iEmpties + 1;
//	iEmpties = iEmpties->next;
//    
//    const unsigned long long occupied_squares = discs[BLACK] | discs[WHITE];
//	
//	for(int i = 0; i<60; i++) {
//		if(((occupied_squares) & (0x1ULL<<PRESORTED_POSITION[i])) == 0) {
//			iEmpties->position = PRESORTED_POSITION[i];
//			iEmpties->previous = iEmpties - 1;
//			iEmpties->next = iEmpties + 1 ;
//			
//			position_to_empties[PRESORTED_POSITION[i]] = iEmpties;
//			iEmpties = iEmpties->next;
//		}
//	}
//	iEmpties->position = NOMOVE;			//sentinel
//	iEmpties->previous = iEmpties - 1; 
//	iEmpties->next = 0;						//nullptr
//	
//	/*BE CAREFULL*/
//	/*copy actual empties list */
//	RXSquareList* previous = empties_list;
//	for(RXSquareList* empties = src.empties_list->next; empties->position != NOMOVE; empties = empties->next) {
//		RXSquareList* empty = position_to_empties[empties->position];
//		empty->previous = previous;
//		previous->next = empty;
//		previous = previous->next;
//	}
//	empties_list[61].previous = previous;
//	previous->next = &empties_list[61];
//	
//}

RXBitBoard::RXBitBoard(const RXBitBoard& src) {
    // 1. Copie directe des variables simples et des Bitboards
    discs[BLACK] = src.discs[BLACK];
    discs[WHITE] = src.discs[WHITE];
    player = src.player;
    n_empty = src.n_empty;
    parity = src.parity;
    n_nodes = src.n_nodes;

    // 2. On copie TOUS les maillons de manière brute (sans recalculer les bitboards)
    for(int i = 0; i < 62; i++) {
        this->empties_list[i].position = src.empties_list[i].position;
        // On réinitialise temporairement les pointeurs pour des raisons de sécurité
        this->empties_list[i].next = nullptr;
        this->empties_list[i].previous = nullptr;
    }

    // 3. On reconstruit la table de correspondance position -> nouveau maillon
    for(int i = 0; i < 62; i++) {
        if (this->empties_list[i].position != NOMOVE) {
            this->position_to_empties[this->empties_list[i].position] = &this->empties_list[i];
        }
    }

    // 4. On re-tisse la liste active exactement comme vous le faisiez
    RXSquareList* previous = this->empties_list; // Tête de liste de la copie
    
    // On parcourt la liste active de la SOURCE
    for(const RXSquareList* empties = src.empties_list->next; empties->position != NOMOVE; empties = empties->next) {
        // On récupère le maillon équivalent dans la COPIE via notre table toute neuve
        RXSquareList* empty = this->position_to_empties[empties->position];
        
        empty->previous = previous;
        previous->next = empty;
        previous = previous->next;
    }
    
    // On ferme la liste avec la sentinelle de fin (index 61)
    this->empties_list[61].previous = previous;
    previous->next = &this->empties_list[61];
    this->empties_list[61].next = nullptr;
}



void RXBitBoard::build(const std::string& init) {
    
    discs[BLACK] = discs[WHITE] = 0ULL;

    n_empty = 64;
    parity = 0;
    
    player = UNDEF_COLOR;

    unsigned int id = 0;
    for (int i = A1; i >= H8; i--) {
        
        switch (std::tolower(init[id])) {
            case 'b':
            case 'x':
            case '*':
                discs[BLACK] |= 0x1ULL<<i;
                --n_empty;
                parity ^=QUADRANT_ID[i];
                break;
            case 'o':
            case 'w':
                discs[WHITE] |= 0x1ULL<<i;
                --n_empty;
                parity ^=QUADRANT_ID[i];
                break;
            case '-':
            case '.':
                break;
            case ' ':
            case '[':
                ++i;
                break;
            default:
                std::cerr << "RXBoard::build incorrect board" << std::endl;
                exit(EXIT_FAILURE);
            }
        ++id;
    }
    
    if(((discs[BLACK] | discs[WHITE]) & 0x0000001818000000ULL) != 0x0000001818000000ULL) {
        std::cerr << "RXBoard::build incorrect board" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(;id<init.length() && player == UNDEF_COLOR; id++)
        switch (std::tolower(init[id])) {
            case 'b':
            case 'x':
            case '*':
                player = BLACK;
                break;
            case 'o':
            case 'w':
                player = WHITE;
                break;
            default:
                break;
        }
            
    if(player == UNDEF_COLOR) {
        std::cerr<< "RXBoard::build incorrect player value" << std::endl;
        exit(EXIT_FAILURE);
    }

    RXSquareList* previous = empties_list;
    for(id = 0; id<60; id++) {
        if(((discs[BLACK] | discs[WHITE]) & (1ULL<<PRESORTED_POSITION[id])) == 0) {
            RXSquareList* empty = position_to_empties[PRESORTED_POSITION[id]];
            empty->previous = previous;
            previous->next = empty;
            previous = previous->next;
        }
    }
    empties_list[61].previous = previous;
    previous->next = &empties_list[61];
            
    
}

std::ostream& operator<<(std::ostream& os, RXBitBoard& board) {

	os << "\n  A B C D E F G H " << std::endl;
	for(int iLine = 1; iLine<=8; iLine++) {
		os << iLine << " ";
		for(int iPosition = (9-iLine)*8 - 1; iPosition>(8-iLine)*8-1; iPosition--) {
			unsigned long long _mask = 0x1ULL<<iPosition;
			if((board.discs[BLACK] & _mask) != 0) {
					os << "# ";
			} else if((board.discs[WHITE] & _mask) !=0) {
					os << "O ";
			} else {
					os << ". ";
			}
			
		}
		os << iLine;
		if(iLine == 4)
			os << "\tNoirs: " << std::popcount(board.discs[BLACK]) << "\tBlancs: " << std::popcount(board.discs[WHITE]);
		if(iLine == 5)
			os << "\t" << (board.player == WHITE ? "BLANCS" : "NOIRS") << " au trait";
//        if(iLine == 6)
//            os << "\tHashcode " << std::hex << board.hashcode() << std::dec;


		os << std::endl;
	}
	os << "  A B C D E F G H \n" << std::endl;
	
	return os;

}



bool RXBitBoard::squareIsEmpty(const int position) const {
	if((discs[BLACK] | discs[WHITE]) & 0x1ULL<<position)
		return false;
	return true;
}



bool RXBitBoard::isPassed() {	
	if(get_mobility(discs[player], discs[player^1]) != 0)
		return false;
			
	return true;
}

bool RXBitBoard::isEndGame() {
	if(isPassed()) {
		player ^= 1;
		if(isPassed()) {
			player ^= 1;
			return true;
		}
		player ^= 1;
	}
	return false;
}



int RXBitBoard::final_score() const {
        
	int score = std::popcount(discs[player]) -std::popcount(discs[player^1]);
	if(score<0)
		score -= n_empty;
	else if(score>0)
		score += n_empty;
		
	return score;
}


unsigned int RXBitBoard::n_moves() const {
    
    const unsigned long long legal_movesBB = get_legal_moves(discs[player], discs[player^1]);
    return std::popcount(legal_movesBB);
    
}


std::string RXBitBoard::cassio_script() const {
	
	std::ostringstream buffer;
	
	for (int square=A1; square>=H8; square--) {
		unsigned long long mask =  0x1ULL<<square;
		if (discs[BLACK] & mask)
			buffer << 'X';
		else if (discs[WHITE] & mask)
			buffer << 'O';
		else 
			buffer << '-';
	}
	
	if (player == BLACK)
		buffer << " X";
	else
		buffer << " O";
	
	return buffer.str();
}



/* DEBUG */
void RXBitBoard::print_empties_list() const {
	std::cout << "emptiesList";
	for(RXSquareList *empties = empties_list->next; empties->position != NOMOVE; empties = empties->next)
		std::cout << " : " << RXMove::index_to_coord(empties->position);
	std::cout << std::endl;
}

template<int Shift, unsigned long long Mask>
inline bool dir_valid(unsigned long long square, unsigned long long P, unsigned long long O) {
    unsigned long long x;
    
    if constexpr (Shift > 0) {
        // Shift left (vers le sud/est)
        x = (square << Shift) & Mask & O;
        x |= (x << Shift) & Mask & O;
        x |= (x << (2 * Shift)) & Mask & O;
        x |= (x << (4 * Shift)) & Mask & O;
        return (x << Shift) & Mask & P;
    } else {
        // Shift right (vers le nord/ouest)
        constexpr int S = -Shift;
        x = (square >> S) & Mask & O;
        x |= (x >> S) & Mask & O;
        x |= (x >> (2 * S)) & Mask & O;
        x |= (x >> (4 * S)) & Mask & O;
        return (x >> S) & Mask & P;
    }
}

bool RXBitBoard::isValid_square(const unsigned int pos) const {
    
    static constexpr unsigned long long MASK_LEFT  = 0xfefefefefefefefeULL;
    static constexpr unsigned long long MASK_RIGHT = 0x7f7f7f7f7f7f7f7fULL;
    static constexpr unsigned long long MASK_ALL   = 0xffffffffffffffffULL;

    
    if(pos == PASS)
        return (get_legal_moves() == 0);
        
    if(pos < PASS) { //pass == 64
        
        const unsigned long long square = 0x1ULL<<pos;
        
        const unsigned long long P = discs[player];
        const unsigned long long O = discs[player^1];
        
        
        if ((P | O) & square) return false; // case occupée
        
        
        return
            dir_valid< 1, MASK_LEFT >(square, P, O) ||  // Est
            dir_valid<-1, MASK_RIGHT>(square, P, O) ||  // Ouest
            dir_valid< 8, MASK_ALL  >(square, P, O) ||  // Sud
            dir_valid<-8, MASK_ALL  >(square, P, O) ||  // Nord
            dir_valid< 9, MASK_LEFT >(square, P, O) ||  // Sud-Est
            dir_valid<-9, MASK_RIGHT>(square, P, O) ||  // Nord-Ouest
            dir_valid< 7, MASK_RIGHT>(square, P, O) ||  // Sud-Ouest
            dir_valid<-7, MASK_LEFT >(square, P, O);    // Nord-Est
    }
    
    return false;
}

/*
void RXBitBoard::check_empties_list() const {
    RXSquareList *empties = empties_list->next;
    for(int i = 0 ; i < n_empty; ++i) {
        if(empties->position < 0 || 63 < empties->position) {
            std::cout << "error empties list" << std::endl;
            print_Board(discs[player], discs[player^1]);
            print_empties_list();
            std::cout << std::endl;
        }
        empties = empties->next;
    }
}
 */


void RXBitBoard::print_moves_list(RXMove* Moves) const {
    std::cout << "MovesList";
    for(; Moves->position != static_cast<char>(NOMOVE); Moves = Moves->next)
        std::cout << " : " << RXMove::index_to_coord(Moves->position);
    std::cout << std::endl;
}

void RXBitBoard::print_64bits(unsigned long long n) {

	std::cout << "\n  A B C D E F G H " << std::endl;
	for(int iLine = 1; iLine<=8; iLine++) {
		std::cout  << iLine << " ";
		for(int iPosition = (9-iLine)*8 - 1; iPosition>(8-iLine)*8-1; iPosition--) {
			unsigned long long _mask = 0x1ULL<<iPosition;
			if((n & _mask) != 0) {
					std::cout  << "@ ";
			} else {
					std::cout  << ". ";
			}
			
		}
		std::cout  << iLine << std::endl;
	}
	std::cout << "  A B C D E F G H \n" << std::endl;
		
}

void RXBitBoard::print_Board() {

    std::cout << "\n  A B C D E F G H " << std::endl;
    for(int iLine = 1; iLine<=8; iLine++) {
        std::cout << iLine << " ";
        for(int iPosition = (9-iLine)*8 - 1; iPosition>(8-iLine)*8-1; iPosition--) {
            unsigned long long _mask = 0x1ULL<<iPosition;
            if((discs[BLACK] & _mask) != 0) {
                std::cout << "# ";
            } else if((discs[WHITE] & _mask) !=0) {
                std::cout << "O ";
            } else {
                std::cout << ". ";
            }
            
        }
        std::cout << iLine;
        if(iLine == 4)
            std::cout << "\tNoirs: " << std::popcount(discs[BLACK]) << "\tBlancs: " << std::popcount(discs[WHITE]);
        if(iLine == 5)
            std::cout << "\t" << (player == WHITE ? "BLANCS" : "NOIRS") << " au trait";
        
        std::cout << std::endl;
    }
    std::cout << "  A B C D E F G H \n" << std::endl;
    
}

std::string RXBitBoard::string_rawdata() {
    
    std::string line = "";

    for(int iPosition = A1; iPosition>=H8; --iPosition) {
        unsigned long long _mask = 0x1ULL<<iPosition;
        if((discs[player] & _mask) != 0) {
            line += player == BLACK ?"X": "O";
        } else if((discs[player^1] & _mask) !=0) {
            line += player != BLACK ?"X": "O";
        } else {
            line += "-";
        }
        
    }
    
    if(player == BLACK) {
        line += " X";
    } else {
        line += " O";
    }

    return line;
}


void RXBitBoard::print_Board(unsigned long long P, unsigned long long O) {

    std::cout << "\n  A B C D E F G H " << std::endl;
    for(int iLine = 1; iLine<=8; iLine++) {
        std::cout << iLine << " ";
        for(int iPosition = (9-iLine)*8 - 1; iPosition>(8-iLine)*8-1; iPosition--) {
            unsigned long long _mask = 0x1ULL<<iPosition;
            if((P & _mask) != 0) {
                std::cout << "P ";
            } else if((O & _mask) !=0) {
                std::cout << "O ";
            } else {
                std::cout << ". ";
            }
            
        }
        std::cout << iLine;

        std::cout << std::endl;
    }
    std::cout << "  A B C D E F G H \n" << std::endl;
    
    

}

