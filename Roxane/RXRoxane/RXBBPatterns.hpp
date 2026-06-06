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

#ifndef RXBBPATTERN_HPP
#define RXBBPATTERN_HPP

#include <algorithm> //std::min
#include <atomic>

#include "RXConstantes.hpp"
#include "RXBitBoard.hpp"
#include "RXPattern.hpp"
#include "RXEvaluation.hpp"

#if ARCH ==  ARCH_ARM_NEON
    #include <arm_neon.h>
#elif ARCH == ARCH_X86_AVX2
    #include <x86intrin.h>
#endif


class RXBBPatterns {
    
    void set_BLACK(const int pos);
    void set_WHITE(const int pos);
    
    // move functions
    typedef void (RXBBPatterns::*type_update_patterns)(RXMove& move) const;
    
#define func(pos) \
    void update_patterns_BLACK_##pos(RXMove& move) const; \
    void update_patterns_WHITE_##pos(RXMove& move) const
    
    func(A1); func(B1); func(C1); func(D1); func(E1); func(F1); func(G1); func(H1);
    func(A2); func(B2); func(C2); func(D2); func(E2); func(F2); func(G2); func(H2);
    func(A3); func(B3); func(C3); func(D3); func(E3); func(F3); func(G3); func(H3);
    func(A4); func(B4); func(C4); func(D4); func(E4); func(F4); func(G4); func(H4);
    func(A5); func(B5); func(C5); func(D5); func(E5); func(F5); func(G5); func(H5);
    func(A6); func(B6); func(C6); func(D6); func(E6); func(F6); func(G6); func(H6);
    func(A7); func(B7); func(C7); func(D7); func(E7); func(F7); func(G7); func(H7);
    func(A8); func(B8); func(C8); func(D8); func(E8); func(F8); func(G8); func(H8);
    
#undef func
    
    static type_update_patterns const update_patterns[][2];


    public :

    RXBitBoard board;
    RXPattern* pattern;

    
    RXBBPatterns();
    
    //constructeur par copie
    RXBBPatterns(const RXBBPatterns& src);
    
    RXBBPatterns& operator=(const RXBBPatterns& src);
    
    ~RXBBPatterns();
    
    void build(const std::string& init);
    
    void do_move(RXMove& move);
    void undo_move(const RXMove& move);
    
    
    
    
    inline void patterns_update(RXMove& move) const;

    
    void reset();
    
    double get_n_nodes() const { return board.get_n_nodes(); }
    int get_n_empty() const { return board.n_empty; }
    
    int final_score() const;
    template <const bool UseFM = true>
    int get_score() const;
    template <const bool UseFM = true>
    int get_score(const RXMove& move) const;
        
    /* DEBUG */
    friend std::ostream& operator<<(std::ostream& os, RXBBPatterns& sBoard);
    
#ifdef STATS_FM
    
    static inline std::atomic<long long> count_with_FM;
    static inline std::atomic<long long> count_without_FM;
    
    static void init_stats() {
        count_with_FM.store(0, std::memory_order_relaxed);
        count_without_FM.store(0, std::memory_order_relaxed);
    }
    
    static void print_stats() {
        long long fm = count_with_FM.load();
        long long no_fm = count_without_FM.load();
        double ratio = (fm + no_fm > 0) ? (double)fm / (fm + no_fm) * 100.0 : 0;
        
        printf("Utilisation FM: %lld | Sans FM: %lld | Ratio FM: %.2f%%\n", fm, no_fm, ratio);
    }
#endif
    
};

__attribute__((always_inline))
inline RXBBPatterns& RXBBPatterns::operator=(const RXBBPatterns& src) {

    // Optimization: Skip self-assignment check to avoid branch misprediction.
    // Safe because 'pattern' pointers are stable (allocated at startup,
    // never reassigned or deleted during search).
//    if(this != &src) {
    
        board = src.board;
    
        *pattern = *(src.pattern); //copy

//    }
    
    return *this;
}

__attribute__((always_inline))
inline void RXBBPatterns::patterns_update(RXMove& move) const {
    (this->*RXBBPatterns::update_patterns[move.position][board.player])(move);
}

__attribute__((always_inline))
inline void RXBBPatterns::do_move(RXMove& move) {
    board.do_move(move);
    move.undo_pattern = pattern;
    pattern = move.pattern;
}

__attribute__((always_inline))
inline void RXBBPatterns::undo_move(const RXMove& move) {
    pattern = move.undo_pattern;
    board.undo_move(move);
}

