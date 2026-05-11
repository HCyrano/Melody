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

#include "RXBBPatterns.hpp"
#include <string>

#include "RXEvaluation.hpp"


std::ostream& operator<<(std::ostream& os, RXBBPatterns& sBoard) {
    
    os << sBoard.board << "Evaluation : " << std::setprecision(6) << static_cast<float>(sBoard.get_score()) << std::endl;
    
    return os;
    
}


RXBBPatterns::RXBBPatterns(): pattern(new RXPattern()) {
    
    
    pattern->set_WHITE_D4();
    pattern->set_BLACK_E4();
    pattern->set_BLACK_D5();
    pattern->set_WHITE_E5();
    
}

RXBBPatterns::RXBBPatterns(const RXBBPatterns& src) : board(src.board), pattern(new RXPattern()) {
    
    
    *pattern = *(src.pattern); //copy
    
}

RXBBPatterns::~RXBBPatterns() {
    delete pattern;
}



void RXBBPatterns::reset() {
    
    board.reset();
    
    pattern->clear();
    
    pattern->set_WHITE_D4();
    pattern->set_BLACK_E4();
    pattern->set_BLACK_D5();
    pattern->set_WHITE_E5();
    
}

void RXBBPatterns::build(const std::string& init) {
    
    board.build(init);
    
    pattern->clear();
    
    for(int square = H8; square <= A1; square++) {
        
        unsigned long long pos = 0x1ULL<<square;
        
        if(board.discs[BLACK] & pos)
            set_BLACK(square);
        else if(board.discs[WHITE] & pos)
            set_WHITE(square);
    }
}


void RXBBPatterns::set_BLACK(const int pos) {
    
    RXPattern& p = *pattern;
    
    switch(pos) {
            
        case A1: p.set_BLACK_A1(); break;
        case B1: p.set_BLACK_B1(); break;
        case C1: p.set_BLACK_C1(); break;
        case D1: p.set_BLACK_D1(); break;
        case E1: p.set_BLACK_E1(); break;
        case F1: p.set_BLACK_F1(); break;
        case G1: p.set_BLACK_G1(); break;
        case H1: p.set_BLACK_H1(); break;
        case A2: p.set_BLACK_A2(); break;
        case B2: p.set_BLACK_B2(); break;
        case C2: p.set_BLACK_C2(); break;
        case D2: p.set_BLACK_D2(); break;
        case E2: p.set_BLACK_E2(); break;
        case F2: p.set_BLACK_F2(); break;
        case G2: p.set_BLACK_G2(); break;
        case H2: p.set_BLACK_H2(); break;
        case A3: p.set_BLACK_A3(); break;
        case B3: p.set_BLACK_B3(); break;
        case C3: p.set_BLACK_C3(); break;
        case D3: p.set_BLACK_D3(); break;
        case E3: p.set_BLACK_E3(); break;
        case F3: p.set_BLACK_F3(); break;
        case G3: p.set_BLACK_G3(); break;
        case H3: p.set_BLACK_H3(); break;
        case A4: p.set_BLACK_A4(); break;
        case B4: p.set_BLACK_B4(); break;
        case C4: p.set_BLACK_C4(); break;
        case D4: p.set_BLACK_D4(); break;
        case E4: p.set_BLACK_E4(); break;
        case F4: p.set_BLACK_F4(); break;
        case G4: p.set_BLACK_G4(); break;
        case H4: p.set_BLACK_H4(); break;
        case A5: p.set_BLACK_A5(); break;
        case B5: p.set_BLACK_B5(); break;
        case C5: p.set_BLACK_C5(); break;
        case D5: p.set_BLACK_D5(); break;
        case E5: p.set_BLACK_E5(); break;
        case F5: p.set_BLACK_F5(); break;
        case G5: p.set_BLACK_G5(); break;
        case H5: p.set_BLACK_H5(); break;
        case A6: p.set_BLACK_A6(); break;
        case B6: p.set_BLACK_B6(); break;
        case C6: p.set_BLACK_C6(); break;
        case D6: p.set_BLACK_D6(); break;
        case E6: p.set_BLACK_E6(); break;
        case F6: p.set_BLACK_F6(); break;
        case G6: p.set_BLACK_G6(); break;
        case H6: p.set_BLACK_H6(); break;
        case A7: p.set_BLACK_A7(); break;
        case B7: p.set_BLACK_B7(); break;
        case C7: p.set_BLACK_C7(); break;
        case D7: p.set_BLACK_D7(); break;
        case E7: p.set_BLACK_E7(); break;
        case F7: p.set_BLACK_F7(); break;
        case G7: p.set_BLACK_G7(); break;
        case H7: p.set_BLACK_H7(); break;
        case A8: p.set_BLACK_A8(); break;
        case B8: p.set_BLACK_B8(); break;
        case C8: p.set_BLACK_C8(); break;
        case D8: p.set_BLACK_D8(); break;
        case E8: p.set_BLACK_E8(); break;
        case F8: p.set_BLACK_F8(); break;
        case G8: p.set_BLACK_G8(); break;
        case H8: p.set_BLACK_H8(); break;
            
    }
}

void RXBBPatterns::set_WHITE(const int pos) {
    
    RXPattern& p = *pattern;
    
    switch(pos) {
            
        case A1: p.set_WHITE_A1(); break;
        case B1: p.set_WHITE_B1(); break;
        case C1: p.set_WHITE_C1(); break;
        case D1: p.set_WHITE_D1(); break;
        case E1: p.set_WHITE_E1(); break;
        case F1: p.set_WHITE_F1(); break;
        case G1: p.set_WHITE_G1(); break;
        case H1: p.set_WHITE_H1(); break;
        case A2: p.set_WHITE_A2(); break;
        case B2: p.set_WHITE_B2(); break;
        case C2: p.set_WHITE_C2(); break;
        case D2: p.set_WHITE_D2(); break;
        case E2: p.set_WHITE_E2(); break;
        case F2: p.set_WHITE_F2(); break;
        case G2: p.set_WHITE_G2(); break;
        case H2: p.set_WHITE_H2(); break;
        case A3: p.set_WHITE_A3(); break;
        case B3: p.set_WHITE_B3(); break;
        case C3: p.set_WHITE_C3(); break;
        case D3: p.set_WHITE_D3(); break;
        case E3: p.set_WHITE_E3(); break;
        case F3: p.set_WHITE_F3(); break;
        case G3: p.set_WHITE_G3(); break;
        case H3: p.set_WHITE_H3(); break;
        case A4: p.set_WHITE_A4(); break;
        case B4: p.set_WHITE_B4(); break;
        case C4: p.set_WHITE_C4(); break;
        case D4: p.set_WHITE_D4(); break;
        case E4: p.set_WHITE_E4(); break;
        case F4: p.set_WHITE_F4(); break;
        case G4: p.set_WHITE_G4(); break;
        case H4: p.set_WHITE_H4(); break;
        case A5: p.set_WHITE_A5(); break;
        case B5: p.set_WHITE_B5(); break;
        case C5: p.set_WHITE_C5(); break;
        case D5: p.set_WHITE_D5(); break;
        case E5: p.set_WHITE_E5(); break;
        case F5: p.set_WHITE_F5(); break;
        case G5: p.set_WHITE_G5(); break;
        case H5: p.set_WHITE_H5(); break;
        case A6: p.set_WHITE_A6(); break;
        case B6: p.set_WHITE_B6(); break;
        case C6: p.set_WHITE_C6(); break;
        case D6: p.set_WHITE_D6(); break;
        case E6: p.set_WHITE_E6(); break;
        case F6: p.set_WHITE_F6(); break;
        case G6: p.set_WHITE_G6(); break;
        case H6: p.set_WHITE_H6(); break;
        case A7: p.set_WHITE_A7(); break;
        case B7: p.set_WHITE_B7(); break;
        case C7: p.set_WHITE_C7(); break;
        case D7: p.set_WHITE_D7(); break;
        case E7: p.set_WHITE_E7(); break;
        case F7: p.set_WHITE_F7(); break;
        case G7: p.set_WHITE_G7(); break;
        case H7: p.set_WHITE_H7(); break;
        case A8: p.set_WHITE_A8(); break;
        case B8: p.set_WHITE_B8(); break;
        case C8: p.set_WHITE_C8(); break;
        case D8: p.set_WHITE_D8(); break;
        case E8: p.set_WHITE_E8(); break;
        case F8: p.set_WHITE_F8(); break;
        case G8: p.set_WHITE_G8(); break;
        case H8: p.set_WHITE_H8(); break;
            
    }
}




