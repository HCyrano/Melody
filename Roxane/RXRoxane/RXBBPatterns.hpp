/*
 *  RXBBPatterns.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 31/07/05.
 *  Copyleft 2005-2025 personnel.
 *
 */

#ifndef RXBBPATTERN_HPP
#define RXBBPATTERN_HPP

#include <cmath>
#ifdef __ARM_NEON
    #include <arm_neon.h>
#endif
#include <atomic>

#include "RXConstantes.hpp"
#include "RXBitBoard.hpp"
#include "RXPattern.hpp"
#include "RXEvaluation.hpp"


class RXBBPatterns {
    
    void set_BLACK(const int pos);
    void set_WHITE(const int pos);
    
    // move functions
#define func(pos)   void update_patterns_BLACK_##pos(RXMove& move) const; \
void update_patterns_WHITE_##pos(RXMove& move) const
    
    func(A1); func(B1); func(C1); func(D1); func(E1); func(F1); func(G1); func(H1);
    func(A2); func(B2); func(C2); func(D2); func(E2); func(F2); func(G2); func(H2);
    func(A3); func(B3); func(C3); func(D3); func(E3); func(F3); func(G3); func(H3);
    func(A4); func(B4); func(C4);                     func(F4); func(G4); func(H4);
    func(A5); func(B5); func(C5);                     func(F5); func(G5); func(H5);
    func(A6); func(B6); func(C6); func(D6); func(E6); func(F6); func(G6); func(H6);
    func(A7); func(B7); func(C7); func(D7); func(E7); func(F7); func(G7); func(H7);
    func(A8); func(B8); func(C8); func(D8); func(E8); func(F8); func(G8); func(H8);
#undef func

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
    
    
    
    void (RXBBPatterns::*update_patterns[64][2])(RXMove& move) const;
    void init_update_patterns();
    
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
template <const bool UseFM = true>
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
    
    const short* __restrict const edgeA = maskA? edge1 : edge2;
    const short* __restrict const edgeB = maskB? edge1 : edge2;
    const short* __restrict const edgeC = maskC? edge1 : edge2;
    const short* __restrict const edgeD = maskD? edge1 : edge2;
    
    
    const int mask = color >> 31;   // 0x00000000 si color=+1, 0xFFFFFFFF si color=-1
    
#ifdef __ARM_NEON
    
    // ── Calcul SIMD des (46+2) 48 cp = (patt[i] ^ mask) - mask ────────────────
    // patt[] est int[48], on charge en int32x4, on XOR+SUB vectoriellement
    
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
    // patt[50..51] sont du padding
    
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
        