//track bug HASH 24/01/2025
inline int RXBBPatterns::final_score() const {
    int score = board.final_score();
    if(score<0)
        return (-MAX_SCORE+1) + (score + 64);
    return (MAX_SCORE-1) + (score-64);
}

// =============================================================================
// Refactored get_score() and get_score(RXMove&).
// The shared logic is encapsulated within acc_score(), which is called by both variants.
// =============================================================================

// -----------------------------------------------------------------------------
// Evaluates the board score using a hybrid Linear + Factorization Machine model.
// Called with precalculated mobilities
// -----------------------------------------------------------------------------
template <const bool UseFM = true,
          int FM_STAGE_MIN  = 9,
          int FM_STAGE_MAX  = 55,
          int FM_STAGE_SWAP = 33>
inline int acc_score(const int   stage,
                          const int   color,
                          const int*  __restrict p,
                          const int   mob_Player,
                          const int   mob_Opponent,
                          const auto& tab_eval,
                          const unsigned long long filled
                     )
{
    using enum RXEvaluation::Feature;
    
    const short* __restrict const mob_P  = tab_eval[MOB_P];
    const short* __restrict const mob_O  = tab_eval[MOB_O];
    const short* __restrict const diag5  = tab_eval[DIAG5];
    const short* __restrict const diag6  = tab_eval[DIAG6];
    const short* __restrict const diag7  = tab_eval[DIAG7];
    const short* __restrict const diag8  = tab_eval[DIAG8];
    const short* __restrict const edge1  = tab_eval[EDGE1];
    const short* __restrict const edge2  = tab_eval[EDGE2];
    const short* __restrict const edge3  = tab_eval[EDGE3];
    const short* __restrict const edge4  = tab_eval[EDGE4];
    const short* __restrict const hv2    = tab_eval[HV2];
    const short* __restrict const hv3    = tab_eval[HV3];
    const short* __restrict const hv4    = tab_eval[HV4];
    const short* __restrict const corner = tab_eval[CORNER];
    
    // -------------------------------------------------------------------------
    // Precomputed indices (Calculated once, reused by both linear and FM stages)
    // -------------------------------------------------------------------------
    
    const unsigned long long maskA = filled & 0x8142000000000000ULL;
    const unsigned long long maskB = filled & 0x0102000000000201ULL;
    const unsigned long long maskC = filled & 0x0000000000004281ULL;
    const unsigned long long maskD = filled & 0x8040000000004080ULL;
    
    // Selection of branch-friendly table
    const short* __restrict const edgeA = maskA? edge1 : edge2;
    const short* __restrict const edgeB = maskB? edge1 : edge2;
    const short* __restrict const edgeC = maskC? edge1 : edge2;
    const short* __restrict const edgeD = maskD? edge1 : edge2;
    
    
    const int mask = color >> 31;   // 0x00000000 si color=+1, 0xFFFFFFFF si color=-1
    
#if ARCH ==  ARCH_ARM_NEON
    
    // ── Calcul SIMD des (50+2) 50 cp = (patt[i] ^ mask) - mask ────────────────
    // patt[] est int[64], on charge en int32x4, on XOR+SUB vectoriellement
    
    const int32_t* pp = p;
    int32x4_t vmask4  = vdupq_n_s32(mask);
    
    // Charger et transformer les 46 indices (12 groupes de 4, + reste)
    int32x4_t vcp_0__3  = vsubq_s32(veorq_s32(vld1q_s32(pp +  0), vmask4), vmask4);
    int32x4_t vcp_4__7  = vsubq_s32(veorq_s32(vld1q_s32(pp +  4), vmask4), vmask4);
    int32x4_t vcp_8_11  = vsubq_s32(veorq_s32(vld1q_s32(pp +  8), vmask4), vmask4);
    int32x4_t vcp12_15  = vsubq_s32(veorq_s32(vld1q_s32(pp + 12), vmask4), vmask4);
    int32x4_t vcp16_19  = vsubq_s32(veorq_s32(vld1q_s32(pp + 16), vmask4), vmask4);
    int32x4_t vcp20_23  = vsubq_s32(veorq_s32(vld1q_s32(pp + 20), vmask4), vmask4);
    int32x4_t vcp24_27  = vsubq_s32(veorq_s32(vld1q_s32(pp + 24), vmask4), vmask4);
    int32x4_t vcp28_31  = vsubq_s32(veorq_s32(vld1q_s32(pp + 28), vmask4), vmask4);
    int32x4_t vcp32_35  = vsubq_s32(veorq_s32(vld1q_s32(pp + 32), vmask4), vmask4);
    int32x4_t vcp36_39  = vsubq_s32(veorq_s32(vld1q_s32(pp + 36), vmask4), vmask4);
    int32x4_t vcp40_43  = vsubq_s32(veorq_s32(vld1q_s32(pp + 40), vmask4), vmask4);
    int32x4_t vcp44_47  = vsubq_s32(veorq_s32(vld1q_s32(pp + 44), vmask4), vmask4);
    int32x4_t vcp48_51  = vsubq_s32(veorq_s32(vld1q_s32(pp + 48), vmask4), vmask4);
    
    // pp+48 charge 4 ints mais seuls [48] et [49] sont utilisés — les 2 extras
    // sont lus mais jamais utilisés, inoffensif si patt[50..51] est accessible
    // patt[50..63] sont du padding
    
    // Extraire les scalaires nécessaires
    // (le compilateur les garde en registre — pas d'aller-retour mémoire)
    const int cp0  = vgetq_lane_s32(vcp_0__3,  0), cp1  = vgetq_lane_s32(vcp_0__3,  1);
    const int cp2  = vgetq_lane_s32(vcp_0__3,  2), cp3  = vgetq_lane_s32(vcp_0__3,  3);
    const int cp4  = vgetq_lane_s32(vcp_4__7,  0), cp5  = vgetq_lane_s32(vcp_4__7,  1);
    const int cp6  = vgetq_lane_s32(vcp_4__7,  2), cp7  = vgetq_lane_s32(vcp_4__7,  3);
    const int cp8  = vgetq_lane_s32(vcp_8_11,  0), cp9  = vgetq_lane_s32(vcp_8_11,  1);
    const int cp10 = vgetq_lane_s32(vcp_8_11,  2), cp11 = vgetq_lane_s32(vcp_8_11,  3);
    const int cp12 = vgetq_lane_s32(vcp12_15,  0), cp13 = vgetq_lane_s32(vcp12_15,  1);
    
    const int cpA = maskA ? vgetq_lane_s32(vcp12_15, 2) : vgetq_lane_s32(vcp16_19, 2);
    const int cpB = maskB ? vgetq_lane_s32(vcp12_15, 3) : vgetq_lane_s32(vcp16_19, 3);
    const int cpC = maskC ? vgetq_lane_s32(vcp16_19, 0) : vgetq_lane_s32(vcp20_23, 0);
    const int cpD = maskD ? vgetq_lane_s32(vcp16_19, 1) : vgetq_lane_s32(vcp20_23, 1);
    
    const int cp22 = vgetq_lane_s32(vcp20_23,  2), cp23 = vgetq_lane_s32(vcp20_23,  3);
    const int cp24 = vgetq_lane_s32(vcp24_27,  0), cp25 = vgetq_lane_s32(vcp24_27,  1);
    const int cp26 = vgetq_lane_s32(vcp24_27,  2), cp27 = vgetq_lane_s32(vcp24_27,  3);
    const int cp28 = vgetq_lane_s32(vcp28_31,  0), cp29 = vgetq_lane_s32(vcp28_31,  1);
    const int cp30 = vgetq_lane_s32(vcp28_31,  2), cp31 = vgetq_lane_s32(vcp28_31,  3);
    const int cp32 = vgetq_lane_s32(vcp32_35,  0), cp33 = vgetq_lane_s32(vcp32_35,  1);
    const int cp34 = vgetq_lane_s32(vcp32_35,  2), cp35 = vgetq_lane_s32(vcp32_35,  3);
    const int cp36 = vgetq_lane_s32(vcp36_39,  0), cp37 = vgetq_lane_s32(vcp36_39,  1);
    const int cp38 = vgetq_lane_s32(vcp36_39,  2), cp39 = vgetq_lane_s32(vcp36_39,  3);
    const int cp40 = vgetq_lane_s32(vcp40_43,  0), cp41 = vgetq_lane_s32(vcp40_43,  1);
    const int cp42 = vgetq_lane_s32(vcp40_43,  2), cp43 = vgetq_lane_s32(vcp40_43,  3);
    const int cp44 = vgetq_lane_s32(vcp44_47,  0), cp45 = vgetq_lane_s32(vcp44_47,  1);
    const int cp46 = vgetq_lane_s32(vcp44_47,  2), cp47 = vgetq_lane_s32(vcp44_47,  3);
    const int cp48 = vgetq_lane_s32(vcp48_51,  0), cp49 = vgetq_lane_s32(vcp48_51,  1);
    //    const int cp50 = vgetq_lane_s32(vcp48_51,  2), cp51 = vgetq_lane_s32(vcp48_51,  3); //padding

#elif ARCH == ARCH_X86_AVX2

    // ── Calcul AVX2 des 50 cp = (patt[i] ^ mask) - mask ──────────────────────
    // AVX2 traite 8 int32 à la fois (256 bits / 32 bits)
    
    const int32_t* pp = p;
    __m256i vmask8 = _mm256_set1_epi32(mask);
    
    // 7 registres × 8 int32 = 56 valeurs (on n'utilise que 50)
    __m256i vcp_0__7  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp +  0)), vmask8), vmask8);
    __m256i vcp_8_15  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp +  8)), vmask8), vmask8);
    __m256i vcp16_23  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp + 16)), vmask8), vmask8);
    __m256i vcp24_31  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp + 24)), vmask8), vmask8);
    __m256i vcp32_39  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp + 32)), vmask8), vmask8);
    __m256i vcp40_47  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp + 40)), vmask8), vmask8);
    __m256i vcp48_51  = _mm256_sub_epi32(_mm256_xor_si256(_mm256_loadu_si256((__m256i*)(pp + 48)), vmask8), vmask8);
    // pp+48 charge 8 ints mais seuls [48] et [49] sont utilisés — padding requis jusqu'à [55]

    // Extraire les scalaires (le compilateur les garde en registre)
    const int cp0  = _mm256_extract_epi32(vcp_0__7, 0), cp1  = _mm256_extract_epi32(vcp_0__7, 1);
    const int cp2  = _mm256_extract_epi32(vcp_0__7, 2), cp3  = _mm256_extract_epi32(vcp_0__7, 3);
    const int cp4  = _mm256_extract_epi32(vcp_0__7, 4), cp5  = _mm256_extract_epi32(vcp_0__7, 5);
    const int cp6  = _mm256_extract_epi32(vcp_0__7, 6), cp7  = _mm256_extract_epi32(vcp_0__7, 7);
    const int cp8  = _mm256_extract_epi32(vcp_8_15, 0), cp9  = _mm256_extract_epi32(vcp_8_15, 1);
    const int cp10 = _mm256_extract_epi32(vcp_8_15, 2), cp11 = _mm256_extract_epi32(vcp_8_15, 3);
    const int cp12 = _mm256_extract_epi32(vcp_8_15, 4), cp13 = _mm256_extract_epi32(vcp_8_15, 5);

    const int cpA = maskA ? _mm256_extract_epi32(vcp_8_15, 6) : _mm256_extract_epi32(vcp16_23, 2);
    const int cpB = maskB ? _mm256_extract_epi32(vcp_8_15, 7) : _mm256_extract_epi32(vcp16_23, 3);
    const int cpC = maskC ? _mm256_extract_epi32(vcp16_23, 0) : _mm256_extract_epi32(vcp16_23, 4);
    const int cpD = maskD ? _mm256_extract_epi32(vcp16_23, 1) : _mm256_extract_epi32(vcp16_23, 5);

    const int cp22 = _mm256_extract_epi32(vcp16_23, 6), cp23 = _mm256_extract_epi32(vcp16_23, 7);
    const int cp24 = _mm256_extract_epi32(vcp24_31, 0), cp25 = _mm256_extract_epi32(vcp24_31, 1);
    const int cp26 = _mm256_extract_epi32(vcp24_31, 2), cp27 = _mm256_extract_epi32(vcp24_31, 3);
    const int cp28 = _mm256_extract_epi32(vcp24_31, 4), cp29 = _mm256_extract_epi32(vcp24_31, 5);
    const int cp30 = _mm256_extract_epi32(vcp24_31, 6), cp31 = _mm256_extract_epi32(vcp24_31, 7);
    const int cp32 = _mm256_extract_epi32(vcp32_39, 0), cp33 = _mm256_extract_epi32(vcp32_39, 1);
    const int cp34 = _mm256_extract_epi32(vcp32_39, 2), cp35 = _mm256_extract_epi32(vcp32_39, 3);
    const int cp36 = _mm256_extract_epi32(vcp32_39, 4), cp37 = _mm256_extract_epi32(vcp32_39, 5);
    const int cp38 = _mm256_extract_epi32(vcp32_39, 6), cp39 = _mm256_extract_epi32(vcp32_39, 7);
    const int cp40 = _mm256_extract_epi32(vcp40_47, 0), cp41 = _mm256_extract_epi32(vcp40_47, 1);
    const int cp42 = _mm256_extract_epi32(vcp40_47, 2), cp43 = _mm256_extract_epi32(vcp40_47, 3);
    const int cp44 = _mm256_extract_epi32(vcp40_47, 4), cp45 = _mm256_extract_epi32(vcp40_47, 5);
    const int cp46 = _mm256_extract_epi32(vcp40_47, 6), cp47 = _mm256_extract_epi32(vcp40_47, 7);
    const int cp48 = _mm256_extract_epi32(vcp48_51, 0), cp49 = _mm256_extract_epi32(vcp48_51, 1);

