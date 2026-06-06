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

#include "RXSetting.hpp"

#include <x86intrin.h>

// forward declaration - définie dans RXBBDoFlips_AVX2.cpp
__m128i mm_flip(const __m256i PP, const __m256i OO, int pos);


/*
 @brief count all legal moves
 
 @param P                    a bitboard representing player
 @param O                    a bitboard representing opponent
 @return count all legal moves
 */
inline unsigned int RXBitBoard::count_legal_moves(const unsigned long long P, const unsigned long long O) {
    
    const unsigned long long legals = get_legal_moves(P, O);
    return __builtin_popcountll(legals);
    
}


/// retourne un pseudo (sous evalué) score de pions stables
/// - Parameters:
///   - color: couleur du joueur
///   - n_stables_cut: valeur de coupure (type alpha, beta)
inline int RXBitBoard::get_stability(const unsigned long long discs_player, const unsigned long long discs_opponent) {
    
    
    const unsigned long long filled = discs_player | discs_opponent;
    const unsigned long long central_mask = discs_player & 0x007e7e7e7e7e7e00ULL;
    
    unsigned long long stable = get_stable_edge(discs_player, discs_opponent);

    unsigned long long h, v, d7, d9;
    
//    h = filled;
//    h &= h >> 4;
//    h &= h >> 2;
//    h &= h >> 1;
//    h &= 0x0101010101010101ULL;
//    
//    //trick multiplication par 255 (remplit les lignes)
//    h = (h << 8) - h; //*=255
//    h |= 0x8181818181818181ULL;
//    
//    v = filled;
//    v &= (v >> 32) | (v << 32);
//    v &= (v >> 16) | (v << 16);
//    v &= (v >>  8) | (v <<  8);
//    
//    v |= 0xFF000000000000FFULL;
//    
//    d7 = filled;
//    d7 &= ((d7>>28) & 0x00000000F0F0F0F0ULL) | ((d7<<28) & 0x0F0F0F0F00000000ULL) | 0xF0F0F0F00F0F0F0FULL;
//    d7 &= ((d7>>14) & 0x0000FCFCFCFCFCFCULL) | ((d7<<14) & 0x3F3F3F3F3F3F0000ULL) | 0xC0C0000000000303ULL;
//    d7 &= (d7>> 7) & (d7<< 7);
//    
//    d7 |= 0xFF818181818181FFULL;
//    
//    d9 = filled;
//    //d9 &= ((d9>>36) & 0x000000000F0F0F0FULL) | ((d9<<36) & 0xF0F0F0F000000000ULL) | 0x0F0F0F0FF0F0F0F0ULL;
//    d9 &= (d9>>36) | (d9<<36) | 0x0F0F0F0FF0F0F0F0ULL;
//    d9 &= ((d9>>18) & 0x00003F3F3F3F3F3FULL) | ((d9<<18) & 0xFCFCFCFCFCFC0000ULL) | 0x030300000000C0C0ULL;
//    d9 &= (d9>> 9) & (d9<< 9);
//    
//    d9 |= 0xFF818181818181FFULL;
    
    uint64_t rdisc = __builtin_bswap64(filled);
    uint64_t l8;
    __m128i l01, l79, r79;
    const __m128i kff  = _mm_set1_epi8(-1);
    const __m128i e790 = _mm_set1_epi64x(0xff80808080808080);
    const __m128i e791 = _mm_set1_epi64x(0x01010101010101ff);
    const __m128i e792 = _mm_set1_epi64x(0x00003f3f3f3f3f3f);
    const __m128i e793 = _mm_set1_epi64x(0x0f0f0f0ff0f0f0f0);

    l01 = l79 = _mm_cvtsi64_si128(filled);    l79 = r79 = _mm_unpacklo_epi64(l79, _mm_cvtsi64_si128(rdisc));
    l01 = _mm_cmpeq_epi8(kff, l01);         l79 = _mm_and_si128(l79, _mm_or_si128(e790, _mm_srli_epi64(l79, 9)));
    h = _mm_cvtsi128_si64(l01);
    r79 = _mm_and_si128(r79, _mm_or_si128(e791, _mm_slli_epi64(r79, 9)));
    l8 = filled;                              l79 = _mm_andnot_si128(_mm_andnot_si128(_mm_srli_epi64(l79, 18), e792), l79);
    l8 &= (l8 >> 8) | (l8 << 56);           r79 = _mm_andnot_si128(_mm_slli_epi64(_mm_andnot_si128(r79, e792), 18), r79);
    l8 &= (l8 >> 16) | (l8 << 48);          l79 = _mm_and_si128(_mm_and_si128(l79, r79), _mm_or_si128(e793, _mm_or_si128(_mm_srli_epi64(l79, 36), _mm_slli_epi64(r79, 36))));
    l8 &= (l8 >> 32) | (l8 << 32);          d9 = _mm_cvtsi128_si64(l79);
    v = l8;                                 d7 = __builtin_bswap64(_mm_cvtsi128_si64(_mm_unpackhi_epi64(l79, l79)));
        
    stable |= (h & v & d7 & d9 & central_mask);
    
    unsigned long long old_stable = 0;
    unsigned long long stable_h, stable_v, stable_d7, stable_d9;
    
    while (stable != old_stable) {
        old_stable = stable;
        stable_h  = ((stable >> 1) | (stable << 1) | h);
        stable_v  = ((stable >> 8) | (stable << 8) | v);
        stable_d7 = ((stable >> 7) | (stable << 7) | d7);
        stable_d9 = ((stable >> 9) | (stable << 9) | d9);
        stable |= (stable_h & stable_v & stable_d7 & stable_d9 & central_mask);
    }
    
    return __builtin_popcountll(stable);

    
}