void RXBBPatterns::update_patterns_BLACK_A1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A1();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040200ULL);
        switch(n) {
            case 6: p.flip_BLACK_G7(); [[fallthrough]];
            case 5: p.flip_BLACK_F6(); [[fallthrough]];
            case 4: p.flip_BLACK_E5(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_B2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080808000ULL);
        switch(n) {
            case 6: p.flip_BLACK_A7(); [[fallthrough]];
            case 5: p.flip_BLACK_A6(); [[fallthrough]];
            case 4: p.flip_BLACK_A5(); [[fallthrough]];
            case 3: p.flip_BLACK_A4(); [[fallthrough]];
            case 2: p.flip_BLACK_A3(); [[fallthrough]];
            case 1: p.flip_BLACK_A2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X7E00000000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G1(); [[fallthrough]];
            case 5: p.flip_BLACK_F1(); [[fallthrough]];
            case 4: p.flip_BLACK_E1(); [[fallthrough]];
            case 3: p.flip_BLACK_D1(); [[fallthrough]];
            case 2: p.flip_BLACK_C1(); [[fallthrough]];
            case 1: p.flip_BLACK_B1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B1();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804020000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G6(); [[fallthrough]];
            case 4: p.flip_BLACK_F5(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_C2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040404000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B7(); [[fallthrough]];
            case 5: p.flip_BLACK_B6(); [[fallthrough]];
            case 4: p.flip_BLACK_B5(); [[fallthrough]];
            case 3: p.flip_BLACK_B4(); [[fallthrough]];
            case 2: p.flip_BLACK_B3(); [[fallthrough]];
            case 1: p.flip_BLACK_B2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X3E00000000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G1(); [[fallthrough]];
            case 4: p.flip_BLACK_F1(); [[fallthrough]];
            case 3: p.flip_BLACK_E1(); [[fallthrough]];
            case 2: p.flip_BLACK_D1(); [[fallthrough]];
            case 1: p.flip_BLACK_C1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020202000ULL);
        switch(n) {
            case 6: p.flip_BLACK_C7(); [[fallthrough]];
            case 5: p.flip_BLACK_C6(); [[fallthrough]];
            case 4: p.flip_BLACK_C5(); [[fallthrough]];
            case 3: p.flip_BLACK_C4(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_C2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080402000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G5(); [[fallthrough]];
            case 3: p.flip_BLACK_F4(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_D2();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0040000000000000ULL)
        p.flip_BLACK_B2();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X1E00000000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G1(); [[fallthrough]];
            case 3: p.flip_BLACK_F1(); [[fallthrough]];
            case 2: p.flip_BLACK_E1(); [[fallthrough]];
            case 1: p.flip_BLACK_D1();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X4000000000000000ULL)
        p.flip_BLACK_B1();
    
}

void RXBBPatterns::update_patterns_BLACK_D1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010101000ULL);
        switch(n) {
            case 6: p.flip_BLACK_D7(); [[fallthrough]];
            case 5: p.flip_BLACK_D6(); [[fallthrough]];
            case 4: p.flip_BLACK_D5(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_D2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040200000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G4(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_E2();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020400000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B3(); [[fallthrough]];
            case 1: p.flip_BLACK_C2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0E00000000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G1(); [[fallthrough]];
            case 2: p.flip_BLACK_F1(); [[fallthrough]];
            case 1: p.flip_BLACK_E1();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X6000000000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B1(); [[fallthrough]];
            case 1: p.flip_BLACK_C1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080800ULL);
        switch(n) {
            case 6: p.flip_BLACK_E7(); [[fallthrough]];
            case 5: p.flip_BLACK_E6(); [[fallthrough]];
            case 4: p.flip_BLACK_E5(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_E2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004020000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G3(); [[fallthrough]];
            case 1: p.flip_BLACK_F2();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010204000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B4(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_D2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0600000000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G1(); [[fallthrough]];
            case 1: p.flip_BLACK_F1();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7000000000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B1(); [[fallthrough]];
            case 2: p.flip_BLACK_C1(); [[fallthrough]];
            case 1: p.flip_BLACK_D1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040400ULL);
        switch(n) {
            case 6: p.flip_BLACK_F7(); [[fallthrough]];
            case 5: p.flip_BLACK_F6(); [[fallthrough]];
            case 4: p.flip_BLACK_F5(); [[fallthrough]];
            case 3: p.flip_BLACK_F4(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_F2();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0002000000000000ULL)
        p.flip_BLACK_G2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102040000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B5(); [[fallthrough]];
            case 3: p.flip_BLACK_C4(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_E2();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0200000000000000ULL)
        p.flip_BLACK_G1();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7800000000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B1(); [[fallthrough]];
            case 3: p.flip_BLACK_C1(); [[fallthrough]];
            case 2: p.flip_BLACK_D1(); [[fallthrough]];
            case 1: p.flip_BLACK_E1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G1();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020400000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B6(); [[fallthrough]];
            case 4: p.flip_BLACK_C5(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_F2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7C00000000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B1(); [[fallthrough]];
            case 4: p.flip_BLACK_C1(); [[fallthrough]];
            case 3: p.flip_BLACK_D1(); [[fallthrough]];
            case 2: p.flip_BLACK_E1(); [[fallthrough]];
            case 1: p.flip_BLACK_F1();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020200ULL);
        switch(n) {
            case 6: p.flip_BLACK_G7(); [[fallthrough]];
            case 5: p.flip_BLACK_G6(); [[fallthrough]];
            case 4: p.flip_BLACK_G5(); [[fallthrough]];
            case 3: p.flip_BLACK_G4(); [[fallthrough]];
            case 2: p.flip_BLACK_G3(); [[fallthrough]];
            case 1: p.flip_BLACK_G2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H1();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810204000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B7(); [[fallthrough]];
            case 5: p.flip_BLACK_C6(); [[fallthrough]];
            case 4: p.flip_BLACK_D5(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_G2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7E00000000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B1(); [[fallthrough]];
            case 5: p.flip_BLACK_C1(); [[fallthrough]];
            case 4: p.flip_BLACK_D1(); [[fallthrough]];
            case 3: p.flip_BLACK_E1(); [[fallthrough]];
            case 2: p.flip_BLACK_F1(); [[fallthrough]];
            case 1: p.flip_BLACK_G1();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010100ULL);
        switch(n) {
            case 6: p.flip_BLACK_H7(); [[fallthrough]];
            case 5: p.flip_BLACK_H6(); [[fallthrough]];
            case 4: p.flip_BLACK_H5(); [[fallthrough]];
            case 3: p.flip_BLACK_H4(); [[fallthrough]];
            case 2: p.flip_BLACK_H3(); [[fallthrough]];
            case 1: p.flip_BLACK_H2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080400ULL);
        switch(n) {
            case 5: p.flip_BLACK_F7(); [[fallthrough]];
            case 4: p.flip_BLACK_E6(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_B3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000808080808000ULL);
        switch(n) {
            case 5: p.flip_BLACK_A7(); [[fallthrough]];
            case 4: p.flip_BLACK_A6(); [[fallthrough]];
            case 3: p.flip_BLACK_A5(); [[fallthrough]];
            case 2: p.flip_BLACK_A4(); [[fallthrough]];
            case 1: p.flip_BLACK_A3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X007E000000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G2(); [[fallthrough]];
            case 5: p.flip_BLACK_F2(); [[fallthrough]];
            case 4: p.flip_BLACK_E2(); [[fallthrough]];
            case 3: p.flip_BLACK_D2(); [[fallthrough]];
            case 2: p.flip_BLACK_C2(); [[fallthrough]];
            case 1: p.flip_BLACK_B2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000201008040200ULL);
        switch(n) {
            case 5: p.flip_BLACK_G7(); [[fallthrough]];
            case 4: p.flip_BLACK_F6(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000404040404000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B7(); [[fallthrough]];
            case 4: p.flip_BLACK_B6(); [[fallthrough]];
            case 3: p.flip_BLACK_B5(); [[fallthrough]];
            case 2: p.flip_BLACK_B4(); [[fallthrough]];
            case 1: p.flip_BLACK_B3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X003E000000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G2(); [[fallthrough]];
            case 4: p.flip_BLACK_F2(); [[fallthrough]];
            case 3: p.flip_BLACK_E2(); [[fallthrough]];
            case 2: p.flip_BLACK_D2(); [[fallthrough]];
            case 1: p.flip_BLACK_C2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000202020202000ULL);
        switch(n) {
            case 5: p.flip_BLACK_C7(); [[fallthrough]];
            case 4: p.flip_BLACK_C6(); [[fallthrough]];
            case 3: p.flip_BLACK_C5(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000100804020000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G6(); [[fallthrough]];
            case 3: p.flip_BLACK_F5(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000400000000000ULL)
        p.flip_BLACK_B3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X001E000000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G2(); [[fallthrough]];
            case 3: p.flip_BLACK_F2(); [[fallthrough]];
            case 2: p.flip_BLACK_E2(); [[fallthrough]];
            case 1: p.flip_BLACK_D2();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0040000000000000ULL)
        p.flip_BLACK_B2();
    
}

void RXBBPatterns::update_patterns_BLACK_D2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000101010101000ULL);
        switch(n) {
            case 5: p.flip_BLACK_D7(); [[fallthrough]];
            case 4: p.flip_BLACK_D6(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000080402000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G5(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000204000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B4(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000E000000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G2(); [[fallthrough]];
            case 2: p.flip_BLACK_F2(); [[fallthrough]];
            case 1: p.flip_BLACK_E2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0060000000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B2(); [[fallthrough]];
            case 1: p.flip_BLACK_C2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000080808080800ULL);
        switch(n) {
            case 5: p.flip_BLACK_E7(); [[fallthrough]];
            case 4: p.flip_BLACK_E6(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040200000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G4(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000102040000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B5(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0006000000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G2(); [[fallthrough]];
            case 1: p.flip_BLACK_F2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0070000000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B2(); [[fallthrough]];
            case 2: p.flip_BLACK_C2(); [[fallthrough]];
            case 1: p.flip_BLACK_D2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040404040400ULL);
        switch(n) {
            case 5: p.flip_BLACK_F7(); [[fallthrough]];
            case 4: p.flip_BLACK_F6(); [[fallthrough]];
            case 3: p.flip_BLACK_F5(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000020000000000ULL)
        p.flip_BLACK_G3();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000081020400000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B6(); [[fallthrough]];
            case 3: p.flip_BLACK_C5(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0002000000000000ULL)
        p.flip_BLACK_G2();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0078000000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B2(); [[fallthrough]];
            case 3: p.flip_BLACK_C2(); [[fallthrough]];
            case 2: p.flip_BLACK_D2(); [[fallthrough]];
            case 1: p.flip_BLACK_E2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040810204000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B7(); [[fallthrough]];
            case 4: p.flip_BLACK_C6(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X007C000000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B2(); [[fallthrough]];
            case 4: p.flip_BLACK_C2(); [[fallthrough]];
            case 3: p.flip_BLACK_D2(); [[fallthrough]];
            case 2: p.flip_BLACK_E2(); [[fallthrough]];
            case 1: p.flip_BLACK_F2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020202020200ULL);
        switch(n) {
            case 5: p.flip_BLACK_G7(); [[fallthrough]];
            case 4: p.flip_BLACK_G6(); [[fallthrough]];
            case 3: p.flip_BLACK_G5(); [[fallthrough]];
            case 2: p.flip_BLACK_G4(); [[fallthrough]];
            case 1: p.flip_BLACK_G3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408102000ULL);
        switch(n) {
            case 5: p.flip_BLACK_C7(); [[fallthrough]];
            case 4: p.flip_BLACK_D6(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_G3();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X007E000000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B2(); [[fallthrough]];
            case 5: p.flip_BLACK_C2(); [[fallthrough]];
            case 4: p.flip_BLACK_D2(); [[fallthrough]];
            case 3: p.flip_BLACK_E2(); [[fallthrough]];
            case 2: p.flip_BLACK_F2(); [[fallthrough]];
            case 1: p.flip_BLACK_G2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000010101010100ULL);
        switch(n) {
            case 5: p.flip_BLACK_H7(); [[fallthrough]];
            case 4: p.flip_BLACK_H6(); [[fallthrough]];
            case 3: p.flip_BLACK_H5(); [[fallthrough]];
            case 2: p.flip_BLACK_H4(); [[fallthrough]];
            case 1: p.flip_BLACK_H3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00007E0000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G3(); [[fallthrough]];
            case 5: p.flip_BLACK_F3(); [[fallthrough]];
            case 4: p.flip_BLACK_E3(); [[fallthrough]];
            case 3: p.flip_BLACK_D3(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_B3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0040000000000000ULL)
        p.flip_BLACK_B2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100800ULL);
        switch(n) {
            case 4: p.flip_BLACK_E7(); [[fallthrough]];
            case 3: p.flip_BLACK_D6(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_B4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0080000000000000ULL)
        p.flip_BLACK_A2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000008080808000ULL);
        switch(n) {
            case 4: p.flip_BLACK_A7(); [[fallthrough]];
            case 3: p.flip_BLACK_A6(); [[fallthrough]];
            case 2: p.flip_BLACK_A5(); [[fallthrough]];
            case 1: p.flip_BLACK_A4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00003E0000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G3(); [[fallthrough]];
            case 4: p.flip_BLACK_F3(); [[fallthrough]];
            case 3: p.flip_BLACK_E3(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0020000000000000ULL)
        p.flip_BLACK_C2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002010080400ULL);
        switch(n) {
            case 4: p.flip_BLACK_F7(); [[fallthrough]];
            case 3: p.flip_BLACK_E6(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0040000000000000ULL)
        p.flip_BLACK_B2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004040404000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B7(); [[fallthrough]];
            case 3: p.flip_BLACK_B6(); [[fallthrough]];
            case 2: p.flip_BLACK_B5(); [[fallthrough]];
            case 1: p.flip_BLACK_B4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001008040200ULL);
        switch(n) {
            case 4: p.flip_BLACK_G7(); [[fallthrough]];
            case 3: p.flip_BLACK_F6(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_D4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002020202000ULL);
        switch(n) {
            case 4: p.flip_BLACK_C7(); [[fallthrough]];
            case 3: p.flip_BLACK_C6(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00001E0000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G3(); [[fallthrough]];
            case 3: p.flip_BLACK_F3(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0010000000000000ULL)
        p.flip_BLACK_D2();
    
    /* direction _N */
    if(flipped & 0X0020000000000000ULL)
        p.flip_BLACK_C2();
    
    /* direction NW */
    if(flipped & 0X0040000000000000ULL)
        p.flip_BLACK_B2();
    
    /* direction _SW */
    if(flipped & 0X0000004000000000ULL)
        p.flip_BLACK_B4();
    
    /* direction _W */
    if(flipped & 0X0000400000000000ULL)
        p.flip_BLACK_B3();
    
}

void RXBBPatterns::update_patterns_BLACK_D3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000804020000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G6(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_E4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001010101000ULL);
        switch(n) {
            case 4: p.flip_BLACK_D7(); [[fallthrough]];
            case 3: p.flip_BLACK_D6(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_D4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000E0000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G3(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0008000000000000ULL)
        p.flip_BLACK_E2();
    
    /* direction _N */
    if(flipped & 0X0010000000000000ULL)
        p.flip_BLACK_D2();
    
    /* direction NW */
    if(flipped & 0X0020000000000000ULL)
        p.flip_BLACK_C2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002040000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B5(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000600000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B3(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000402000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G5(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000808080800ULL);
        switch(n) {
            case 4: p.flip_BLACK_E7(); [[fallthrough]];
            case 3: p.flip_BLACK_E6(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_E4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000060000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G3(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0004000000000000ULL)
        p.flip_BLACK_F2();
    
    /* direction _N */
    if(flipped & 0X0008000000000000ULL)
        p.flip_BLACK_E2();
    
    /* direction NW */
    if(flipped & 0X0010000000000000ULL)
        p.flip_BLACK_D2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001020400000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B6(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_D4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000700000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B3(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F3();
    
    /* direction _SE */
    if(flipped & 0X0000000200000000ULL)
        p.flip_BLACK_G4();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000404040400ULL);
        switch(n) {
            case 4: p.flip_BLACK_F7(); [[fallthrough]];
            case 3: p.flip_BLACK_F6(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000020000000000ULL)
        p.flip_BLACK_G3();
    
    /* direction NE */
    if(flipped & 0X0002000000000000ULL)
        p.flip_BLACK_G2();
    
    /* direction _N */
    if(flipped & 0X0004000000000000ULL)
        p.flip_BLACK_F2();
    
    /* direction NW */
    if(flipped & 0X0008000000000000ULL)
        p.flip_BLACK_E2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000810204000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B7(); [[fallthrough]];
            case 3: p.flip_BLACK_C6(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_E4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000780000000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B3(); [[fallthrough]];
            case 3: p.flip_BLACK_C3(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G3();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00007C0000000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B3(); [[fallthrough]];
            case 4: p.flip_BLACK_C3(); [[fallthrough]];
            case 3: p.flip_BLACK_D3(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000408102000ULL);
        switch(n) {
            case 4: p.flip_BLACK_C7(); [[fallthrough]];
            case 3: p.flip_BLACK_D6(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0004000000000000ULL)
        p.flip_BLACK_F2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000202020200ULL);
        switch(n) {
            case 4: p.flip_BLACK_G7(); [[fallthrough]];
            case 3: p.flip_BLACK_G6(); [[fallthrough]];
            case 2: p.flip_BLACK_G5(); [[fallthrough]];
            case 1: p.flip_BLACK_G4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0002000000000000ULL)
        p.flip_BLACK_G2();
    
}

void RXBBPatterns::update_patterns_BLACK_H3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H3();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00007E0000000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B3(); [[fallthrough]];
            case 5: p.flip_BLACK_C3(); [[fallthrough]];
            case 4: p.flip_BLACK_D3(); [[fallthrough]];
            case 3: p.flip_BLACK_E3(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_G3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204081000ULL);
        switch(n) {
            case 4: p.flip_BLACK_D7(); [[fallthrough]];
            case 3: p.flip_BLACK_E6(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_G4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0002000000000000ULL)
        p.flip_BLACK_G2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000101010100ULL);
        switch(n) {
            case 4: p.flip_BLACK_H7(); [[fallthrough]];
            case 3: p.flip_BLACK_H6(); [[fallthrough]];
            case 2: p.flip_BLACK_H5(); [[fallthrough]];
            case 1: p.flip_BLACK_H4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0001000000000000ULL)
        p.flip_BLACK_H2();
    
}

void RXBBPatterns::update_patterns_BLACK_A4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A4();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007E00000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G4(); [[fallthrough]];
            case 5: p.flip_BLACK_F4(); [[fallthrough]];
            case 4: p.flip_BLACK_E4(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_B4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0020400000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_C2(); [[fallthrough]];
            case 1: p.flip_BLACK_B3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040201000ULL);
        switch(n) {
            case 3: p.flip_BLACK_D7(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_B5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080800000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_A2(); [[fallthrough]];
            case 1: p.flip_BLACK_A3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000080808000ULL);
        switch(n) {
            case 3: p.flip_BLACK_A7(); [[fallthrough]];
            case 2: p.flip_BLACK_A6(); [[fallthrough]];
            case 1: p.flip_BLACK_A5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B4();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000003E00000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G4(); [[fallthrough]];
            case 4: p.flip_BLACK_F4(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010200000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_D2(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000020100800ULL);
        switch(n) {
            case 3: p.flip_BLACK_E7(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040400000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B2(); [[fallthrough]];
            case 1: p.flip_BLACK_B3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040404000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B7(); [[fallthrough]];
            case 2: p.flip_BLACK_B6(); [[fallthrough]];
            case 1: p.flip_BLACK_B5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C4();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000010080400ULL);
        switch(n) {
            case 3: p.flip_BLACK_F7(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_D5();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000020202000ULL);
        switch(n) {
            case 3: p.flip_BLACK_C7(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001E00000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G4(); [[fallthrough]];
            case 3: p.flip_BLACK_F4(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_D4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008100000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_E2(); [[fallthrough]];
            case 1: p.flip_BLACK_D3();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020200000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_C2(); [[fallthrough]];
            case 1: p.flip_BLACK_C3();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000400000000000ULL)
        p.flip_BLACK_B3();
    
    /* direction _SW */
    if(flipped & 0X0000000040000000ULL)
        p.flip_BLACK_B5();
    
    /* direction _W */
    if(flipped & 0X0000004000000000ULL)
        p.flip_BLACK_B4();
    
}

void RXBBPatterns::update_patterns_BLACK_D4(RXMove& move) const {}
void RXBBPatterns::update_patterns_BLACK_E4(RXMove& move) const {}


void RXBBPatterns::update_patterns_BLACK_F4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F4();
    
    /* direction _SE */
    if(flipped & 0X0000000002000000ULL)
        p.flip_BLACK_G5();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000004040400ULL);
        switch(n) {
            case 3: p.flip_BLACK_F7(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000200000000ULL)
        p.flip_BLACK_G4();
    
    /* direction NE */
    if(flipped & 0X0000020000000000ULL)
        p.flip_BLACK_G3();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_F2(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_D2(); [[fallthrough]];
            case 1: p.flip_BLACK_E3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000008102000ULL);
        switch(n) {
            case 3: p.flip_BLACK_C7(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_E5();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007800000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B4(); [[fallthrough]];
            case 3: p.flip_BLACK_C4(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_E4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G4();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007C00000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B4(); [[fallthrough]];
            case 4: p.flip_BLACK_C4(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000004081000ULL);
        switch(n) {
            case 3: p.flip_BLACK_D7(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_E2(); [[fallthrough]];
            case 1: p.flip_BLACK_F3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002020200ULL);
        switch(n) {
            case 3: p.flip_BLACK_G7(); [[fallthrough]];
            case 2: p.flip_BLACK_G6(); [[fallthrough]];
            case 1: p.flip_BLACK_G5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G2(); [[fallthrough]];
            case 1: p.flip_BLACK_G3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H4();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007E00000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B4(); [[fallthrough]];
            case 5: p.flip_BLACK_C4(); [[fallthrough]];
            case 4: p.flip_BLACK_D4(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_G4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040800ULL);
        switch(n) {
            case 3: p.flip_BLACK_E7(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_G5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0004020000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_F2(); [[fallthrough]];
            case 1: p.flip_BLACK_G3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000001010100ULL);
        switch(n) {
            case 3: p.flip_BLACK_H7(); [[fallthrough]];
            case 2: p.flip_BLACK_H6(); [[fallthrough]];
            case 1: p.flip_BLACK_H5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010000000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_H2(); [[fallthrough]];
            case 1: p.flip_BLACK_H3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A5();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007E000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G5(); [[fallthrough]];
            case 5: p.flip_BLACK_F5(); [[fallthrough]];
            case 4: p.flip_BLACK_E5(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_B5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010204000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_D2(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_B4();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000402000ULL);
        switch(n) {
            case 2: p.flip_BLACK_C7(); [[fallthrough]];
            case 1: p.flip_BLACK_B6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_A2(); [[fallthrough]];
            case 2: p.flip_BLACK_A3(); [[fallthrough]];
            case 1: p.flip_BLACK_A4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000808000ULL);
        switch(n) {
            case 2: p.flip_BLACK_A7(); [[fallthrough]];
            case 1: p.flip_BLACK_A6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B5();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000003E000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G5(); [[fallthrough]];
            case 4: p.flip_BLACK_F5(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_E2(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000201000ULL);
        switch(n) {
            case 2: p.flip_BLACK_D7(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B2(); [[fallthrough]];
            case 2: p.flip_BLACK_B3(); [[fallthrough]];
            case 1: p.flip_BLACK_B4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000404000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B7(); [[fallthrough]];
            case 1: p.flip_BLACK_B6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C5();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000100800ULL);
        switch(n) {
            case 2: p.flip_BLACK_E7(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000202000ULL);
        switch(n) {
            case 2: p.flip_BLACK_C7(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000001E000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G5(); [[fallthrough]];
            case 3: p.flip_BLACK_F5(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_D5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_F2(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_D4();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202000000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_C2(); [[fallthrough]];
            case 2: p.flip_BLACK_C3(); [[fallthrough]];
            case 1: p.flip_BLACK_C4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000004000000000ULL)
        p.flip_BLACK_B4();
    
    /* direction _SW */
    if(flipped & 0X0000000000400000ULL)
        p.flip_BLACK_B6();
    
    /* direction _W */
    if(flipped & 0X0000000040000000ULL)
        p.flip_BLACK_B5();
    
}

void RXBBPatterns::update_patterns_BLACK_D5(RXMove& move) const {}
void RXBBPatterns::update_patterns_BLACK_E5(RXMove& move) const {}

void RXBBPatterns::update_patterns_BLACK_F5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F5();
    
    /* direction _SE */
    if(flipped & 0X0000000000020000ULL)
        p.flip_BLACK_G6();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000040400ULL);
        switch(n) {
            case 2: p.flip_BLACK_F7(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000002000000ULL)
        p.flip_BLACK_G5();
    
    /* direction NE */
    if(flipped & 0X0000000200000000ULL)
        p.flip_BLACK_G4();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040400000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_F2(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100800000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_C2(); [[fallthrough]];
            case 2: p.flip_BLACK_D3(); [[fallthrough]];
            case 1: p.flip_BLACK_E4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000081000ULL);
        switch(n) {
            case 2: p.flip_BLACK_D7(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000078000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B5(); [[fallthrough]];
            case 3: p.flip_BLACK_C5(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_E5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G5();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007C000000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B5(); [[fallthrough]];
            case 4: p.flip_BLACK_C5(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000040800ULL);
        switch(n) {
            case 2: p.flip_BLACK_E7(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080400000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_D2(); [[fallthrough]];
            case 2: p.flip_BLACK_E3(); [[fallthrough]];
            case 1: p.flip_BLACK_F4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020200ULL);
        switch(n) {
            case 2: p.flip_BLACK_G7(); [[fallthrough]];
            case 1: p.flip_BLACK_G6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020200000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G2(); [[fallthrough]];
            case 2: p.flip_BLACK_G3(); [[fallthrough]];
            case 1: p.flip_BLACK_G4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H5();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007E000000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B5(); [[fallthrough]];
            case 5: p.flip_BLACK_C5(); [[fallthrough]];
            case 4: p.flip_BLACK_D5(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_G5();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020400ULL);
        switch(n) {
            case 2: p.flip_BLACK_F7(); [[fallthrough]];
            case 1: p.flip_BLACK_G6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040200000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_E2(); [[fallthrough]];
            case 2: p.flip_BLACK_F3(); [[fallthrough]];
            case 1: p.flip_BLACK_G4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000010100ULL);
        switch(n) {
            case 2: p.flip_BLACK_H7(); [[fallthrough]];
            case 1: p.flip_BLACK_H6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010100000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_H2(); [[fallthrough]];
            case 2: p.flip_BLACK_H3(); [[fallthrough]];
            case 1: p.flip_BLACK_H4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007E0000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G6(); [[fallthrough]];
            case 5: p.flip_BLACK_F6(); [[fallthrough]];
            case 4: p.flip_BLACK_E6(); [[fallthrough]];
            case 3: p.flip_BLACK_D6(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_B6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102040000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_E2(); [[fallthrough]];
            case 3: p.flip_BLACK_D3(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_B5();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000000000004000ULL)
        p.flip_BLACK_B7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_A2(); [[fallthrough]];
            case 3: p.flip_BLACK_A3(); [[fallthrough]];
            case 2: p.flip_BLACK_A4(); [[fallthrough]];
            case 1: p.flip_BLACK_A5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000008000ULL)
        p.flip_BLACK_A7();
    
}

void RXBBPatterns::update_patterns_BLACK_B6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000003E0000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G6(); [[fallthrough]];
            case 4: p.flip_BLACK_F6(); [[fallthrough]];
            case 3: p.flip_BLACK_E6(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_F2(); [[fallthrough]];
            case 3: p.flip_BLACK_E3(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000000000002000ULL)
        p.flip_BLACK_C7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B2(); [[fallthrough]];
            case 3: p.flip_BLACK_B3(); [[fallthrough]];
            case 2: p.flip_BLACK_B4(); [[fallthrough]];
            case 1: p.flip_BLACK_B5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000004000ULL)
        p.flip_BLACK_B7();
    
}

void RXBBPatterns::update_patterns_BLACK_C6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C6();
    
    /* direction _SE */
    if(flipped & 0X0000000000001000ULL)
        p.flip_BLACK_D7();
    
    /* direction S_ */
    if(flipped & 0X0000000000002000ULL)
        p.flip_BLACK_C7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000001E0000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G6(); [[fallthrough]];
            case 3: p.flip_BLACK_F6(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G2(); [[fallthrough]];
            case 3: p.flip_BLACK_F3(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_D5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_C2(); [[fallthrough]];
            case 3: p.flip_BLACK_C3(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000040000000ULL)
        p.flip_BLACK_B5();
    
    /* direction _SW */
    if(flipped & 0X0000000000004000ULL)
        p.flip_BLACK_B7();
    
    /* direction _W */
    if(flipped & 0X0000000000400000ULL)
        p.flip_BLACK_B6();
    
}

void RXBBPatterns::update_patterns_BLACK_D6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000800ULL)
        p.flip_BLACK_E7();
    
    /* direction S_ */
    if(flipped & 0X0000000000001000ULL)
        p.flip_BLACK_D7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000000E0000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G6(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G3(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_E5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_D2(); [[fallthrough]];
            case 3: p.flip_BLACK_D3(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_D5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B4(); [[fallthrough]];
            case 1: p.flip_BLACK_C5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000002000ULL)
        p.flip_BLACK_C7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000600000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B6(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000400ULL)
        p.flip_BLACK_F7();
    
    /* direction S_ */
    if(flipped & 0X0000000000000800ULL)
        p.flip_BLACK_E7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000060000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G6(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204000000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G4(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_E2(); [[fallthrough]];
            case 3: p.flip_BLACK_E3(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_E5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010000000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B3(); [[fallthrough]];
            case 2: p.flip_BLACK_C4(); [[fallthrough]];
            case 1: p.flip_BLACK_D5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000001000ULL)
        p.flip_BLACK_D7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000700000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B6(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000200ULL)
        p.flip_BLACK_G7();
    
    /* direction S_ */
    if(flipped & 0X0000000000000400ULL)
        p.flip_BLACK_F7();
    
    /* direction _E */
    if(flipped & 0X0000000000020000ULL)
        p.flip_BLACK_G6();
    
    /* direction NE */
    if(flipped & 0X0000000002000000ULL)
        p.flip_BLACK_G5();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_F2(); [[fallthrough]];
            case 3: p.flip_BLACK_F3(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B2(); [[fallthrough]];
            case 3: p.flip_BLACK_C3(); [[fallthrough]];
            case 2: p.flip_BLACK_D4(); [[fallthrough]];
            case 1: p.flip_BLACK_E5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000800ULL)
        p.flip_BLACK_E7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000780000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B6(); [[fallthrough]];
            case 3: p.flip_BLACK_C6(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G6();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007C0000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B6(); [[fallthrough]];
            case 4: p.flip_BLACK_C6(); [[fallthrough]];
            case 3: p.flip_BLACK_D6(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000400ULL)
        p.flip_BLACK_F7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_C2(); [[fallthrough]];
            case 3: p.flip_BLACK_D3(); [[fallthrough]];
            case 2: p.flip_BLACK_E4(); [[fallthrough]];
            case 1: p.flip_BLACK_F5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000000200ULL)
        p.flip_BLACK_G7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G2(); [[fallthrough]];
            case 3: p.flip_BLACK_G3(); [[fallthrough]];
            case 2: p.flip_BLACK_G4(); [[fallthrough]];
            case 1: p.flip_BLACK_G5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H6();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007E0000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B6(); [[fallthrough]];
            case 5: p.flip_BLACK_C6(); [[fallthrough]];
            case 4: p.flip_BLACK_D6(); [[fallthrough]];
            case 3: p.flip_BLACK_E6(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_G6();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000200ULL)
        p.flip_BLACK_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080402000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_D2(); [[fallthrough]];
            case 3: p.flip_BLACK_E3(); [[fallthrough]];
            case 2: p.flip_BLACK_F4(); [[fallthrough]];
            case 1: p.flip_BLACK_G5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000000100ULL)
        p.flip_BLACK_H7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101000000ULL);
        switch(n) {
            case 4: p.flip_BLACK_H2(); [[fallthrough]];
            case 3: p.flip_BLACK_H3(); [[fallthrough]];
            case 2: p.flip_BLACK_H4(); [[fallthrough]];
            case 1: p.flip_BLACK_H5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A7();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020400000ULL);
        switch(n) {
            case 5: p.flip_BLACK_F2(); [[fallthrough]];
            case 4: p.flip_BLACK_E3(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_B6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080800000ULL);
        switch(n) {
            case 5: p.flip_BLACK_A2(); [[fallthrough]];
            case 4: p.flip_BLACK_A3(); [[fallthrough]];
            case 3: p.flip_BLACK_A4(); [[fallthrough]];
            case 2: p.flip_BLACK_A5(); [[fallthrough]];
            case 1: p.flip_BLACK_A6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007E00ULL);
        switch(n) {
            case 6: p.flip_BLACK_G7(); [[fallthrough]];
            case 5: p.flip_BLACK_F7(); [[fallthrough]];
            case 4: p.flip_BLACK_E7(); [[fallthrough]];
            case 3: p.flip_BLACK_D7(); [[fallthrough]];
            case 2: p.flip_BLACK_C7(); [[fallthrough]];
            case 1: p.flip_BLACK_B7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B7();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810200000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G2(); [[fallthrough]];
            case 4: p.flip_BLACK_F3(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040400000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B2(); [[fallthrough]];
            case 4: p.flip_BLACK_B3(); [[fallthrough]];
            case 3: p.flip_BLACK_B4(); [[fallthrough]];
            case 2: p.flip_BLACK_B5(); [[fallthrough]];
            case 1: p.flip_BLACK_B6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000003E00ULL);
        switch(n) {
            case 5: p.flip_BLACK_G7(); [[fallthrough]];
            case 4: p.flip_BLACK_F7(); [[fallthrough]];
            case 3: p.flip_BLACK_E7(); [[fallthrough]];
            case 2: p.flip_BLACK_D7(); [[fallthrough]];
            case 1: p.flip_BLACK_C7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020200000ULL);
        switch(n) {
            case 5: p.flip_BLACK_C2(); [[fallthrough]];
            case 4: p.flip_BLACK_C3(); [[fallthrough]];
            case 3: p.flip_BLACK_C4(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408100000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G3(); [[fallthrough]];
            case 3: p.flip_BLACK_F4(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000000400000ULL)
        p.flip_BLACK_B6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000001E00ULL);
        switch(n) {
            case 4: p.flip_BLACK_G7(); [[fallthrough]];
            case 3: p.flip_BLACK_F7(); [[fallthrough]];
            case 2: p.flip_BLACK_E7(); [[fallthrough]];
            case 1: p.flip_BLACK_D7();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0000000000004000ULL)
        p.flip_BLACK_B7();
    
}

void RXBBPatterns::update_patterns_BLACK_D7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010100000ULL);
        switch(n) {
            case 5: p.flip_BLACK_D2(); [[fallthrough]];
            case 4: p.flip_BLACK_D3(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204080000ULL);
        switch(n) {
            case 3: p.flip_BLACK_G4(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040200000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B5(); [[fallthrough]];
            case 1: p.flip_BLACK_C6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000E00ULL);
        switch(n) {
            case 3: p.flip_BLACK_G7(); [[fallthrough]];
            case 2: p.flip_BLACK_F7(); [[fallthrough]];
            case 1: p.flip_BLACK_E7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000006000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B7(); [[fallthrough]];
            case 1: p.flip_BLACK_C7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080000ULL);
        switch(n) {
            case 5: p.flip_BLACK_E2(); [[fallthrough]];
            case 4: p.flip_BLACK_E3(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040000ULL);
        switch(n) {
            case 2: p.flip_BLACK_G5(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B4(); [[fallthrough]];
            case 2: p.flip_BLACK_C5(); [[fallthrough]];
            case 1: p.flip_BLACK_D6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000600ULL);
        switch(n) {
            case 2: p.flip_BLACK_G7(); [[fallthrough]];
            case 1: p.flip_BLACK_F7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B7(); [[fallthrough]];
            case 2: p.flip_BLACK_C7(); [[fallthrough]];
            case 1: p.flip_BLACK_D7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040000ULL);
        switch(n) {
            case 5: p.flip_BLACK_F2(); [[fallthrough]];
            case 4: p.flip_BLACK_F3(); [[fallthrough]];
            case 3: p.flip_BLACK_F4(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0000000000020000ULL)
        p.flip_BLACK_G6();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080000ULL);
        switch(n) {
            case 4: p.flip_BLACK_B3(); [[fallthrough]];
            case 3: p.flip_BLACK_C4(); [[fallthrough]];
            case 2: p.flip_BLACK_D5(); [[fallthrough]];
            case 1: p.flip_BLACK_E6();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000000000200ULL)
        p.flip_BLACK_G7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007800ULL);
        switch(n) {
            case 4: p.flip_BLACK_B7(); [[fallthrough]];
            case 3: p.flip_BLACK_C7(); [[fallthrough]];
            case 2: p.flip_BLACK_D7(); [[fallthrough]];
            case 1: p.flip_BLACK_E7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040000ULL);
        switch(n) {
            case 5: p.flip_BLACK_B2(); [[fallthrough]];
            case 4: p.flip_BLACK_C3(); [[fallthrough]];
            case 3: p.flip_BLACK_D4(); [[fallthrough]];
            case 2: p.flip_BLACK_E5(); [[fallthrough]];
            case 1: p.flip_BLACK_F6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G2(); [[fallthrough]];
            case 4: p.flip_BLACK_G3(); [[fallthrough]];
            case 3: p.flip_BLACK_G4(); [[fallthrough]];
            case 2: p.flip_BLACK_G5(); [[fallthrough]];
            case 1: p.flip_BLACK_G6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007C00ULL);
        switch(n) {
            case 5: p.flip_BLACK_B7(); [[fallthrough]];
            case 4: p.flip_BLACK_C7(); [[fallthrough]];
            case 3: p.flip_BLACK_D7(); [[fallthrough]];
            case 2: p.flip_BLACK_E7(); [[fallthrough]];
            case 1: p.flip_BLACK_F7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804020000ULL);
        switch(n) {
            case 5: p.flip_BLACK_C2(); [[fallthrough]];
            case 4: p.flip_BLACK_D3(); [[fallthrough]];
            case 3: p.flip_BLACK_E4(); [[fallthrough]];
            case 2: p.flip_BLACK_F5(); [[fallthrough]];
            case 1: p.flip_BLACK_G6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010000ULL);
        switch(n) {
            case 5: p.flip_BLACK_H2(); [[fallthrough]];
            case 4: p.flip_BLACK_H3(); [[fallthrough]];
            case 3: p.flip_BLACK_H4(); [[fallthrough]];
            case 2: p.flip_BLACK_H5(); [[fallthrough]];
            case 1: p.flip_BLACK_H6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007E00ULL);
        switch(n) {
            case 6: p.flip_BLACK_B7(); [[fallthrough]];
            case 5: p.flip_BLACK_C7(); [[fallthrough]];
            case 4: p.flip_BLACK_D7(); [[fallthrough]];
            case 3: p.flip_BLACK_E7(); [[fallthrough]];
            case 2: p.flip_BLACK_F7(); [[fallthrough]];
            case 1: p.flip_BLACK_G7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_A8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_A8();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810204000ULL);
        switch(n) {
            case 6: p.flip_BLACK_G2(); [[fallthrough]];
            case 5: p.flip_BLACK_F3(); [[fallthrough]];
            case 4: p.flip_BLACK_E4(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_B7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080808000ULL);
        switch(n) {
            case 6: p.flip_BLACK_A2(); [[fallthrough]];
            case 5: p.flip_BLACK_A3(); [[fallthrough]];
            case 4: p.flip_BLACK_A4(); [[fallthrough]];
            case 3: p.flip_BLACK_A5(); [[fallthrough]];
            case 2: p.flip_BLACK_A6(); [[fallthrough]];
            case 1: p.flip_BLACK_A7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007EULL);
        switch(n) {
            case 6: p.flip_BLACK_G8(); [[fallthrough]];
            case 5: p.flip_BLACK_F8(); [[fallthrough]];
            case 4: p.flip_BLACK_E8(); [[fallthrough]];
            case 3: p.flip_BLACK_D8(); [[fallthrough]];
            case 2: p.flip_BLACK_C8(); [[fallthrough]];
            case 1: p.flip_BLACK_B8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_B8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_B8();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408102000ULL);
        switch(n) {
            case 5: p.flip_BLACK_G3(); [[fallthrough]];
            case 4: p.flip_BLACK_F4(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_C7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040404000ULL);
        switch(n) {
            case 6: p.flip_BLACK_B2(); [[fallthrough]];
            case 5: p.flip_BLACK_B3(); [[fallthrough]];
            case 4: p.flip_BLACK_B4(); [[fallthrough]];
            case 3: p.flip_BLACK_B5(); [[fallthrough]];
            case 2: p.flip_BLACK_B6(); [[fallthrough]];
            case 1: p.flip_BLACK_B7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000003EULL);
        switch(n) {
            case 5: p.flip_BLACK_G8(); [[fallthrough]];
            case 4: p.flip_BLACK_F8(); [[fallthrough]];
            case 3: p.flip_BLACK_E8(); [[fallthrough]];
            case 2: p.flip_BLACK_D8(); [[fallthrough]];
            case 1: p.flip_BLACK_C8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_C8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_C8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020202000ULL);
        switch(n) {
            case 6: p.flip_BLACK_C2(); [[fallthrough]];
            case 5: p.flip_BLACK_C3(); [[fallthrough]];
            case 4: p.flip_BLACK_C4(); [[fallthrough]];
            case 3: p.flip_BLACK_C5(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_C7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204081000ULL);
        switch(n) {
            case 4: p.flip_BLACK_G4(); [[fallthrough]];
            case 3: p.flip_BLACK_F5(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_D7();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000000004000ULL)
        p.flip_BLACK_B7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000001EULL);
        switch(n) {
            case 4: p.flip_BLACK_G8(); [[fallthrough]];
            case 3: p.flip_BLACK_F8(); [[fallthrough]];
            case 2: p.flip_BLACK_E8(); [[fallthrough]];
            case 1: p.flip_BLACK_D8();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0000000000000040ULL)
        p.flip_BLACK_B8();
    
}

void RXBBPatterns::update_patterns_BLACK_D8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_D8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010101000ULL);
        switch(n) {
            case 6: p.flip_BLACK_D2(); [[fallthrough]];
            case 5: p.flip_BLACK_D3(); [[fallthrough]];
            case 4: p.flip_BLACK_D4(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_D7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040800ULL);
        switch(n) {
            case 3: p.flip_BLACK_G5(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_E7();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000402000ULL);
        switch(n) {
            case 2: p.flip_BLACK_B6(); [[fallthrough]];
            case 1: p.flip_BLACK_C7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000000EULL);
        switch(n) {
            case 3: p.flip_BLACK_G8(); [[fallthrough]];
            case 2: p.flip_BLACK_F8(); [[fallthrough]];
            case 1: p.flip_BLACK_E8();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000060ULL);
        switch(n) {
            case 2: p.flip_BLACK_B8(); [[fallthrough]];
            case 1: p.flip_BLACK_C8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_E8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_E8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080800ULL);
        switch(n) {
            case 6: p.flip_BLACK_E2(); [[fallthrough]];
            case 5: p.flip_BLACK_E3(); [[fallthrough]];
            case 4: p.flip_BLACK_E4(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_E7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020400ULL);
        switch(n) {
            case 2: p.flip_BLACK_G6(); [[fallthrough]];
            case 1: p.flip_BLACK_F7();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040201000ULL);
        switch(n) {
            case 3: p.flip_BLACK_B5(); [[fallthrough]];
            case 2: p.flip_BLACK_C6(); [[fallthrough]];
            case 1: p.flip_BLACK_D7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000006ULL);
        switch(n) {
            case 2: p.flip_BLACK_G8(); [[fallthrough]];
            case 1: p.flip_BLACK_F8();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000070ULL);
        switch(n) {
            case 3: p.flip_BLACK_B8(); [[fallthrough]];
            case 2: p.flip_BLACK_C8(); [[fallthrough]];
            case 1: p.flip_BLACK_D8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_F8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_F8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040400ULL);
        switch(n) {
            case 6: p.flip_BLACK_F2(); [[fallthrough]];
            case 5: p.flip_BLACK_F3(); [[fallthrough]];
            case 4: p.flip_BLACK_F4(); [[fallthrough]];
            case 3: p.flip_BLACK_F5(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_F7();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0000000000000200ULL)
        p.flip_BLACK_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100800ULL);
        switch(n) {
            case 4: p.flip_BLACK_B4(); [[fallthrough]];
            case 3: p.flip_BLACK_C5(); [[fallthrough]];
            case 2: p.flip_BLACK_D6(); [[fallthrough]];
            case 1: p.flip_BLACK_E7();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000000000002ULL)
        p.flip_BLACK_G8();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000078ULL);
        switch(n) {
            case 4: p.flip_BLACK_B8(); [[fallthrough]];
            case 3: p.flip_BLACK_C8(); [[fallthrough]];
            case 2: p.flip_BLACK_D8(); [[fallthrough]];
            case 1: p.flip_BLACK_E8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_G8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_G8();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080400ULL);
        switch(n) {
            case 5: p.flip_BLACK_B3(); [[fallthrough]];
            case 4: p.flip_BLACK_C4(); [[fallthrough]];
            case 3: p.flip_BLACK_D5(); [[fallthrough]];
            case 2: p.flip_BLACK_E6(); [[fallthrough]];
            case 1: p.flip_BLACK_F7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020200ULL);
        switch(n) {
            case 6: p.flip_BLACK_G2(); [[fallthrough]];
            case 5: p.flip_BLACK_G3(); [[fallthrough]];
            case 4: p.flip_BLACK_G4(); [[fallthrough]];
            case 3: p.flip_BLACK_G5(); [[fallthrough]];
            case 2: p.flip_BLACK_G6(); [[fallthrough]];
            case 1: p.flip_BLACK_G7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007CULL);
        switch(n) {
            case 5: p.flip_BLACK_B8(); [[fallthrough]];
            case 4: p.flip_BLACK_C8(); [[fallthrough]];
            case 3: p.flip_BLACK_D8(); [[fallthrough]];
            case 2: p.flip_BLACK_E8(); [[fallthrough]];
            case 1: p.flip_BLACK_F8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_BLACK_H8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_BLACK_H8();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040200ULL);
        switch(n) {
            case 6: p.flip_BLACK_B2(); [[fallthrough]];
            case 5: p.flip_BLACK_C3(); [[fallthrough]];
            case 4: p.flip_BLACK_D4(); [[fallthrough]];
            case 3: p.flip_BLACK_E5(); [[fallthrough]];
            case 2: p.flip_BLACK_F6(); [[fallthrough]];
            case 1: p.flip_BLACK_G7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010100ULL);
        switch(n) {
            case 6: p.flip_BLACK_H2(); [[fallthrough]];
            case 5: p.flip_BLACK_H3(); [[fallthrough]];
            case 4: p.flip_BLACK_H4(); [[fallthrough]];
            case 3: p.flip_BLACK_H5(); [[fallthrough]];
            case 2: p.flip_BLACK_H6(); [[fallthrough]];
            case 1: p.flip_BLACK_H7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007EULL);
        switch(n) {
            case 6: p.flip_BLACK_B8(); [[fallthrough]];
            case 5: p.flip_BLACK_C8(); [[fallthrough]];
            case 4: p.flip_BLACK_D8(); [[fallthrough]];
            case 3: p.flip_BLACK_E8(); [[fallthrough]];
            case 2: p.flip_BLACK_F8(); [[fallthrough]];
            case 1: p.flip_BLACK_G8();
        }
    }
    
    
}



void RXBBPatterns::update_patterns_WHITE_A1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A1();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040200ULL);
        switch(n) {
            case 6: p.flip_WHITE_G7(); [[fallthrough]];
            case 5: p.flip_WHITE_F6(); [[fallthrough]];
            case 4: p.flip_WHITE_E5(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_B2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080808000ULL);
        switch(n) {
            case 6: p.flip_WHITE_A7(); [[fallthrough]];
            case 5: p.flip_WHITE_A6(); [[fallthrough]];
            case 4: p.flip_WHITE_A5(); [[fallthrough]];
            case 3: p.flip_WHITE_A4(); [[fallthrough]];
            case 2: p.flip_WHITE_A3(); [[fallthrough]];
            case 1: p.flip_WHITE_A2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X7E00000000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G1(); [[fallthrough]];
            case 5: p.flip_WHITE_F1(); [[fallthrough]];
            case 4: p.flip_WHITE_E1(); [[fallthrough]];
            case 3: p.flip_WHITE_D1(); [[fallthrough]];
            case 2: p.flip_WHITE_C1(); [[fallthrough]];
            case 1: p.flip_WHITE_B1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B1();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804020000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G6(); [[fallthrough]];
            case 4: p.flip_WHITE_F5(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_C2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040404000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B7(); [[fallthrough]];
            case 5: p.flip_WHITE_B6(); [[fallthrough]];
            case 4: p.flip_WHITE_B5(); [[fallthrough]];
            case 3: p.flip_WHITE_B4(); [[fallthrough]];
            case 2: p.flip_WHITE_B3(); [[fallthrough]];
            case 1: p.flip_WHITE_B2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X3E00000000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G1(); [[fallthrough]];
            case 4: p.flip_WHITE_F1(); [[fallthrough]];
            case 3: p.flip_WHITE_E1(); [[fallthrough]];
            case 2: p.flip_WHITE_D1(); [[fallthrough]];
            case 1: p.flip_WHITE_C1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020202000ULL);
        switch(n) {
            case 6: p.flip_WHITE_C7(); [[fallthrough]];
            case 5: p.flip_WHITE_C6(); [[fallthrough]];
            case 4: p.flip_WHITE_C5(); [[fallthrough]];
            case 3: p.flip_WHITE_C4(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_C2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080402000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G5(); [[fallthrough]];
            case 3: p.flip_WHITE_F4(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_D2();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0040000000000000ULL)
        p.flip_WHITE_B2();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X1E00000000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G1(); [[fallthrough]];
            case 3: p.flip_WHITE_F1(); [[fallthrough]];
            case 2: p.flip_WHITE_E1(); [[fallthrough]];
            case 1: p.flip_WHITE_D1();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X4000000000000000ULL)
        p.flip_WHITE_B1();
    
}

void RXBBPatterns::update_patterns_WHITE_D1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010101000ULL);
        switch(n) {
            case 6: p.flip_WHITE_D7(); [[fallthrough]];
            case 5: p.flip_WHITE_D6(); [[fallthrough]];
            case 4: p.flip_WHITE_D5(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_D2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040200000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G4(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_E2();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020400000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B3(); [[fallthrough]];
            case 1: p.flip_WHITE_C2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0E00000000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G1(); [[fallthrough]];
            case 2: p.flip_WHITE_F1(); [[fallthrough]];
            case 1: p.flip_WHITE_E1();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X6000000000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B1(); [[fallthrough]];
            case 1: p.flip_WHITE_C1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080800ULL);
        switch(n) {
            case 6: p.flip_WHITE_E7(); [[fallthrough]];
            case 5: p.flip_WHITE_E6(); [[fallthrough]];
            case 4: p.flip_WHITE_E5(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_E2();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004020000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G3(); [[fallthrough]];
            case 1: p.flip_WHITE_F2();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010204000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B4(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_D2();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0600000000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G1(); [[fallthrough]];
            case 1: p.flip_WHITE_F1();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7000000000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B1(); [[fallthrough]];
            case 2: p.flip_WHITE_C1(); [[fallthrough]];
            case 1: p.flip_WHITE_D1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F1();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040400ULL);
        switch(n) {
            case 6: p.flip_WHITE_F7(); [[fallthrough]];
            case 5: p.flip_WHITE_F6(); [[fallthrough]];
            case 4: p.flip_WHITE_F5(); [[fallthrough]];
            case 3: p.flip_WHITE_F4(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_F2();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0002000000000000ULL)
        p.flip_WHITE_G2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102040000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B5(); [[fallthrough]];
            case 3: p.flip_WHITE_C4(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_E2();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0200000000000000ULL)
        p.flip_WHITE_G1();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7800000000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B1(); [[fallthrough]];
            case 3: p.flip_WHITE_C1(); [[fallthrough]];
            case 2: p.flip_WHITE_D1(); [[fallthrough]];
            case 1: p.flip_WHITE_E1();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G1();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020400000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B6(); [[fallthrough]];
            case 4: p.flip_WHITE_C5(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_F2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7C00000000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B1(); [[fallthrough]];
            case 4: p.flip_WHITE_C1(); [[fallthrough]];
            case 3: p.flip_WHITE_D1(); [[fallthrough]];
            case 2: p.flip_WHITE_E1(); [[fallthrough]];
            case 1: p.flip_WHITE_F1();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020200ULL);
        switch(n) {
            case 6: p.flip_WHITE_G7(); [[fallthrough]];
            case 5: p.flip_WHITE_G6(); [[fallthrough]];
            case 4: p.flip_WHITE_G5(); [[fallthrough]];
            case 3: p.flip_WHITE_G4(); [[fallthrough]];
            case 2: p.flip_WHITE_G3(); [[fallthrough]];
            case 1: p.flip_WHITE_G2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H1(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H1();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810204000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B7(); [[fallthrough]];
            case 5: p.flip_WHITE_C6(); [[fallthrough]];
            case 4: p.flip_WHITE_D5(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_G2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X7E00000000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B1(); [[fallthrough]];
            case 5: p.flip_WHITE_C1(); [[fallthrough]];
            case 4: p.flip_WHITE_D1(); [[fallthrough]];
            case 3: p.flip_WHITE_E1(); [[fallthrough]];
            case 2: p.flip_WHITE_F1(); [[fallthrough]];
            case 1: p.flip_WHITE_G1();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010100ULL);
        switch(n) {
            case 6: p.flip_WHITE_H7(); [[fallthrough]];
            case 5: p.flip_WHITE_H6(); [[fallthrough]];
            case 4: p.flip_WHITE_H5(); [[fallthrough]];
            case 3: p.flip_WHITE_H4(); [[fallthrough]];
            case 2: p.flip_WHITE_H3(); [[fallthrough]];
            case 1: p.flip_WHITE_H2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080400ULL);
        switch(n) {
            case 5: p.flip_WHITE_F7(); [[fallthrough]];
            case 4: p.flip_WHITE_E6(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_B3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000808080808000ULL);
        switch(n) {
            case 5: p.flip_WHITE_A7(); [[fallthrough]];
            case 4: p.flip_WHITE_A6(); [[fallthrough]];
            case 3: p.flip_WHITE_A5(); [[fallthrough]];
            case 2: p.flip_WHITE_A4(); [[fallthrough]];
            case 1: p.flip_WHITE_A3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X007E000000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G2(); [[fallthrough]];
            case 5: p.flip_WHITE_F2(); [[fallthrough]];
            case 4: p.flip_WHITE_E2(); [[fallthrough]];
            case 3: p.flip_WHITE_D2(); [[fallthrough]];
            case 2: p.flip_WHITE_C2(); [[fallthrough]];
            case 1: p.flip_WHITE_B2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000201008040200ULL);
        switch(n) {
            case 5: p.flip_WHITE_G7(); [[fallthrough]];
            case 4: p.flip_WHITE_F6(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000404040404000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B7(); [[fallthrough]];
            case 4: p.flip_WHITE_B6(); [[fallthrough]];
            case 3: p.flip_WHITE_B5(); [[fallthrough]];
            case 2: p.flip_WHITE_B4(); [[fallthrough]];
            case 1: p.flip_WHITE_B3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X003E000000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G2(); [[fallthrough]];
            case 4: p.flip_WHITE_F2(); [[fallthrough]];
            case 3: p.flip_WHITE_E2(); [[fallthrough]];
            case 2: p.flip_WHITE_D2(); [[fallthrough]];
            case 1: p.flip_WHITE_C2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000202020202000ULL);
        switch(n) {
            case 5: p.flip_WHITE_C7(); [[fallthrough]];
            case 4: p.flip_WHITE_C6(); [[fallthrough]];
            case 3: p.flip_WHITE_C5(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000100804020000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G6(); [[fallthrough]];
            case 3: p.flip_WHITE_F5(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000400000000000ULL)
        p.flip_WHITE_B3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X001E000000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G2(); [[fallthrough]];
            case 3: p.flip_WHITE_F2(); [[fallthrough]];
            case 2: p.flip_WHITE_E2(); [[fallthrough]];
            case 1: p.flip_WHITE_D2();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0040000000000000ULL)
        p.flip_WHITE_B2();
    
}

void RXBBPatterns::update_patterns_WHITE_D2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000101010101000ULL);
        switch(n) {
            case 5: p.flip_WHITE_D7(); [[fallthrough]];
            case 4: p.flip_WHITE_D6(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000080402000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G5(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000204000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B4(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000E000000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G2(); [[fallthrough]];
            case 2: p.flip_WHITE_F2(); [[fallthrough]];
            case 1: p.flip_WHITE_E2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0060000000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B2(); [[fallthrough]];
            case 1: p.flip_WHITE_C2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000080808080800ULL);
        switch(n) {
            case 5: p.flip_WHITE_E7(); [[fallthrough]];
            case 4: p.flip_WHITE_E6(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040200000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G4(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000102040000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B5(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0006000000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G2(); [[fallthrough]];
            case 1: p.flip_WHITE_F2();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0070000000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B2(); [[fallthrough]];
            case 2: p.flip_WHITE_C2(); [[fallthrough]];
            case 1: p.flip_WHITE_D2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040404040400ULL);
        switch(n) {
            case 5: p.flip_WHITE_F7(); [[fallthrough]];
            case 4: p.flip_WHITE_F6(); [[fallthrough]];
            case 3: p.flip_WHITE_F5(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000020000000000ULL)
        p.flip_WHITE_G3();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000081020400000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B6(); [[fallthrough]];
            case 3: p.flip_WHITE_C5(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0002000000000000ULL)
        p.flip_WHITE_G2();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0078000000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B2(); [[fallthrough]];
            case 3: p.flip_WHITE_C2(); [[fallthrough]];
            case 2: p.flip_WHITE_D2(); [[fallthrough]];
            case 1: p.flip_WHITE_E2();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000040810204000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B7(); [[fallthrough]];
            case 4: p.flip_WHITE_C6(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X007C000000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B2(); [[fallthrough]];
            case 4: p.flip_WHITE_C2(); [[fallthrough]];
            case 3: p.flip_WHITE_D2(); [[fallthrough]];
            case 2: p.flip_WHITE_E2(); [[fallthrough]];
            case 1: p.flip_WHITE_F2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020202020200ULL);
        switch(n) {
            case 5: p.flip_WHITE_G7(); [[fallthrough]];
            case 4: p.flip_WHITE_G6(); [[fallthrough]];
            case 3: p.flip_WHITE_G5(); [[fallthrough]];
            case 2: p.flip_WHITE_G4(); [[fallthrough]];
            case 1: p.flip_WHITE_G3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H2(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408102000ULL);
        switch(n) {
            case 5: p.flip_WHITE_C7(); [[fallthrough]];
            case 4: p.flip_WHITE_D6(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_G3();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X007E000000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B2(); [[fallthrough]];
            case 5: p.flip_WHITE_C2(); [[fallthrough]];
            case 4: p.flip_WHITE_D2(); [[fallthrough]];
            case 3: p.flip_WHITE_E2(); [[fallthrough]];
            case 2: p.flip_WHITE_F2(); [[fallthrough]];
            case 1: p.flip_WHITE_G2();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000010101010100ULL);
        switch(n) {
            case 5: p.flip_WHITE_H7(); [[fallthrough]];
            case 4: p.flip_WHITE_H6(); [[fallthrough]];
            case 3: p.flip_WHITE_H5(); [[fallthrough]];
            case 2: p.flip_WHITE_H4(); [[fallthrough]];
            case 1: p.flip_WHITE_H3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00007E0000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G3(); [[fallthrough]];
            case 5: p.flip_WHITE_F3(); [[fallthrough]];
            case 4: p.flip_WHITE_E3(); [[fallthrough]];
            case 3: p.flip_WHITE_D3(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_B3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0040000000000000ULL)
        p.flip_WHITE_B2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100800ULL);
        switch(n) {
            case 4: p.flip_WHITE_E7(); [[fallthrough]];
            case 3: p.flip_WHITE_D6(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_B4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0080000000000000ULL)
        p.flip_WHITE_A2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000008080808000ULL);
        switch(n) {
            case 4: p.flip_WHITE_A7(); [[fallthrough]];
            case 3: p.flip_WHITE_A6(); [[fallthrough]];
            case 2: p.flip_WHITE_A5(); [[fallthrough]];
            case 1: p.flip_WHITE_A4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B3();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00003E0000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G3(); [[fallthrough]];
            case 4: p.flip_WHITE_F3(); [[fallthrough]];
            case 3: p.flip_WHITE_E3(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0020000000000000ULL)
        p.flip_WHITE_C2();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002010080400ULL);
        switch(n) {
            case 4: p.flip_WHITE_F7(); [[fallthrough]];
            case 3: p.flip_WHITE_E6(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0040000000000000ULL)
        p.flip_WHITE_B2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004040404000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B7(); [[fallthrough]];
            case 3: p.flip_WHITE_B6(); [[fallthrough]];
            case 2: p.flip_WHITE_B5(); [[fallthrough]];
            case 1: p.flip_WHITE_B4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001008040200ULL);
        switch(n) {
            case 4: p.flip_WHITE_G7(); [[fallthrough]];
            case 3: p.flip_WHITE_F6(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_D4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002020202000ULL);
        switch(n) {
            case 4: p.flip_WHITE_C7(); [[fallthrough]];
            case 3: p.flip_WHITE_C6(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00001E0000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G3(); [[fallthrough]];
            case 3: p.flip_WHITE_F3(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0010000000000000ULL)
        p.flip_WHITE_D2();
    
    /* direction _N */
    if(flipped & 0X0020000000000000ULL)
        p.flip_WHITE_C2();
    
    /* direction NW */
    if(flipped & 0X0040000000000000ULL)
        p.flip_WHITE_B2();
    
    /* direction _SW */
    if(flipped & 0X0000004000000000ULL)
        p.flip_WHITE_B4();
    
    /* direction _W */
    if(flipped & 0X0000400000000000ULL)
        p.flip_WHITE_B3();
    
}

void RXBBPatterns::update_patterns_WHITE_D3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000804020000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G6(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_E4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001010101000ULL);
        switch(n) {
            case 4: p.flip_WHITE_D7(); [[fallthrough]];
            case 3: p.flip_WHITE_D6(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_D4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000E0000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G3(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0008000000000000ULL)
        p.flip_WHITE_E2();
    
    /* direction _N */
    if(flipped & 0X0010000000000000ULL)
        p.flip_WHITE_D2();
    
    /* direction NW */
    if(flipped & 0X0020000000000000ULL)
        p.flip_WHITE_C2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000002040000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B5(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000600000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B3(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E3();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000402000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G5(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000808080800ULL);
        switch(n) {
            case 4: p.flip_WHITE_E7(); [[fallthrough]];
            case 3: p.flip_WHITE_E6(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_E4();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000060000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G3(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0004000000000000ULL)
        p.flip_WHITE_F2();
    
    /* direction _N */
    if(flipped & 0X0008000000000000ULL)
        p.flip_WHITE_E2();
    
    /* direction NW */
    if(flipped & 0X0010000000000000ULL)
        p.flip_WHITE_D2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001020400000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B6(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_D4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000700000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B3(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F3();
    
    /* direction _SE */
    if(flipped & 0X0000000200000000ULL)
        p.flip_WHITE_G4();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000404040400ULL);
        switch(n) {
            case 4: p.flip_WHITE_F7(); [[fallthrough]];
            case 3: p.flip_WHITE_F6(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000020000000000ULL)
        p.flip_WHITE_G3();
    
    /* direction NE */
    if(flipped & 0X0002000000000000ULL)
        p.flip_WHITE_G2();
    
    /* direction _N */
    if(flipped & 0X0004000000000000ULL)
        p.flip_WHITE_F2();
    
    /* direction NW */
    if(flipped & 0X0008000000000000ULL)
        p.flip_WHITE_E2();
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000810204000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B7(); [[fallthrough]];
            case 3: p.flip_WHITE_C6(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_E4();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000780000000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B3(); [[fallthrough]];
            case 3: p.flip_WHITE_C3(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G3();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00007C0000000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B3(); [[fallthrough]];
            case 4: p.flip_WHITE_C3(); [[fallthrough]];
            case 3: p.flip_WHITE_D3(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000408102000ULL);
        switch(n) {
            case 4: p.flip_WHITE_C7(); [[fallthrough]];
            case 3: p.flip_WHITE_D6(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0004000000000000ULL)
        p.flip_WHITE_F2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000202020200ULL);
        switch(n) {
            case 4: p.flip_WHITE_G7(); [[fallthrough]];
            case 3: p.flip_WHITE_G6(); [[fallthrough]];
            case 2: p.flip_WHITE_G5(); [[fallthrough]];
            case 1: p.flip_WHITE_G4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0002000000000000ULL)
        p.flip_WHITE_G2();
    
}

void RXBBPatterns::update_patterns_WHITE_H3(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H3();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00007E0000000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B3(); [[fallthrough]];
            case 5: p.flip_WHITE_C3(); [[fallthrough]];
            case 4: p.flip_WHITE_D3(); [[fallthrough]];
            case 3: p.flip_WHITE_E3(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_G3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204081000ULL);
        switch(n) {
            case 4: p.flip_WHITE_D7(); [[fallthrough]];
            case 3: p.flip_WHITE_E6(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_G4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0002000000000000ULL)
        p.flip_WHITE_G2();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000101010100ULL);
        switch(n) {
            case 4: p.flip_WHITE_H7(); [[fallthrough]];
            case 3: p.flip_WHITE_H6(); [[fallthrough]];
            case 2: p.flip_WHITE_H5(); [[fallthrough]];
            case 1: p.flip_WHITE_H4();
        }
    }
    
    
    /* direction _N */
    if(flipped & 0X0001000000000000ULL)
        p.flip_WHITE_H2();
    
}

void RXBBPatterns::update_patterns_WHITE_A4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A4();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007E00000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G4(); [[fallthrough]];
            case 5: p.flip_WHITE_F4(); [[fallthrough]];
            case 4: p.flip_WHITE_E4(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_B4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0020400000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_C2(); [[fallthrough]];
            case 1: p.flip_WHITE_B3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040201000ULL);
        switch(n) {
            case 3: p.flip_WHITE_D7(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_B5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080800000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_A2(); [[fallthrough]];
            case 1: p.flip_WHITE_A3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000080808000ULL);
        switch(n) {
            case 3: p.flip_WHITE_A7(); [[fallthrough]];
            case 2: p.flip_WHITE_A6(); [[fallthrough]];
            case 1: p.flip_WHITE_A5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B4();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000003E00000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G4(); [[fallthrough]];
            case 4: p.flip_WHITE_F4(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010200000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_D2(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000020100800ULL);
        switch(n) {
            case 3: p.flip_WHITE_E7(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040400000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B2(); [[fallthrough]];
            case 1: p.flip_WHITE_B3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040404000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B7(); [[fallthrough]];
            case 2: p.flip_WHITE_B6(); [[fallthrough]];
            case 1: p.flip_WHITE_B5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C4();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000010080400ULL);
        switch(n) {
            case 3: p.flip_WHITE_F7(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_D5();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000020202000ULL);
        switch(n) {
            case 3: p.flip_WHITE_C7(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000001E00000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G4(); [[fallthrough]];
            case 3: p.flip_WHITE_F4(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_D4();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008100000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_E2(); [[fallthrough]];
            case 1: p.flip_WHITE_D3();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020200000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_C2(); [[fallthrough]];
            case 1: p.flip_WHITE_C3();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000400000000000ULL)
        p.flip_WHITE_B3();
    
    /* direction _SW */
    if(flipped & 0X0000000040000000ULL)
        p.flip_WHITE_B5();
    
    /* direction _W */
    if(flipped & 0X0000004000000000ULL)
        p.flip_WHITE_B4();
    
}

void RXBBPatterns::update_patterns_WHITE_D4(RXMove& move) const {}
void RXBBPatterns::update_patterns_WHITE_E4(RXMove& move) const {}

void RXBBPatterns::update_patterns_WHITE_F4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F4();
    
    /* direction _SE */
    if(flipped & 0X0000000002000000ULL)
        p.flip_WHITE_G5();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000004040400ULL);
        switch(n) {
            case 3: p.flip_WHITE_F7(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000200000000ULL)
        p.flip_WHITE_G4();
    
    /* direction NE */
    if(flipped & 0X0000020000000000ULL)
        p.flip_WHITE_G3();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_F2(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_D2(); [[fallthrough]];
            case 1: p.flip_WHITE_E3();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000008102000ULL);
        switch(n) {
            case 3: p.flip_WHITE_C7(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_E5();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007800000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B4(); [[fallthrough]];
            case 3: p.flip_WHITE_C4(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_E4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G4();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007C00000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B4(); [[fallthrough]];
            case 4: p.flip_WHITE_C4(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000004081000ULL);
        switch(n) {
            case 3: p.flip_WHITE_D7(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_E2(); [[fallthrough]];
            case 1: p.flip_WHITE_F3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002020200ULL);
        switch(n) {
            case 3: p.flip_WHITE_G7(); [[fallthrough]];
            case 2: p.flip_WHITE_G6(); [[fallthrough]];
            case 1: p.flip_WHITE_G5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G2(); [[fallthrough]];
            case 1: p.flip_WHITE_G3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H4(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H4();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000007E00000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B4(); [[fallthrough]];
            case 5: p.flip_WHITE_C4(); [[fallthrough]];
            case 4: p.flip_WHITE_D4(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_G4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040800ULL);
        switch(n) {
            case 3: p.flip_WHITE_E7(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_G5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0004020000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_F2(); [[fallthrough]];
            case 1: p.flip_WHITE_G3();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000001010100ULL);
        switch(n) {
            case 3: p.flip_WHITE_H7(); [[fallthrough]];
            case 2: p.flip_WHITE_H6(); [[fallthrough]];
            case 1: p.flip_WHITE_H5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010000000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_H2(); [[fallthrough]];
            case 1: p.flip_WHITE_H3();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A5();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007E000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G5(); [[fallthrough]];
            case 5: p.flip_WHITE_F5(); [[fallthrough]];
            case 4: p.flip_WHITE_E5(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_B5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0010204000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_D2(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_B4();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000402000ULL);
        switch(n) {
            case 2: p.flip_WHITE_C7(); [[fallthrough]];
            case 1: p.flip_WHITE_B6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_A2(); [[fallthrough]];
            case 2: p.flip_WHITE_A3(); [[fallthrough]];
            case 1: p.flip_WHITE_A4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000808000ULL);
        switch(n) {
            case 2: p.flip_WHITE_A7(); [[fallthrough]];
            case 1: p.flip_WHITE_A6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B5();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000003E000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G5(); [[fallthrough]];
            case 4: p.flip_WHITE_F5(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_E2(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000201000ULL);
        switch(n) {
            case 2: p.flip_WHITE_D7(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B2(); [[fallthrough]];
            case 2: p.flip_WHITE_B3(); [[fallthrough]];
            case 1: p.flip_WHITE_B4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000404000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B7(); [[fallthrough]];
            case 1: p.flip_WHITE_B6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C5();
    
    /* direction _SE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000100800ULL);
        switch(n) {
            case 2: p.flip_WHITE_E7(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000202000ULL);
        switch(n) {
            case 2: p.flip_WHITE_C7(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000001E000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G5(); [[fallthrough]];
            case 3: p.flip_WHITE_F5(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_D5();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_F2(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_D4();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202000000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_C2(); [[fallthrough]];
            case 2: p.flip_WHITE_C3(); [[fallthrough]];
            case 1: p.flip_WHITE_C4();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000004000000000ULL)
        p.flip_WHITE_B4();
    
    /* direction _SW */
    if(flipped & 0X0000000000400000ULL)
        p.flip_WHITE_B6();
    
    /* direction _W */
    if(flipped & 0X0000000040000000ULL)
        p.flip_WHITE_B5();
    
}

void RXBBPatterns::update_patterns_WHITE_D5(RXMove& move) const {}
void RXBBPatterns::update_patterns_WHITE_E5(RXMove& move) const {}

void RXBBPatterns::update_patterns_WHITE_F5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F5();
    
    /* direction _SE */
    if(flipped & 0X0000000000020000ULL)
        p.flip_WHITE_G6();
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000040400ULL);
        switch(n) {
            case 2: p.flip_WHITE_F7(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000002000000ULL)
        p.flip_WHITE_G5();
    
    /* direction NE */
    if(flipped & 0X0000000200000000ULL)
        p.flip_WHITE_G4();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040400000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_F2(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100800000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_C2(); [[fallthrough]];
            case 2: p.flip_WHITE_D3(); [[fallthrough]];
            case 1: p.flip_WHITE_E4();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000081000ULL);
        switch(n) {
            case 2: p.flip_WHITE_D7(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000078000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B5(); [[fallthrough]];
            case 3: p.flip_WHITE_C5(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_E5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G5();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007C000000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B5(); [[fallthrough]];
            case 4: p.flip_WHITE_C5(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000040800ULL);
        switch(n) {
            case 2: p.flip_WHITE_E7(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080400000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_D2(); [[fallthrough]];
            case 2: p.flip_WHITE_E3(); [[fallthrough]];
            case 1: p.flip_WHITE_F4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020200ULL);
        switch(n) {
            case 2: p.flip_WHITE_G7(); [[fallthrough]];
            case 1: p.flip_WHITE_G6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020200000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G2(); [[fallthrough]];
            case 2: p.flip_WHITE_G3(); [[fallthrough]];
            case 1: p.flip_WHITE_G4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H5(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H5();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000007E000000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B5(); [[fallthrough]];
            case 5: p.flip_WHITE_C5(); [[fallthrough]];
            case 4: p.flip_WHITE_D5(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_G5();
        }
    }
    
    
    /* direction _SW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020400ULL);
        switch(n) {
            case 2: p.flip_WHITE_F7(); [[fallthrough]];
            case 1: p.flip_WHITE_G6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0008040200000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_E2(); [[fallthrough]];
            case 2: p.flip_WHITE_F3(); [[fallthrough]];
            case 1: p.flip_WHITE_G4();
        }
    }
    
    
    /* direction S_ */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000010100ULL);
        switch(n) {
            case 2: p.flip_WHITE_H7(); [[fallthrough]];
            case 1: p.flip_WHITE_H6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010100000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_H2(); [[fallthrough]];
            case 2: p.flip_WHITE_H3(); [[fallthrough]];
            case 1: p.flip_WHITE_H4();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007E0000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G6(); [[fallthrough]];
            case 5: p.flip_WHITE_F6(); [[fallthrough]];
            case 4: p.flip_WHITE_E6(); [[fallthrough]];
            case 3: p.flip_WHITE_D6(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_B6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0008102040000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_E2(); [[fallthrough]];
            case 3: p.flip_WHITE_D3(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_B5();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000000000004000ULL)
        p.flip_WHITE_B7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_A2(); [[fallthrough]];
            case 3: p.flip_WHITE_A3(); [[fallthrough]];
            case 2: p.flip_WHITE_A4(); [[fallthrough]];
            case 1: p.flip_WHITE_A5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000008000ULL)
        p.flip_WHITE_A7();
    
}

void RXBBPatterns::update_patterns_WHITE_B6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000003E0000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G6(); [[fallthrough]];
            case 4: p.flip_WHITE_F6(); [[fallthrough]];
            case 3: p.flip_WHITE_E6(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_F2(); [[fallthrough]];
            case 3: p.flip_WHITE_E3(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction _SE */
    if(flipped & 0X0000000000002000ULL)
        p.flip_WHITE_C7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B2(); [[fallthrough]];
            case 3: p.flip_WHITE_B3(); [[fallthrough]];
            case 2: p.flip_WHITE_B4(); [[fallthrough]];
            case 1: p.flip_WHITE_B5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000004000ULL)
        p.flip_WHITE_B7();
    
}

void RXBBPatterns::update_patterns_WHITE_C6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C6();
    
    /* direction _SE */
    if(flipped & 0X0000000000001000ULL)
        p.flip_WHITE_D7();
    
    /* direction S_ */
    if(flipped & 0X0000000000002000ULL)
        p.flip_WHITE_C7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000001E0000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G6(); [[fallthrough]];
            case 3: p.flip_WHITE_F6(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G2(); [[fallthrough]];
            case 3: p.flip_WHITE_F3(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_D5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_C2(); [[fallthrough]];
            case 3: p.flip_WHITE_C3(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000040000000ULL)
        p.flip_WHITE_B5();
    
    /* direction _SW */
    if(flipped & 0X0000000000004000ULL)
        p.flip_WHITE_B7();
    
    /* direction _W */
    if(flipped & 0X0000000000400000ULL)
        p.flip_WHITE_B6();
    
}

void RXBBPatterns::update_patterns_WHITE_D6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000800ULL)
        p.flip_WHITE_E7();
    
    /* direction S_ */
    if(flipped & 0X0000000000001000ULL)
        p.flip_WHITE_D7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000000E0000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G6(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G3(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_E5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_D2(); [[fallthrough]];
            case 3: p.flip_WHITE_D3(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_D5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B4(); [[fallthrough]];
            case 1: p.flip_WHITE_C5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000002000ULL)
        p.flip_WHITE_C7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000600000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B6(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000400ULL)
        p.flip_WHITE_F7();
    
    /* direction S_ */
    if(flipped & 0X0000000000000800ULL)
        p.flip_WHITE_E7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000060000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G6(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204000000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G4(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_E2(); [[fallthrough]];
            case 3: p.flip_WHITE_E3(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_E5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010000000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B3(); [[fallthrough]];
            case 2: p.flip_WHITE_C4(); [[fallthrough]];
            case 1: p.flip_WHITE_D5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000001000ULL)
        p.flip_WHITE_D7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000700000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B6(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F6();
    
    /* direction _SE */
    if(flipped & 0X0000000000000200ULL)
        p.flip_WHITE_G7();
    
    /* direction S_ */
    if(flipped & 0X0000000000000400ULL)
        p.flip_WHITE_F7();
    
    /* direction _E */
    if(flipped & 0X0000000000020000ULL)
        p.flip_WHITE_G6();
    
    /* direction NE */
    if(flipped & 0X0000000002000000ULL)
        p.flip_WHITE_G5();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_F2(); [[fallthrough]];
            case 3: p.flip_WHITE_F3(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B2(); [[fallthrough]];
            case 3: p.flip_WHITE_C3(); [[fallthrough]];
            case 2: p.flip_WHITE_D4(); [[fallthrough]];
            case 1: p.flip_WHITE_E5();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000800ULL)
        p.flip_WHITE_E7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000780000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B6(); [[fallthrough]];
            case 3: p.flip_WHITE_C6(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G6();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007C0000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B6(); [[fallthrough]];
            case 4: p.flip_WHITE_C6(); [[fallthrough]];
            case 3: p.flip_WHITE_D6(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000400ULL)
        p.flip_WHITE_F7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_C2(); [[fallthrough]];
            case 3: p.flip_WHITE_D3(); [[fallthrough]];
            case 2: p.flip_WHITE_E4(); [[fallthrough]];
            case 1: p.flip_WHITE_F5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000000200ULL)
        p.flip_WHITE_G7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G2(); [[fallthrough]];
            case 3: p.flip_WHITE_G3(); [[fallthrough]];
            case 2: p.flip_WHITE_G4(); [[fallthrough]];
            case 1: p.flip_WHITE_G5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H6(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H6();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X00000000007E0000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B6(); [[fallthrough]];
            case 5: p.flip_WHITE_C6(); [[fallthrough]];
            case 4: p.flip_WHITE_D6(); [[fallthrough]];
            case 3: p.flip_WHITE_E6(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_G6();
        }
    }
    
    
    /* direction _SW */
    if(flipped & 0X0000000000000200ULL)
        p.flip_WHITE_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0010080402000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_D2(); [[fallthrough]];
            case 3: p.flip_WHITE_E3(); [[fallthrough]];
            case 2: p.flip_WHITE_F4(); [[fallthrough]];
            case 1: p.flip_WHITE_G5();
        }
    }
    
    
    /* direction S_ */
    if(flipped & 0X0000000000000100ULL)
        p.flip_WHITE_H7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101000000ULL);
        switch(n) {
            case 4: p.flip_WHITE_H2(); [[fallthrough]];
            case 3: p.flip_WHITE_H3(); [[fallthrough]];
            case 2: p.flip_WHITE_H4(); [[fallthrough]];
            case 1: p.flip_WHITE_H5();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A7();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0004081020400000ULL);
        switch(n) {
            case 5: p.flip_WHITE_F2(); [[fallthrough]];
            case 4: p.flip_WHITE_E3(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_B6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080800000ULL);
        switch(n) {
            case 5: p.flip_WHITE_A2(); [[fallthrough]];
            case 4: p.flip_WHITE_A3(); [[fallthrough]];
            case 3: p.flip_WHITE_A4(); [[fallthrough]];
            case 2: p.flip_WHITE_A5(); [[fallthrough]];
            case 1: p.flip_WHITE_A6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007E00ULL);
        switch(n) {
            case 6: p.flip_WHITE_G7(); [[fallthrough]];
            case 5: p.flip_WHITE_F7(); [[fallthrough]];
            case 4: p.flip_WHITE_E7(); [[fallthrough]];
            case 3: p.flip_WHITE_D7(); [[fallthrough]];
            case 2: p.flip_WHITE_C7(); [[fallthrough]];
            case 1: p.flip_WHITE_B7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B7();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810200000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G2(); [[fallthrough]];
            case 4: p.flip_WHITE_F3(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040400000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B2(); [[fallthrough]];
            case 4: p.flip_WHITE_B3(); [[fallthrough]];
            case 3: p.flip_WHITE_B4(); [[fallthrough]];
            case 2: p.flip_WHITE_B5(); [[fallthrough]];
            case 1: p.flip_WHITE_B6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000003E00ULL);
        switch(n) {
            case 5: p.flip_WHITE_G7(); [[fallthrough]];
            case 4: p.flip_WHITE_F7(); [[fallthrough]];
            case 3: p.flip_WHITE_E7(); [[fallthrough]];
            case 2: p.flip_WHITE_D7(); [[fallthrough]];
            case 1: p.flip_WHITE_C7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020200000ULL);
        switch(n) {
            case 5: p.flip_WHITE_C2(); [[fallthrough]];
            case 4: p.flip_WHITE_C3(); [[fallthrough]];
            case 3: p.flip_WHITE_C4(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408100000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G3(); [[fallthrough]];
            case 3: p.flip_WHITE_F4(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000000400000ULL)
        p.flip_WHITE_B6();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000001E00ULL);
        switch(n) {
            case 4: p.flip_WHITE_G7(); [[fallthrough]];
            case 3: p.flip_WHITE_F7(); [[fallthrough]];
            case 2: p.flip_WHITE_E7(); [[fallthrough]];
            case 1: p.flip_WHITE_D7();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0000000000004000ULL)
        p.flip_WHITE_B7();
    
}

void RXBBPatterns::update_patterns_WHITE_D7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010100000ULL);
        switch(n) {
            case 5: p.flip_WHITE_D2(); [[fallthrough]];
            case 4: p.flip_WHITE_D3(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204080000ULL);
        switch(n) {
            case 3: p.flip_WHITE_G4(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040200000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B5(); [[fallthrough]];
            case 1: p.flip_WHITE_C6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000E00ULL);
        switch(n) {
            case 3: p.flip_WHITE_G7(); [[fallthrough]];
            case 2: p.flip_WHITE_F7(); [[fallthrough]];
            case 1: p.flip_WHITE_E7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000006000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B7(); [[fallthrough]];
            case 1: p.flip_WHITE_C7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080000ULL);
        switch(n) {
            case 5: p.flip_WHITE_E2(); [[fallthrough]];
            case 4: p.flip_WHITE_E3(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040000ULL);
        switch(n) {
            case 2: p.flip_WHITE_G5(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B4(); [[fallthrough]];
            case 2: p.flip_WHITE_C5(); [[fallthrough]];
            case 1: p.flip_WHITE_D6();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000600ULL);
        switch(n) {
            case 2: p.flip_WHITE_G7(); [[fallthrough]];
            case 1: p.flip_WHITE_F7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B7(); [[fallthrough]];
            case 2: p.flip_WHITE_C7(); [[fallthrough]];
            case 1: p.flip_WHITE_D7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F7();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040000ULL);
        switch(n) {
            case 5: p.flip_WHITE_F2(); [[fallthrough]];
            case 4: p.flip_WHITE_F3(); [[fallthrough]];
            case 3: p.flip_WHITE_F4(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0000000000020000ULL)
        p.flip_WHITE_G6();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080000ULL);
        switch(n) {
            case 4: p.flip_WHITE_B3(); [[fallthrough]];
            case 3: p.flip_WHITE_C4(); [[fallthrough]];
            case 2: p.flip_WHITE_D5(); [[fallthrough]];
            case 1: p.flip_WHITE_E6();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000000000200ULL)
        p.flip_WHITE_G7();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007800ULL);
        switch(n) {
            case 4: p.flip_WHITE_B7(); [[fallthrough]];
            case 3: p.flip_WHITE_C7(); [[fallthrough]];
            case 2: p.flip_WHITE_D7(); [[fallthrough]];
            case 1: p.flip_WHITE_E7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040000ULL);
        switch(n) {
            case 5: p.flip_WHITE_B2(); [[fallthrough]];
            case 4: p.flip_WHITE_C3(); [[fallthrough]];
            case 3: p.flip_WHITE_D4(); [[fallthrough]];
            case 2: p.flip_WHITE_E5(); [[fallthrough]];
            case 1: p.flip_WHITE_F6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G2(); [[fallthrough]];
            case 4: p.flip_WHITE_G3(); [[fallthrough]];
            case 3: p.flip_WHITE_G4(); [[fallthrough]];
            case 2: p.flip_WHITE_G5(); [[fallthrough]];
            case 1: p.flip_WHITE_G6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007C00ULL);
        switch(n) {
            case 5: p.flip_WHITE_B7(); [[fallthrough]];
            case 4: p.flip_WHITE_C7(); [[fallthrough]];
            case 3: p.flip_WHITE_D7(); [[fallthrough]];
            case 2: p.flip_WHITE_E7(); [[fallthrough]];
            case 1: p.flip_WHITE_F7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H7(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0020100804020000ULL);
        switch(n) {
            case 5: p.flip_WHITE_C2(); [[fallthrough]];
            case 4: p.flip_WHITE_D3(); [[fallthrough]];
            case 3: p.flip_WHITE_E4(); [[fallthrough]];
            case 2: p.flip_WHITE_F5(); [[fallthrough]];
            case 1: p.flip_WHITE_G6();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010000ULL);
        switch(n) {
            case 5: p.flip_WHITE_H2(); [[fallthrough]];
            case 4: p.flip_WHITE_H3(); [[fallthrough]];
            case 3: p.flip_WHITE_H4(); [[fallthrough]];
            case 2: p.flip_WHITE_H5(); [[fallthrough]];
            case 1: p.flip_WHITE_H6();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000007E00ULL);
        switch(n) {
            case 6: p.flip_WHITE_B7(); [[fallthrough]];
            case 5: p.flip_WHITE_C7(); [[fallthrough]];
            case 4: p.flip_WHITE_D7(); [[fallthrough]];
            case 3: p.flip_WHITE_E7(); [[fallthrough]];
            case 2: p.flip_WHITE_F7(); [[fallthrough]];
            case 1: p.flip_WHITE_G7();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_A8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_A8();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0002040810204000ULL);
        switch(n) {
            case 6: p.flip_WHITE_G2(); [[fallthrough]];
            case 5: p.flip_WHITE_F3(); [[fallthrough]];
            case 4: p.flip_WHITE_E4(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_B7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0080808080808000ULL);
        switch(n) {
            case 6: p.flip_WHITE_A2(); [[fallthrough]];
            case 5: p.flip_WHITE_A3(); [[fallthrough]];
            case 4: p.flip_WHITE_A4(); [[fallthrough]];
            case 3: p.flip_WHITE_A5(); [[fallthrough]];
            case 2: p.flip_WHITE_A6(); [[fallthrough]];
            case 1: p.flip_WHITE_A7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007EULL);
        switch(n) {
            case 6: p.flip_WHITE_G8(); [[fallthrough]];
            case 5: p.flip_WHITE_F8(); [[fallthrough]];
            case 4: p.flip_WHITE_E8(); [[fallthrough]];
            case 3: p.flip_WHITE_D8(); [[fallthrough]];
            case 2: p.flip_WHITE_C8(); [[fallthrough]];
            case 1: p.flip_WHITE_B8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_B8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_B8();
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000020408102000ULL);
        switch(n) {
            case 5: p.flip_WHITE_G3(); [[fallthrough]];
            case 4: p.flip_WHITE_F4(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_C7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0040404040404000ULL);
        switch(n) {
            case 6: p.flip_WHITE_B2(); [[fallthrough]];
            case 5: p.flip_WHITE_B3(); [[fallthrough]];
            case 4: p.flip_WHITE_B4(); [[fallthrough]];
            case 3: p.flip_WHITE_B5(); [[fallthrough]];
            case 2: p.flip_WHITE_B6(); [[fallthrough]];
            case 1: p.flip_WHITE_B7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000003EULL);
        switch(n) {
            case 5: p.flip_WHITE_G8(); [[fallthrough]];
            case 4: p.flip_WHITE_F8(); [[fallthrough]];
            case 3: p.flip_WHITE_E8(); [[fallthrough]];
            case 2: p.flip_WHITE_D8(); [[fallthrough]];
            case 1: p.flip_WHITE_C8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_C8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_C8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0020202020202000ULL);
        switch(n) {
            case 6: p.flip_WHITE_C2(); [[fallthrough]];
            case 5: p.flip_WHITE_C3(); [[fallthrough]];
            case 4: p.flip_WHITE_C4(); [[fallthrough]];
            case 3: p.flip_WHITE_C5(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_C7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000204081000ULL);
        switch(n) {
            case 4: p.flip_WHITE_G4(); [[fallthrough]];
            case 3: p.flip_WHITE_F5(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_D7();
        }
    }
    
    
    /* direction NW */
    if(flipped & 0X0000000000004000ULL)
        p.flip_WHITE_B7();
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000001EULL);
        switch(n) {
            case 4: p.flip_WHITE_G8(); [[fallthrough]];
            case 3: p.flip_WHITE_F8(); [[fallthrough]];
            case 2: p.flip_WHITE_E8(); [[fallthrough]];
            case 1: p.flip_WHITE_D8();
        }
    }
    
    
    /* direction _W */
    if(flipped & 0X0000000000000040ULL)
        p.flip_WHITE_B8();
    
}

void RXBBPatterns::update_patterns_WHITE_D8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_D8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0010101010101000ULL);
        switch(n) {
            case 6: p.flip_WHITE_D2(); [[fallthrough]];
            case 5: p.flip_WHITE_D3(); [[fallthrough]];
            case 4: p.flip_WHITE_D4(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_D7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000002040800ULL);
        switch(n) {
            case 3: p.flip_WHITE_G5(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_E7();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000402000ULL);
        switch(n) {
            case 2: p.flip_WHITE_B6(); [[fallthrough]];
            case 1: p.flip_WHITE_C7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000000EULL);
        switch(n) {
            case 3: p.flip_WHITE_G8(); [[fallthrough]];
            case 2: p.flip_WHITE_F8(); [[fallthrough]];
            case 1: p.flip_WHITE_E8();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000060ULL);
        switch(n) {
            case 2: p.flip_WHITE_B8(); [[fallthrough]];
            case 1: p.flip_WHITE_C8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_E8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_E8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0008080808080800ULL);
        switch(n) {
            case 6: p.flip_WHITE_E2(); [[fallthrough]];
            case 5: p.flip_WHITE_E3(); [[fallthrough]];
            case 4: p.flip_WHITE_E4(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_E7();
        }
    }
    
    
    /* direction NE */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000020400ULL);
        switch(n) {
            case 2: p.flip_WHITE_G6(); [[fallthrough]];
            case 1: p.flip_WHITE_F7();
        }
    }
    
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000040201000ULL);
        switch(n) {
            case 3: p.flip_WHITE_B5(); [[fallthrough]];
            case 2: p.flip_WHITE_C6(); [[fallthrough]];
            case 1: p.flip_WHITE_D7();
        }
    }
    
    
    /* direction _E */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000006ULL);
        switch(n) {
            case 2: p.flip_WHITE_G8(); [[fallthrough]];
            case 1: p.flip_WHITE_F8();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000070ULL);
        switch(n) {
            case 3: p.flip_WHITE_B8(); [[fallthrough]];
            case 2: p.flip_WHITE_C8(); [[fallthrough]];
            case 1: p.flip_WHITE_D8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_F8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_F8();
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0004040404040400ULL);
        switch(n) {
            case 6: p.flip_WHITE_F2(); [[fallthrough]];
            case 5: p.flip_WHITE_F3(); [[fallthrough]];
            case 4: p.flip_WHITE_F4(); [[fallthrough]];
            case 3: p.flip_WHITE_F5(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_F7();
        }
    }
    
    
    /* direction NE */
    if(flipped & 0X0000000000000200ULL)
        p.flip_WHITE_G7();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000004020100800ULL);
        switch(n) {
            case 4: p.flip_WHITE_B4(); [[fallthrough]];
            case 3: p.flip_WHITE_C5(); [[fallthrough]];
            case 2: p.flip_WHITE_D6(); [[fallthrough]];
            case 1: p.flip_WHITE_E7();
        }
    }
    
    
    /* direction _E */
    if(flipped & 0X0000000000000002ULL)
        p.flip_WHITE_G8();
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X0000000000000078ULL);
        switch(n) {
            case 4: p.flip_WHITE_B8(); [[fallthrough]];
            case 3: p.flip_WHITE_C8(); [[fallthrough]];
            case 2: p.flip_WHITE_D8(); [[fallthrough]];
            case 1: p.flip_WHITE_E8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_G8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_G8();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0000402010080400ULL);
        switch(n) {
            case 5: p.flip_WHITE_B3(); [[fallthrough]];
            case 4: p.flip_WHITE_C4(); [[fallthrough]];
            case 3: p.flip_WHITE_D5(); [[fallthrough]];
            case 2: p.flip_WHITE_E6(); [[fallthrough]];
            case 1: p.flip_WHITE_F7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0002020202020200ULL);
        switch(n) {
            case 6: p.flip_WHITE_G2(); [[fallthrough]];
            case 5: p.flip_WHITE_G3(); [[fallthrough]];
            case 4: p.flip_WHITE_G4(); [[fallthrough]];
            case 3: p.flip_WHITE_G5(); [[fallthrough]];
            case 2: p.flip_WHITE_G6(); [[fallthrough]];
            case 1: p.flip_WHITE_G7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007CULL);
        switch(n) {
            case 5: p.flip_WHITE_B8(); [[fallthrough]];
            case 4: p.flip_WHITE_C8(); [[fallthrough]];
            case 3: p.flip_WHITE_D8(); [[fallthrough]];
            case 2: p.flip_WHITE_E8(); [[fallthrough]];
            case 1: p.flip_WHITE_F8();
        }
    }
    
    
}

void RXBBPatterns::update_patterns_WHITE_H8(RXMove& move) const {
    
    const unsigned long long flipped = move.flipped;
    RXPattern& p = *(move.pattern);
    p = *pattern; //copy
    
    p.set_WHITE_H8();
    
    /* direction NW */
    {
        const int n = __builtin_popcountll(flipped & 0X0040201008040200ULL);
        switch(n) {
            case 6: p.flip_WHITE_B2(); [[fallthrough]];
            case 5: p.flip_WHITE_C3(); [[fallthrough]];
            case 4: p.flip_WHITE_D4(); [[fallthrough]];
            case 3: p.flip_WHITE_E5(); [[fallthrough]];
            case 2: p.flip_WHITE_F6(); [[fallthrough]];
            case 1: p.flip_WHITE_G7();
        }
    }
    
    
    /* direction _N */
    {
        const int n = __builtin_popcountll(flipped & 0X0001010101010100ULL);
        switch(n) {
            case 6: p.flip_WHITE_H2(); [[fallthrough]];
            case 5: p.flip_WHITE_H3(); [[fallthrough]];
            case 4: p.flip_WHITE_H4(); [[fallthrough]];
            case 3: p.flip_WHITE_H5(); [[fallthrough]];
            case 2: p.flip_WHITE_H6(); [[fallthrough]];
            case 1: p.flip_WHITE_H7();
        }
    }
    
    
    /* direction _W */
    {
        const int n = __builtin_popcountll(flipped & 0X000000000000007EULL);
        switch(n) {
            case 6: p.flip_WHITE_B8(); [[fallthrough]];
            case 5: p.flip_WHITE_C8(); [[fallthrough]];
            case 4: p.flip_WHITE_D8(); [[fallthrough]];
            case 3: p.flip_WHITE_E8(); [[fallthrough]];
            case 2: p.flip_WHITE_F8(); [[fallthrough]];
            case 1: p.flip_WHITE_G8();
        }
    }
    
    
}


// Ordre : [pos][0] = BLACK, [pos][1] = WHITE
RXBBPatterns::type_update_patterns const RXBBPatterns::update_patterns_NEON[][2] = {
#define func(pos) { &RXBBPatterns::update_patterns_BLACK_##pos, &RXBBPatterns::update_patterns_WHITE_##pos }
    func(H8), func(G8), func(F8), func(E8), func(D8), func(C8), func(B8), func(A8),
    func(H7), func(G7), func(F7), func(E7), func(D7), func(C7), func(B7), func(A7),
    func(H6), func(G6), func(F6), func(E6), func(D6), func(C6), func(B6), func(A6),
    func(H5), func(G5), func(F5), func(E5), func(D5), func(C5), func(B5), func(A5),
    func(H4), func(G4), func(F4), func(E4), func(D4), func(C4), func(B4), func(A4),
    func(H3), func(G3), func(F3), func(E3), func(D3), func(C3), func(B3), func(A3),
    func(H2), func(G2), func(F2), func(E2), func(D2), func(C2), func(B2), func(A2),
    func(H1), func(G1), func(F1), func(E1), func(D1), func(C1), func(B1), func(A1),
#undef func
};