#else
    auto cp = [mask, p](int i) __attribute__((always_inline)) -> int {
        return (p[i] ^ mask) - mask;
    };
    
    const int cp0  = cp(0),  cp1  = cp(1),  cp2  = cp(2),  cp3  = cp(3);
    const int cp4  = cp(4),  cp5  = cp(5),  cp6  = cp(6),  cp7  = cp(7);
    const int cp8  = cp(8),  cp9  = cp(9),  cp10 = cp(10), cp11 = cp(11);
    const int cp12 = cp(12), cp13 = cp(13);
    
    const int cpA = maskA ? cp(14) : cp(18);
    const int cpB = maskB ? cp(15) : cp(19);
    const int cpC = maskC ? cp(16) : cp(20);
    const int cpD = maskD ? cp(17) : cp(21);
    
    const int cp22 = cp(22), cp23 = cp(23), cp24 = cp(24), cp25 = cp(25);
    const int cp26 = cp(26), cp27 = cp(27), cp28 = cp(28), cp29 = cp(29);
    const int cp30 = cp(30), cp31 = cp(31), cp32 = cp(32), cp33 = cp(33);
    const int cp34 = cp(34), cp35 = cp(35), cp36 = cp(36), cp37 = cp(37);
    const int cp38 = cp(38), cp39 = cp(39), cp40 = cp(40), cp41 = cp(41);
    const int cp42 = cp(42), cp43 = cp(43), cp44 = cp(44), cp45 = cp(45);
    const int cp46 = cp(46), cp47 = cp(47), cp48 = cp(48), cp49 = cp(49);
    
