//
//  RXBitBoard_x86.hpp
//  Roxane
//
//  Created by Causse Bruno on 13/02/2026.
//

#include "RXSetting.hpp"

#if ARCH == ARCH_X86_AVX2
    #include <x86intrin.h>

    // forward declaration - définie dans RXBBDoFlips_AVX2.cpp
    __m128i mm_flip(const __m128i OP, int pos);

#endif

/*
 @brief count all legal moves
 
 @param P                    a bitboard representing player
 @param O                    a bitboard representing opponent
 @return count all legal moves
 */
inline int RXBitBoard::count_legal_moves(const unsigned long long p_discs, const unsigned long long o_discs) {
    
    const unsigned long long legals = get_legal_moves(p_discs, o_discs);
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
    
    h = filled;
    h &= h >> 4;
    h &= h >> 2;
    h &= h >> 1;
    h &= 0x0101010101010101ULL;
    
    //trick multiplication par 255 (remplit les lignes)
    h = (h << 8) - h; //*=255
    h |= 0x8181818181818181ULL;
    
    v = filled;
    v &= (v >> 32) | (v << 32);
    v &= (v >> 16) | (v << 16);
    v &= (v >>  8) | (v <<  8);
    
    v |= 0xFF000000000000FFULL;
    
    d7 = filled;
    d7 &= ((d7>>28) & 0x00000000F0F0F0F0ULL) | ((d7<<28) & 0x0F0F0F0F00000000ULL) | 0xF0F0F0F00F0F0F0FULL;
    d7 &= ((d7>>14) & 0x0000FCFCFCFCFCFCULL) | ((d7<<14) & 0x3F3F3F3F3F3F0000ULL) | 0xC0C0000000000303ULL;
    d7 &= (d7>> 7) & (d7<< 7);
    
    d7 |= 0xFF818181818181FFULL;
    
    d9 = filled;
    //d9 &= ((d9>>36) & 0x000000000F0F0F0FULL) | ((d9<<36) & 0xF0F0F0F000000000ULL) | 0x0F0F0F0FF0F0F0F0ULL;
    d9 &= (d9>>36) | (d9<<36) | 0x0F0F0F0FF0F0F0F0ULL;
    d9 &= ((d9>>18) & 0x00003F3F3F3F3F3FULL) | ((d9<<18) & 0xFCFCFCFCFCFC0000ULL) | 0x030300000000C0C0ULL;
    d9 &= (d9>> 9) & (d9<< 9);
    
    d9 |= 0xFF818181818181FFULL;
        
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




/*
 @brief Get a bitboard representing all legal moves
 
 @param P                    a bitboard representing player
 @param O                    a bitboard representing opponent
 @return all legal moves as a bitboard
 */
// original code from http://www.amy.hi-ho.ne.jp/okuhara/bitboard.htm
// modified by Nyanyan
// version EDAX version identique a Roxane (presentation pour la vectorisation)
//inline uint64_t RXBitBoard::calc_legal(const uint64_t P, const uint64_t O){
//    uint64_t moves, mO;
//    uint64_t flip1, flip7, flip9, flip8, pre1, pre7, pre9, pre8;
//    mO = O & 0x7e7e7e7e7e7e7e7eULL;
//    flip1 = mO & (P << 1);         flip7  = mO & (P << 7);        flip9  = mO & (P << 9);        flip8  = O & (P << 8);
//    flip1 |= mO & (flip1 << 1);    flip7 |= mO & (flip7 << 7);    flip9 |= mO & (flip9 << 9);    flip8 |= O & (flip8 << 8);
//    pre1 = mO & (mO << 1);         pre7 = mO & (mO << 7);         pre9 = mO & (mO << 9);         pre8 = O & (O << 8);
//    flip1 |= pre1 & (flip1 << 2);  flip7 |= pre7 & (flip7 << 14); flip9 |= pre9 & (flip9 << 18); flip8 |= pre8 & (flip8 << 16);
//    flip1 |= pre1 & (flip1 << 2);  flip7 |= pre7 & (flip7 << 14); flip9 |= pre9 & (flip9 << 18); flip8 |= pre8 & (flip8 << 16);
//    moves = flip1 << 1;            moves |= flip7 << 7;           moves |= flip9 << 9;           moves |= flip8 << 8;
//    flip1 = mO & (P >> 1);         flip7  = mO & (P >> 7);        flip9  = mO & (P >> 9);        flip8  = O & (P >> 8);
//    flip1 |= mO & (flip1 >> 1);    flip7 |= mO & (flip7 >> 7);    flip9 |= mO & (flip9 >> 9);    flip8 |= O & (flip8 >> 8);
//    pre1 >>= 1;                    pre7 >>= 7;                    pre9 >>= 9;                    pre8 >>= 8;
//    flip1 |= pre1 & (flip1 >> 2);  flip7 |= pre7 & (flip7 >> 14); flip9 |= pre9 & (flip9 >> 18); flip8 |= pre8 & (flip8 >> 16);
//    flip1 |= pre1 & (flip1 >> 2);  flip7 |= pre7 & (flip7 >> 14); flip9 |= pre9 & (flip9 >> 18); flip8 |= pre8 & (flip8 >> 16);
//    moves |= flip1 >> 1;           moves |= flip7 >> 7;           moves |= flip9 >> 9;           moves |= flip8 >> 8;
//    return moves & ~(P | O);
//}

#if ARCH == ARCH_X86_NEON

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

#else

inline unsigned long long RXBitBoard::get_legal_moves(const unsigned long long p_discs, const unsigned long long o_discs) {
    
    
    const unsigned long long inner_o_discs = o_discs & 0x7E7E7E7E7E7E7E7EULL;
    
    
    /* direction W */
    unsigned long long
    flipped  = (p_discs >> 1) & inner_o_discs;
    flipped |= (flipped >> 1) & inner_o_discs;
    
    unsigned long long adjacent_o_discs = inner_o_discs & (inner_o_discs >> 1);
    
    flipped |= (flipped >> 2) & adjacent_o_discs;
    flipped |= (flipped >> 2) & adjacent_o_discs;
    
    unsigned long long legals = flipped >> 1;
    
    
    //    /* direction _E*/
    //    flipped  = (p_discs << 1) & inner_o_discs;
    //    flipped |= (flipped << 1) & inner_o_discs;
    //
    //    adjacent_o_discs = inner_o_discs & (inner_o_discs << 1);
    //
    //    flipped |= (flipped << 2) & adjacent_o_discs;
    //    flipped |= (flipped << 2) & adjacent_o_discs;
    //
    //    legals |= flipped << 1;
    
    // trick
    /* direction _E */
    flipped = (p_discs << 1);
    legals |= ((flipped + inner_o_discs) & ~flipped);
    
    
    /* direction S */
    flipped  = (p_discs >>  8) & o_discs;
    flipped |= (flipped >>  8) & o_discs;
    
    adjacent_o_discs = o_discs & (o_discs >> 8);
    
    flipped |= (flipped >> 16) & adjacent_o_discs;
    flipped |= (flipped >> 16) & adjacent_o_discs;
    
    legals |= flipped >> 8;
    
    
    /* direction N */
    flipped  = (p_discs <<  8) & o_discs;
    flipped |= (flipped <<  8) & o_discs;
    
    adjacent_o_discs = o_discs & (o_discs << 8);
    
    flipped |= (flipped << 16) & adjacent_o_discs;
    flipped |= (flipped << 16) & adjacent_o_discs;
    
    legals |= flipped << 8;
    
    
    /* direction NE */
    flipped  = (p_discs >>  7) & inner_o_discs;
    flipped |= (flipped >>  7) & inner_o_discs;
    
    adjacent_o_discs = inner_o_discs & (inner_o_discs >> 7);
    
    flipped |= (flipped >> 14) & adjacent_o_discs;
    flipped |= (flipped >> 14) & adjacent_o_discs;
    
    legals |= flipped >> 7;
    
    
    /* direction SW */
    flipped  = (p_discs <<  7) & inner_o_discs;
    flipped |= (flipped <<  7) & inner_o_discs;
    
    adjacent_o_discs = inner_o_discs & (inner_o_discs << 7);
    
    flipped |= (flipped << 14) & adjacent_o_discs;
    flipped |= (flipped << 14) & adjacent_o_discs;
    
    legals |= flipped << 7;
    
    
    /* direction NW */
    flipped  = (p_discs >>  9) & inner_o_discs;
    flipped |= (flipped >>  9) & inner_o_discs;
    
    adjacent_o_discs = inner_o_discs & (inner_o_discs >> 9);
    
    flipped |= (flipped >> 18) & adjacent_o_discs;
    flipped |= (flipped >> 18) & adjacent_o_discs;
    
    legals |= flipped >> 9;
    
    
    /* direction SE */
    flipped  = (p_discs <<  9) & inner_o_discs;
    flipped |= (flipped <<  9) & inner_o_discs;
    
    adjacent_o_discs = inner_o_discs & (inner_o_discs << 9);
    
    flipped |= (flipped << 18) & adjacent_o_discs;
    flipped |= (flipped << 18) & adjacent_o_discs;
    
    legals |= flipped << 9;
    
    //Removes existing discs
    legals &= ~(p_discs | o_discs);
    
    return legals;
    
}

#endif


inline void RXBitBoard::dual_count_legal_moves(int& mob_P, int& mob_O) const {
    return RXBitBoard::dual_count_legal_moves(discs[player], discs[player^1], mob_P, mob_O);
}

inline void RXBitBoard::dual_count_legal_moves(const unsigned long long discs_player, const unsigned long long discs_opponent, int& mob_P, int& mob_O) {
    mob_P = RXBitBoard::count_legal_moves(discs_player, discs_opponent);
    mob_O = RXBitBoard::count_legal_moves(discs_opponent, discs_player);
}