inline unsigned long long RXBitBoard::get_legal_moves(const unsigned long long P, const unsigned long long O)
{


    __m256i PP = _mm256_set1_epi64x(P);
    __m256i OO = _mm256_set1_epi64x(O);
    
    __m256i    MM, flip_l, flip_r, pre_l, pre_r;
    __m128i    M;
    const __m256i dir1 = _mm256_set_epi64x(7, 9, 8, 1);
    const __m256i dir2 = _mm256_add_epi64(dir1, dir1);
    const __m256i mask = _mm256_and_si256(OO, _mm256_set_epi64x(0x007E7E7E7E7E7E00, 0x007E7E7E7E7E7E00, 0x00FFFFFFFFFFFF00, 0x7E7E7E7E7E7E7E7E));
    const __m128i occupied = _mm_or_si128(_mm256_castsi256_si128(PP), _mm256_castsi256_si128(OO));

    flip_l = _mm256_and_si256(mask, _mm256_sllv_epi64(PP, dir1));
    flip_r = _mm256_and_si256(mask, _mm256_srlv_epi64(PP, dir1));
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(mask, _mm256_sllv_epi64(flip_l, dir1)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(mask, _mm256_srlv_epi64(flip_r, dir1)));
    pre_l = _mm256_and_si256(mask, _mm256_sllv_epi64(mask, dir1));
    pre_r = _mm256_srlv_epi64(pre_l, dir1);
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, dir2)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, dir2)));
    flip_l = _mm256_or_si256(flip_l, _mm256_and_si256(pre_l, _mm256_sllv_epi64(flip_l, dir2)));
    flip_r = _mm256_or_si256(flip_r, _mm256_and_si256(pre_r, _mm256_srlv_epi64(flip_r, dir2)));
    MM = _mm256_or_si256(_mm256_sllv_epi64(flip_l, dir1), _mm256_srlv_epi64(flip_r, dir1));

    M = _mm_or_si128(_mm256_castsi256_si128(MM), _mm256_extracti128_si256(MM, 1));
    return _mm_cvtsi128_si64(_mm_andnot_si128(occupied, _mm_or_si128(M, _mm_unpackhi_epi64(M, M))));    // mask with empties
}



inline void RXBitBoard::dual_count_legal_moves(unsigned int& mob_P, unsigned int& mob_O) const {
    return RXBitBoard::dual_count_legal_moves(discs[player], discs[player^1], mob_P, mob_O);
}

inline void RXBitBoard::dual_count_legal_moves(const unsigned long long discs_player, const unsigned long long discs_opponent, unsigned int& mob_P, unsigned int& mob_O) {
    mob_P = RXBitBoard::count_legal_moves(discs_player, discs_opponent);
    mob_O = RXBitBoard::count_legal_moves(discs_opponent, discs_player);
}