#endif
    
    // -------------------------------------------------------------------------
    // Linear Evaluation: Standard weight accumulation for mobility and patterns
    // -------------------------------------------------------------------------
    int
    eval  = mob_P[mob_Player];
    eval += mob_O[mob_Opponent];
    
    eval += diag5[cp0]  + diag5[cp1]  + diag5[cp2]  + diag5[cp3];
    eval += diag6[cp4]  + diag6[cp5]  + diag6[cp6]  + diag6[cp7];
    eval += diag7[cp8]  + diag7[cp9]  + diag7[cp10] + diag7[cp11];
    eval += diag8[cp12] + diag8[cp13];
    
    eval += edgeA[cpA] + edgeB[cpB] + edgeC[cpC] + edgeD[cpD];
    
    eval += edge3[cp22] + edge3[cp23] + edge3[cp24] + edge3[cp25];
    eval += edge4[cp26] + edge4[cp27] + edge4[cp28] + edge4[cp29]
          + edge4[cp30] + edge4[cp31] + edge4[cp32] + edge4[cp33];
    
    eval += hv2[cp34] + hv2[cp35] + hv2[cp36] + hv2[cp37];
    eval += hv3[cp38] + hv3[cp39] + hv3[cp40] + hv3[cp41];
    eval += hv4[cp42] + hv4[cp43] + hv4[cp44] + hv4[cp45];
    
    eval += corner[cp46] + corner[cp47] + corner[cp48] + corner[cp49];
    
    // -------------------------------------------------------------------------
    // FM Correction: Captures non-linear interactions between features
    // -------------------------------------------------------------------------