        if (stage > 9 && stage < 55) {
            
            eval += RXEvaluation::eval_w0[stage];
            
#ifdef __ARM_NEON
            
            //            // Accumulator declaration — 2 lo/hi pairs to cover 16 lanes
            //            int32x4_t sum_0 = vdupq_n_s32(0);    // lanes  0..3
            //            int32x4_t sum_1 = vdupq_n_s32(0);    // lanes  4..7
            //            int32x4_t sum_2 = vdupq_n_s32(0);    // lanes  8..11
            //            int32x4_t sum_3 = vdupq_n_s32(0);    // lanes 12..15
            
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
                
                //                // 1. Somme (Linéaire) : On garde le widening
                //                int16x8_t v_lo = vld1q_s16(v_ref.data);
                //                int16x8_t v_hi = vld1q_s16(v_ref.data + 8);
                //
                //                sum_0 = vaddw_s16(sum_0, vget_low_s16(v_lo));
                //                sum_1 = vaddw_s16(sum_1, vget_high_s16(v_lo));
                //                sum_2 = vaddw_s16(sum_2, vget_low_s16(v_hi));
                //                sum_3 = vaddw_s16(sum_3, vget_high_s16(v_hi));
                
                
                // OPTIMIZATION: Total accumulated sum remains within the short integer bounds.
                // Ensure this holds true after any changes to the latent vectors.
                sum_lo = vaddq_s16(sum_lo, vld1q_s16(v_ref.data));
                sum_hi = vaddq_s16(sum_hi, vld1q_s16(v_ref.data + 8));
                
                
                // --- Partie Carrés (Interaction) ---
                // On charge les carrés précalculés directement dans les accumulateurs
                // Plus besoin de vget_low/high ni de vmlal !
                sumsq_0 = vaddq_s32(sumsq_0, vld1q_s32(&v_ref.squares[0]));
                sumsq_1 = vaddq_s32(sumsq_1, vld1q_s32(&v_ref.squares[4]));
                sumsq_2 = vaddq_s32(sumsq_2, vld1q_s32(&v_ref.squares[8]));
                sumsq_3 = vaddq_s32(sumsq_3, vld1q_s32(&v_ref.squares[12]));
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
            
            const Vec_short* const& vMob_P  = RXEvaluation::gVMob_P;
            const Vec_short* const& vMob_O  = RXEvaluation::gVMob_O;
            const Vec_short* const& vDiag5  = RXEvaluation::gVDiag5;
            const Vec_short* const& vDiag6  = RXEvaluation::gVDiag6;
            const Vec_short* const& vDiag7  = RXEvaluation::gVDiag7;
            const Vec_short* const& vDiag8  = RXEvaluation::gVDiag8;
            const Vec_short* const& vEdge1  = RXEvaluation::gVEdge1;
            const Vec_short* const& vEdge2  = RXEvaluation::gVEdge2;
            const Vec_short* const& vEdge3  = RXEvaluation::gVEdge3;
            const Vec_short* const& vEdge4  = RXEvaluation::gVEdge4;
            const Vec_short* const& vHv2    = RXEvaluation::gVHv2;
            const Vec_short* const& vHv3    = RXEvaluation::gVHv3;
            const Vec_short* const& vHv4    = RXEvaluation::gVHv4;
            const Vec_short* const& vCorner = RXEvaluation::gVCorner;
            
            
            // Remplacement des 4 appels par une version plus "branch-friendly"
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
            
            
#ifdef __ARM_NEON
            
//            // FM = Σ (sum²  - sumsq)
//            int32x4_t res0 = vsubq_s32(vmulq_s32(sum_0, sum_0), sumsq_0);
//            int32x4_t res1 = vsubq_s32(vmulq_s32(sum_1, sum_1), sumsq_1);
//            int32x4_t res2 = vsubq_s32(vmulq_s32(sum_2, sum_2), sumsq_2);
//            int32x4_t res3 = vsubq_s32(vmulq_s32(sum_3, sum_3), sumsq_3);
            
            // OPTIMIZATION: Total accumulated sum remains within the short integer bounds.
            // Ensure this holds true after any changes to the latent vectors.
            // Widening au moment du calcul final seulement

            // Initialise res = -sumsq (une instruction)
            int32x4_t res0 = vnegq_s32(sumsq_0);
            int32x4_t res1 = vnegq_s32(sumsq_1);
            int32x4_t res2 = vnegq_s32(sumsq_2);
            int32x4_t res3 = vnegq_s32(sumsq_3);

            // Puis widening multiply-accumulate depuis int16 (sum est encore int16x8_t)
            // vmlal_s16 : res += (int16 * int16) avec widening automatique
            // Optimisation : vmlal_high_s16 évite les vget_high
            res0 = vmlal_s16     (res0, vget_low_s16(sum_lo), vget_low_s16(sum_lo));
            res1 = vmlal_high_s16(res1, sum_lo, sum_lo);   // ← smlal2
            res2 = vmlal_s16     (res2, vget_low_s16(sum_hi), vget_low_s16(sum_hi));
            res3 = vmlal_high_s16(res3, sum_hi, sum_hi);   // ← smlal2

            int fm_interaction = vaddvq_s32(
                                            vaddq_s32(vaddq_s32(res0, res1),
                                                      vaddq_s32(res2, res3))
                                            );
            
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

#ifdef __ARM_NEON

    uint64x2_t mob  = board.dual_count_legal_moves();
    const int mob_P = std::min(23, (int)vgetq_lane_u64(mob, 0));
    const int mob_O = std::min(23, (int)vgetq_lane_u64(mob, 1));

#else
    
    int mob_P = 0;
    int mob_O = 0;
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

#ifdef __ARM_NEON

    uint64x2_t mob  = RXBitBoard::dual_count_legal_moves(discs_player, discs_opponent);
    const int mob_P = std::min(23, (int)vgetq_lane_u64(mob, 0));
    const int mob_O = std::min(23, (int)vgetq_lane_u64(mob, 1));

#else
    
    int mob_P = 0;
    int mob_O = 0;
    RXBitBoard::dual_count_legal_moves(discs_player, discs_opponent, mob_P, mob_O);

#endif


    return acc_score<UseFM>(stage, color, move.pattern->patt,
                          mob_P, mob_O,
                          RXEvaluation::eval_w[stage],
                          filled);
}


#endif