#ifdef FACT_MACH
    
    if constexpr (UseFM) {
        
        if (stage > FM_STAGE_MIN && stage < FM_STAGE_MAX) {
            
            int id = stage < FM_STAGE_SWAP ? 0:1; //Midgame/Endgame
            
            eval += RXEvaluation::eval_w0[stage];
            
#if ARCH == ARCH_ARM_NEON
            
            // OPTIMIZATION
            // Version 1
            // sumsq += x²   (48×)
            // res = -sumsq + sum² /* 4 instructions vnegq_s32 */
            //     = -Σx² + sum²
            
            // Version 2
            // sumsq -= x²   (48×)  →  sumsq = -Σx²
            // res = sumsq + sum²
            //     = -Σx² + sum²
            
            // OPTIMIZATION: Total accumulated sum remains within the short integer bounds.
            // Ensure this holds true after any changes to the latent vectors.
            int16x8_t sum_lo = vdupq_n_s16(0);   // lanes 0..7
            int16x8_t sum_hi = vdupq_n_s16(0);   // lanes 8..15
            
            int32x4_t sumsq_0 = vdupq_n_s32(0);
            int32x4_t sumsq_1 = vdupq_n_s32(0);
            int32x4_t sumsq_2 = vdupq_n_s32(0);
            int32x4_t sumsq_3 = vdupq_n_s32(0);
            
            auto acc = [&](const Vec_short* __restrict V, int idx) __attribute__((always_inline)) {
                const Vec_short& v_ref = V[idx];
                
                // accumulation (48 fois) :
                sumsq_0 = vsubq_s32(sumsq_0, vld1q_s32(v_ref.squares    ));
                sumsq_1 = vsubq_s32(sumsq_1, vld1q_s32(v_ref.squares + 4));
                sumsq_2 = vsubq_s32(sumsq_2, vld1q_s32(v_ref.squares + 8));
                sumsq_3 = vsubq_s32(sumsq_3, vld1q_s32(v_ref.squares +12));

                // OPTIMIZATION: Total accumulated sum remains within the short integer bounds.
                // Ensure this holds true after any changes to the latent vectors.
                sum_lo = vaddq_s16(sum_lo, vld1q_s16(v_ref.data    ));
                sum_hi = vaddq_s16(sum_hi, vld1q_s16(v_ref.data + 8));
                
            };
            
#elif ARCH == ARCH_X86_AVX2

            // RANK=16 : sum_vx et sum_vx_sq en 2 registres __m256i (16 × int16)
            // sum en int16 (bounds garantis par la conception des vecteurs latents)
            // sumsq en int32 pour éviter l'overflow (50 acc × valeurs int16²)
            
            __m256i sum     = _mm256_setzero_si256();  // 16 × int16 : Σ x_i
            __m256i sumsq_0 = _mm256_setzero_si256();  // 8  × int32 : -Σ x_i² lanes 0..7
            __m256i sumsq_1 = _mm256_setzero_si256();  // 8  × int32 : -Σ x_i² lanes 8..15
            
            auto acc = [&](const Vec_short* __restrict V, int idx) __attribute__((always_inline)) {
                const Vec_short& v_ref = V[idx];
                
                // sumsq -= x² (équivalent NEON version 2 : sumsq = -Σx²)
                sumsq_0 = _mm256_sub_epi32(sumsq_0, _mm256_loadu_si256((__m256i*)(v_ref.squares    )));
                sumsq_1 = _mm256_sub_epi32(sumsq_1, _mm256_loadu_si256((__m256i*)(v_ref.squares + 8)));
                
                // sum += x (int16 × 16)
                sum     = _mm256_add_epi16(sum, _mm256_loadu_si256((__m256i*)v_ref.data));
            };

            
#else
            
            int sum_vx[RANK]    = {};
            int sum_vx_sq[RANK] = {};
            
            auto acc = [&](const Vec_short* V, int idx) __attribute__((always_inline)) {
                const Vec_short& vec = V[idx];
                for (unsigned int f = 0; f < RANK; ++f) {
                    sum_vx[f]    += vec.data[f];    // Addition simple
                    sum_vx_sq[f] += vec.squares[f]; // Addition simple (plus de multiplication !)
                }
            };
            
#endif  // __ARM_NEON
            
            const Vec_short* const& vMob_P  = RXEvaluation::gVMob_P[id];
            const Vec_short* const& vMob_O  = RXEvaluation::gVMob_O[id];
            const Vec_short* const& vDiag5  = RXEvaluation::gVDiag5[id];
            const Vec_short* const& vDiag6  = RXEvaluation::gVDiag6[id];
            const Vec_short* const& vDiag7  = RXEvaluation::gVDiag7[id];
            const Vec_short* const& vDiag8  = RXEvaluation::gVDiag8[id];
            const Vec_short* const& vEdge1  = RXEvaluation::gVEdge1[id];
            const Vec_short* const& vEdge2  = RXEvaluation::gVEdge2[id];
            const Vec_short* const& vEdge3  = RXEvaluation::gVEdge3[id];
            const Vec_short* const& vEdge4  = RXEvaluation::gVEdge4[id];
            const Vec_short* const& vHv2    = RXEvaluation::gVHv2[id];
            const Vec_short* const& vHv3    = RXEvaluation::gVHv3[id];
            const Vec_short* const& vHv4    = RXEvaluation::gVHv4[id];
            const Vec_short* const& vCorner = RXEvaluation::gVCorner[id];
            
            
            // Selection of branch-friendly table
            const Vec_short* __restrict const vEdgeA = maskA ? vEdge1 : vEdge2;
            const Vec_short* __restrict const vEdgeB = maskB ? vEdge1 : vEdge2;
            const Vec_short* __restrict const vEdgeC = maskC ? vEdge1 : vEdge2;
            const Vec_short* __restrict const vEdgeD = maskD ? vEdge1 : vEdge2;
            
            /*
             // Defer to the compiler.
             
             // CORNER — 4 accès dans 177147 entrées → priorité max, prefetch en premier
             __builtin_prefetch(&vCorner[cp42], 0, 3);
             __builtin_prefetch(&vCorner[cp43], 0, 3);
             __builtin_prefetch(&vCorner[cp44], 0, 3);
             __builtin_prefetch(&vCorner[cp45], 0, 3);
             
             // EDGE2X — 4 accès dans 59049 entrées / EDGE64 — 4 accès
             __builtin_prefetch(&vEdgeA[cpA], 0, 1);
             __builtin_prefetch(&vEdgeB[cpB], 0, 1);
             __builtin_prefetch(&vEdgeC[cpC], 0, 1);
             __builtin_prefetch(&vEdgeD[cpD], 0, 1);
             
             // EDGE5 — 8 accès
             __builtin_prefetch(&vEdge3[cp22], 0, 1);
             __builtin_prefetch(&vEdge3[cp23], 0, 1);
             __builtin_prefetch(&vEdge3[cp24], 0, 1);
             __builtin_prefetch(&vEdge3[cp25], 0, 1);
             __builtin_prefetch(&vEdge3[cp26], 0, 1);
             __builtin_prefetch(&vEdge3[cp27], 0, 1);
             __builtin_prefetch(&vEdge3[cp28], 0, 1);
             __builtin_prefetch(&vEdge3[cp29], 0, 1);
             */
            
            // mobilités
            acc(vMob_P, mob_Player);
            acc(vMob_O, mob_Opponent);
            
            acc(vDiag5, cp0);  acc(vDiag5, cp1);  acc(vDiag5, cp2);  acc(vDiag5, cp3);
            acc(vDiag6, cp4);  acc(vDiag6, cp5);  acc(vDiag6, cp6);  acc(vDiag6, cp7);
            acc(vDiag7, cp8);  acc(vDiag7, cp9);  acc(vDiag7, cp10); acc(vDiag7, cp11);
            acc(vDiag8, cp12); acc(vDiag8, cp13);
            acc(vHv2,   cp34); acc(vHv2,   cp35); acc(vHv2,   cp36); acc(vHv2,   cp37);
            acc(vHv3,   cp38); acc(vHv3,   cp39); acc(vHv3,   cp40); acc(vHv3,   cp41);
            acc(vHv4,   cp42); acc(vHv4,   cp43); acc(vHv4,   cp44); acc(vHv4,   cp45);
            
            acc(vEdgeA,  cpA); acc(vEdgeB,  cpB); acc(vEdgeC,  cpC); acc(vEdgeD,  cpD);
            
            acc(vEdge3, cp22); acc(vEdge3, cp23); acc(vEdge3, cp24); acc(vEdge3, cp25);
            acc(vEdge4, cp26); acc(vEdge4, cp27); acc(vEdge4, cp28); acc(vEdge4, cp29);
            acc(vEdge4, cp30); acc(vEdge4, cp31); acc(vEdge4, cp32); acc(vEdge4, cp33);
            
            acc(vCorner,cp46); acc(vCorner,cp47); acc(vCorner,cp48); acc(vCorner,cp49);
            
            
#if ARCH == ARCH_ARM_NEON
            

            // widening multiply-accumulate depuis int16 (sum est encore int16x8_t)
            // vmlal_s16 : res += (int16 * int16) avec widening automatique
            // Optimisation : vmlal_high_s16 évite les vget_high
            int32x4_t res0 = vmlal_s16     (sumsq_0, vget_low_s16(sum_lo), vget_low_s16(sum_lo));
            int32x4_t res1 = vmlal_high_s16(sumsq_1, sum_lo, sum_lo);   // ← smlal2
            int32x4_t res2 = vmlal_s16     (sumsq_2, vget_low_s16(sum_hi), vget_low_s16(sum_hi));
            int32x4_t res3 = vmlal_high_s16(sumsq_3, sum_hi, sum_hi);   // ← smlal2

            int fm_interaction = vaddvq_s32(
                                            vaddq_s32(vaddq_s32(res0, res1),
                                                      vaddq_s32(res2, res3))
                                            );

#elif ARCH == ARCH_X86_AVX2
            
            // Étape 1 : sq via madd (évite cvtepi16 + mullo)
            // sum est __m256i int16 → on travaille directement dessus
            __m256i sq = _mm256_madd_epi16(sum, sum);
            // sq contient 8×int32 : sq[i] = sum[2i]² + sum[2i+1]²

            // Étape 2 : fusionner sumsq (sumsq_0 et sumsq_1 doivent être sur 8 lanes chacun)
            __m256i sumsq = _mm256_add_epi32(sumsq_0, sumsq_1);

            // Étape 3 : res = sq + sumsq
            __m256i res = _mm256_add_epi32(sq, sumsq);

            // Étape 4 : réduction (inchangée, déjà optimale sur Intel)
            __m128i r = _mm_add_epi32(_mm256_castsi256_si128(res),
                                        _mm256_extracti128_si256(res, 1));
            r = _mm_add_epi32(r, _mm_shuffle_epi32(r, 0x4E));
            r = _mm_add_epi32(r, _mm_shuffle_epi32(r, 0xB1));
            int fm_interaction = _mm_cvtsi128_si32(r);
            
#else
            int fm_interaction = 0;
            for (unsigned int f = 0; f < RANK; ++f)
                fm_interaction += sum_vx[f] * sum_vx[f] - sum_vx_sq[f];
#endif
            
            //eval += ((fm_interaction/2)+128) / 256;
            eval += (fm_interaction + 256 - ((fm_interaction>>31) & 512)) / 512;

        }
    }
#endif  // FACT_MACH
    
    //if (eval > 0) eval += 128; else eval -= 128;
    //return eval / 256;
    
    // Version la plus rapide — un seul ASR sur ARM
    return (eval + 128 - ((eval >> 31) & 256)) / 256;
    
}

// -----------------------------------------------------------------------------
//  get_score() — position courante
// -----------------------------------------------------------------------------
template <const bool UseFM>
inline int RXBBPatterns::get_score() const
{

#ifdef STATS_FM
    
    if constexpr (UseFM) {
        count_with_FM.fetch_add(1, std::memory_order_relaxed);
    } else {
        count_without_FM.fetch_add(1, std::memory_order_relaxed);
    }
    
#endif
    
    const int stage = 60 - board.n_empty;
    const int color = 1 - 2 * board.player;
    
    const unsigned long long filled = board.discs[board.player] | board.discs[board.player^1];

#if ARCH == ARCH_ARM_NEON

    uint64x2_t mob  = board.dual_count_legal_moves();
    const int mob_P = std::min(23, (int)vgetq_lane_u64(mob, 0));
    const int mob_O = std::min(23, (int)vgetq_lane_u64(mob, 1));

#else
    
    unsigned int mob_P = 0;
    unsigned int mob_O = 0;
    board.dual_count_legal_moves(mob_P, mob_O);
    
#endif

    return acc_score<UseFM>(stage, color, pattern->patt,
                          mob_P, mob_O,
                          RXEvaluation::eval_w[stage],
                          filled);
}


// -----------------------------------------------------------------------------
//  get_score(RXMove&) — après un coup candidat
// -----------------------------------------------------------------------------
template <const bool UseFM>
inline int RXBBPatterns::get_score(const RXMove& move) const
{
    
#ifdef STATS_FM
    
    if constexpr (UseFM) {
        count_with_FM.fetch_add(1, std::memory_order_relaxed);
    } else {
        count_without_FM.fetch_add(1, std::memory_order_relaxed);
    }
    
#endif

    const int stage = 61 - board.n_empty;
    const int color = 2 * board.player - 1;
    
    const unsigned long long discs_player   = board.discs[board.player ^ 1] ^ move.flipped;
    const unsigned long long discs_opponent = (board.discs[board.player] ^ move.flipped) | move.square;
    
    const unsigned long long filled = discs_player | discs_opponent;

#if ARCH == ARCH_ARM_NEON

    uint64x2_t mob  = RXBitBoard::dual_count_legal_moves(discs_player, discs_opponent);
    const int mob_P = std::min(23, (int)vgetq_lane_u64(mob, 0));
    const int mob_O = std::min(23, (int)vgetq_lane_u64(mob, 1));

#else
    
    unsigned int mob_P = 0;
    unsigned int mob_O = 0;
    RXBitBoard::dual_count_legal_moves(discs_player, discs_opponent, mob_P, mob_O);

#endif


    return acc_score<UseFM>(stage, color, move.pattern->patt,
                          mob_P, mob_O,
                          RXEvaluation::eval_w[stage],
                          filled);
}


#endif
