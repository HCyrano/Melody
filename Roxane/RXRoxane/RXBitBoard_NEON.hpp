//
//  RXBitBoard_NEON.hpp
//  Roxane
//
//  Created by Causse Bruno on 13/02/2026.
//

#include <bit> // std::rotl


// Strictly, (long long) >> 64 is undefined in C, but either 0 bit (no change)
// or 64 bit (zero out) shift will lead valid result (i.e. flipped == 0).
//#define outflank_right(O,maskr) (0x8000000000000000ULL >> __builtin_clzll(~(O) & (maskr)))

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long outflank_right(const unsigned long long O, const unsigned long long maskr) noexcept {
    //Fixed version without undefined behavior (UB)
    //const unsigned long long masked = ~O & maskr;
    //return (0x8000000000000000ULL >> __builtin_clzll(masked | 1ULL)) & -(masked != 0ULL);

    return 0x8000000000000000ULL >> __builtin_clzll(~O & maskr);
}

// in case continuous from MSB
//#define outflank_right_H(O) (0x80000000u >> __builtin_clz(~(O)))

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned int outflank_right_H(const unsigned int O) noexcept {
    //Fixed version without undefined behavior (UB)
    //const unsigned int masked = ~O;
    //return (0x80000000u >> __builtin_clz(masked | 1u)) & -(masked != 0u);
    
    return 0x80000000u >> __builtin_clz(~O);
}


//#define not_O_in_mask(mask,O)   vbicq_u64((mask), vdupq_n_u64(O))

[[nodiscard]] __attribute__((always_inline))
static constexpr inline uint64x2_t not_O_in_mask(const uint64x2_t mask, const unsigned long long O) noexcept {
    return vbicq_u64(mask, vdupq_n_u64(O));
}

//rotl8
//#define rotl8(static_cast<uint8_t>(x,y)  __builtin_rotateleft8((x),(y))
//
//__attribute__((always_inline))
//static constexpr inline uint8_t rotl8(const uint8_t x, const uint8_t y) {
//    // Le masque (y & 7) est une excellente optimisation pour les architectures 8-bit
//    // ou pour aider l'optimiseur sur ARM/x86.
//    return static_cast<uint8_t>((x << (y & 7)) | (x >> ((8 - y) & 7)));
//}

// C++20: optimal circular shift for 8-bit types
//uint8_t result = std::rotl(static_cast<uint8_t>(P >> 48), 3);

//Clang on Apple Silicon will compile this into a SUBS instruction followed by a CSEL (Conditional Select).
//This is the 'Holy Grail' of ARM optimization: 2 cycles, 0 branches.
//Set all bits below the sole outflank bit if outfrank != 0
[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long OutflankToFlipmask(unsigned long long outflank) noexcept {
    return outflank ? (outflank - 1) : 0;
}




inline unsigned int RXBitBoard::count_stable_edge(const unsigned long long P, const unsigned long long O) {
    return __builtin_popcountll(RXBitBoard::get_stable_edge(P, O));
}

inline int RXBitBoard::get_stability(const unsigned long long discs_player, const unsigned long long discs_opponent) {
    
    const unsigned long long filled = discs_player | discs_opponent;
    const unsigned long long central_mask = discs_player & 0x007e7e7e7e7e7e00ULL;
    
    unsigned long long stable = get_stable_edge(discs_player, discs_opponent);
    
    uint8x8_t h8;
    uint64x2_t l79, r79;
    const uint64x2_t e790 = vdupq_n_u64(0x007f7f7f7f7f7f7f);
    const uint64x2_t e791 = vdupq_n_u64(0xfefefefefefefe00);
    const uint64x2_t e792 = vdupq_n_u64(0x00003f3f3f3f3f3f);
    const uint64x2_t e793 = vdupq_n_u64(0x0f0f0f0ff0f0f0f0);
    
    
    unsigned long long h, v, d7, d9;
    
    h8 = vcreate_u8(filled);                l79 = r79 = vreinterpretq_u64_u8(vcombine_u8(h8, vrev64_u8(h8)));
    h8 = vceq_u8(h8, vdup_n_u8(0xff));      l79 = vandq_u64(l79, vornq_u64(vshrq_n_u64(l79, 9), e790));
    h = vget_lane_u64(vreinterpret_u64_u8(h8), 0);
    r79 = vandq_u64(r79, vornq_u64(vshlq_n_u64(r79, 9), e791));
    v = filled;                             l79 = vbicq_u64(l79, vbicq_u64(e792, vshrq_n_u64(l79, 18)));
    v &= (v >> 8) | (v << 56);              r79 = vbicq_u64(r79, vshlq_n_u64(vbicq_u64(e792, r79), 18));
    v &= (v >> 16) | (v << 48);             l79 = vandq_u64(vandq_u64(l79, r79), vorrq_u64(e793, vsliq_n_u64(vshrq_n_u64(l79, 36), r79, 36)));
    v &= (v >> 32) | (v << 32);             d7 = __builtin_bswap64(vgetq_lane_u64(l79, 1));
    d9 = vgetq_lane_u64(l79, 0);
    
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

__attribute__((always_inline))
inline unsigned long long RXBitBoard::hashcode() const {
    
    const uint16x4_t p_lines = vcreate_u16(discs[player]);
    const uint16x4_t o_lines = vcreate_u16(discs[player^1]);
    
    unsigned long long hashcode  =
      hashcodeTable_lines1_2[PLAYER][vget_lane_u16(p_lines,3)]
    ^ hashcodeTable_lines3_4[PLAYER][vget_lane_u16(p_lines,2)]
    ^ hashcodeTable_lines5_6[PLAYER][vget_lane_u16(p_lines,1)]
    ^ hashcodeTable_lines7_8[PLAYER][vget_lane_u16(p_lines,0)]
    
    ^ hashcodeTable_lines1_2[OPPONENT][vget_lane_u16(o_lines,3)]
    ^ hashcodeTable_lines3_4[OPPONENT][vget_lane_u16(o_lines,2)]
    ^ hashcodeTable_lines5_6[OPPONENT][vget_lane_u16(o_lines,1)]
    ^ hashcodeTable_lines7_8[OPPONENT][vget_lane_u16(o_lines,0)];
    
    return hashcode;
    
}

__attribute__((always_inline))
inline unsigned long long RXBitBoard::hashcode_after_move(RXMove* move) const {
    
    const uint16x4_t p_lines = vcreate_u16(discs[player^1] ^ move->flipped);
    const uint16x4_t o_lines = vcreate_u16(discs[player] | (move->flipped | move->square));
    
    unsigned long long hashcode  =
      hashcodeTable_lines1_2[PLAYER][vget_lane_u16(p_lines,3)]
    ^ hashcodeTable_lines3_4[PLAYER][vget_lane_u16(p_lines,2)]
    ^ hashcodeTable_lines5_6[PLAYER][vget_lane_u16(p_lines,1)]
    ^ hashcodeTable_lines7_8[PLAYER][vget_lane_u16(p_lines,0)]
    
    ^ hashcodeTable_lines1_2[OPPONENT][vget_lane_u16(o_lines,3)]
    ^ hashcodeTable_lines3_4[OPPONENT][vget_lane_u16(o_lines,2)]
    ^ hashcodeTable_lines5_6[OPPONENT][vget_lane_u16(o_lines,1)]
    ^ hashcodeTable_lines7_8[OPPONENT][vget_lane_u16(o_lines,0)];
    
    return hashcode;
    
}


inline unsigned long long RXBitBoard::get_legal_moves(const unsigned long long p_discs, const unsigned long long o_discs) {
    constexpr int64x2_t S_H  = { -1,  1}, S2_H  = { -2,  2}, S4_H  = { -4,  4};
    constexpr int64x2_t S_V  = { -8,  8}, S2_V  = {-16, 16}, S4_V  = {-32, 32};
    constexpr int64x2_t S_D7 = { -7,  7}, S2_D7 = {-14, 14}, S4_D7 = {-28, 28};
    constexpr int64x2_t S_D9 = { -9,  9}, S2_D9 = {-18, 18}, S4_D9 = {-36, 36};
    
    const uint64x2_t P = vdupq_n_u64(p_discs);
    const uint64x2_t O = vdupq_n_u64(o_discs);
    const uint64x2_t O_inner = vdupq_n_u64(o_discs & 0x7E7E7E7E7E7E7E7EULL);
    
    auto kogge_stone_step = [](uint64x2_t& flip, int64x2_t shift, uint64x2_t mask) {
        flip = vorrq_u64(flip, vandq_u64(vshlq_u64(flip, shift), mask));
    };
    
    uint64x2_t fH  = vandq_u64(vshlq_u64(P, S_H),  O_inner);
    uint64x2_t fV  = vandq_u64(vshlq_u64(P, S_V),  O);
    uint64x2_t fD7 = vandq_u64(vshlq_u64(P, S_D7), O_inner);
    uint64x2_t fD9 = vandq_u64(vshlq_u64(P, S_D9), O_inner);
    
    const uint64x2_t aH  = vandq_u64(O_inner, vshlq_u64(O_inner, S_H));
    const uint64x2_t aV  = vandq_u64(O, vshlq_u64(O, S_V));
    const uint64x2_t aD7 = vandq_u64(O_inner, vshlq_u64(O_inner, S_D7));
    const uint64x2_t aD9 = vandq_u64(O_inner, vshlq_u64(O_inner, S_D9));
    
    const uint64x2_t a2H  = vandq_u64(aH,  vshlq_u64(aH,  S2_H));
    const uint64x2_t a2V  = vandq_u64(aV,  vshlq_u64(aV,  S2_V));
    const uint64x2_t a2D7 = vandq_u64(aD7, vshlq_u64(aD7, S2_D7));
    const uint64x2_t a2D9 = vandq_u64(aD9, vshlq_u64(aD9, S2_D9));
    
    kogge_stone_step(fH,  S_H,  O_inner);
    kogge_stone_step(fV,  S_V,  O);
    kogge_stone_step(fD7, S_D7, O_inner);
    kogge_stone_step(fD9, S_D9, O_inner);
    
    kogge_stone_step(fH,  S2_H,  aH);
    kogge_stone_step(fV,  S2_V,  aV);
    kogge_stone_step(fD7, S2_D7, aD7);
    kogge_stone_step(fD9, S2_D9, aD9);
    
    kogge_stone_step(fH,  S4_H,  a2H);
    kogge_stone_step(fV,  S4_V,  a2V);
    kogge_stone_step(fD7, S4_D7, a2D7);
    kogge_stone_step(fD9, S4_D9, a2D9);
    
    const uint64x2_t legals = vorrq_u64(
        vorrq_u64(vshlq_u64(fH, S_H), vshlq_u64(fV, S_V)),
        vorrq_u64(vshlq_u64(fD7, S_D7), vshlq_u64(fD9, S_D9))
    );
    
    uint64x2_t result = vorrq_u64(legals, vextq_u64(legals, legals, 1));
    return (vgetq_lane_u64(result, 0) & ~(p_discs | o_discs));
}


template<int Shift>
__attribute__((always_inline))
inline uint64x2_t vshift_u64(uint64x2_t v) {
    if constexpr (Shift > 0)
        return vshlq_n_u64(v, Shift);          // logical left shift ✓
    else
        return vshrq_n_u64(v, -Shift);         // logical right shift ✓
}

template<int Shift, bool IsHorizontal>
__attribute__((always_inline))
inline uint64x2_t propagate_kogge_stone(
        const uint64x2_t p_vec,
        const uint64x2_t o_vec,
        const uint64x2_t mask_inner)
{
    const uint64x2_t mask = IsHorizontal
        ? mask_inner
        : vdupq_n_u64(0xFFFFFFFFFFFFFFFFULL);
    const uint64x2_t prop = vandq_u64(o_vec, mask);

    // Initialisation
    uint64x2_t g = vandq_u64(vshift_u64<Shift>(p_vec), prop);

    // Pré-calcul des shifts de prop en parallèle (ILP maximal)
    const uint64x2_t g1      = vshift_u64<  Shift>(g);
    const uint64x2_t prop1   = vshift_u64<  Shift>(prop);
    const uint64x2_t prop2   = vandq_u64(prop, prop1);
    const uint64x2_t prop2s  = vshift_u64<2*Shift>(prop2);

    // Chaîne d'accumulation (dépendances inévitables)
    g = vorrq_u64(g, vandq_u64(g1, prop));

    const uint64x2_t g2    = vshift_u64<2*Shift>(g);
    const uint64x2_t prop4 = vandq_u64(prop2, prop2s);
    g = vorrq_u64(g, vandq_u64(g2, prop2));

    const uint64x2_t g4 = vshift_u64<4*Shift>(g);
    g = vorrq_u64(g, vandq_u64(g4, prop4));

    return vshift_u64<Shift>(g);
}

// ============================================================
// OPTIMISATION 1 : 8 directions → 4 paires ORées
// Expose plus d'ILP au pipeline OoO du M4 (issue width = 4)
// Le compilateur peut superposer les 2 Kogge-Stone d'une paire
// ============================================================
inline uint64x2_t RXBitBoard::dual_legal_moves(
        const unsigned long long p,
        const unsigned long long o)
{
    const uint64x2_t p_vec  = {p, o};
    const uint64x2_t o_vec  = {o, p};
    const uint64x2_t mask_h = vdupq_n_u64(0x7E7E7E7E7E7E7E7EULL);

    // 4 paires indépendantes → le compilateur les superpose
    const uint64x2_t l_NS = vorrq_u64(
        propagate_kogge_stone< 8, false>(p_vec, o_vec, mask_h),  // N
        propagate_kogge_stone<-8, false>(p_vec, o_vec, mask_h)); // S

    const uint64x2_t l_EW = vorrq_u64(
        propagate_kogge_stone< 1, true>(p_vec, o_vec, mask_h),   // E
        propagate_kogge_stone<-1, true>(p_vec, o_vec, mask_h));  // W

    const uint64x2_t l_NE_SW = vorrq_u64(
        propagate_kogge_stone< 7, true>(p_vec, o_vec, mask_h),   // NE
        propagate_kogge_stone<-7, true>(p_vec, o_vec, mask_h));  // SW

    const uint64x2_t l_NW_SE = vorrq_u64(
        propagate_kogge_stone< 9, true>(p_vec, o_vec, mask_h),   // NW
        propagate_kogge_stone<-9, true>(p_vec, o_vec, mask_h));  // SE

    // Arbre de OR équilibré (profondeur 2 au lieu de 7)
    const uint64x2_t legals = vorrq_u64(
        vorrq_u64(l_NS, l_EW),
        vorrq_u64(l_NE_SW, l_NW_SE));

    return vbicq_u64(legals, vdupq_n_u64(p | o));
}

inline uint64x2_t RXBitBoard::dual_count_legal_moves() const {
    return dual_count_legal_moves(discs[player], discs[player^1]);
}

inline uint64x2_t RXBitBoard::dual_count_legal_moves(
        const unsigned long long p,
        const unsigned long long o)
{
    const uint64x2_t legals = dual_legal_moves(p, o);

    // Popcount NEON : canonique et optimal
    const uint8x16_t  cnt8  = vcntq_u8(vreinterpretq_u8_u64(legals));
    const uint16x8_t  sum16 = vpaddlq_u8(cnt8);
    const uint32x4_t  sum32 = vpaddlq_u16(sum16);
    return vpaddlq_u32(sum32);
}


__attribute__((always_inline))
inline int RXBitBoard::count_flips(const int pos, const unsigned long long P) const
{
    unsigned int    n_flips;
    const unsigned char *COUNT_FLIP_X = COUNT_FLIP[pos & 7];
    const unsigned char *COUNT_FLIP_Y = COUNT_FLIP[pos >> 3];
    uint64x2_t    PP = vdupq_n_u64(P);
    uint64x2_t    II;
    unsigned int t;
    const uint64x2_t dmask = { 0x0808040402020101, 0x8080404020201010 };

    PP = vreinterpretq_u64_u8(vzip1q_u8(vreinterpretq_u8_u64(PP), vreinterpretq_u8_u64(PP)));
    II = vandq_u64(PP, mask_dvhd[pos][0]);    // 2 dirs interleaved
    t = vaddvq_u16(vreinterpretq_u16_u64(II));
    n_flips  = COUNT_FLIP_X[t >> 8];
    n_flips += COUNT_FLIP_X[t & 0xFF];
    II = vandq_u64(vreinterpretq_u64_u8(vtstq_u8(vreinterpretq_u8_u64(PP), vreinterpretq_u8_u64(mask_dvhd[pos][1]))), dmask);
    t = vaddvq_u16(vreinterpretq_u16_u64(II));
    n_flips += COUNT_FLIP_Y[t >> 8];
    n_flips += COUNT_FLIP_Y[t & 0xFF];

    return n_flips;
}


//unroll
inline int RXBitBoard::final_score_2(const unsigned long long discs_player, const unsigned long long discs_opponent, const int alpha, const int beta, const int idSquare1, const int idSquare2) const {
    
    unsigned long long flipped, n_player, n_opponent;
    
    int n_flips, bestscore = UNDEF_SCORE;
    ++n_nodes;
    
    // try to play on the first available square
    if((discs_opponent & NEIGHBOR[idSquare1]) && (flipped = do_flips[idSquare1](discs_player, discs_opponent))) {
        ++n_nodes;
        
        n_opponent = discs_opponent ^ flipped;
        
        bestscore = 62 - 2*__builtin_popcountll(n_opponent);
        
        n_flips = count_flips(idSquare2, n_opponent);
        if(n_flips !=0) {
            bestscore -= n_flips;
        } else {
            
            if(bestscore >= 0) {
                bestscore += 2;
                if(bestscore < beta) {
                    bestscore += count_flips(idSquare2, ~n_opponent);
                }
            } else {
                if(bestscore < beta) {
                    n_flips = count_flips(idSquare2, ~n_opponent);
                    
                    if(n_flips != 0)
                        bestscore += n_flips + 2;
                }
            }
        }
        
        if(bestscore >= beta)
            return bestscore;
        
    }
    
    // if needed, try to play on the second & last available square
    if((discs_opponent & NEIGHBOR[idSquare2]) && (flipped = do_flips[idSquare2](discs_player, discs_opponent))) {
        ++n_nodes;
        
        n_opponent = discs_opponent ^ flipped;
        
        int score = 62 - 2*__builtin_popcountll(n_opponent);
        
        n_flips = count_flips(idSquare1, n_opponent);
        if(n_flips !=0) {
            score -= n_flips;
        } else {
            
            if(score >= 0) {
                score += 2;
                if(score < beta) {
                    score += count_flips(idSquare1, ~n_opponent);
                }
            } else {
                if(score < beta) {
                    n_flips = count_flips(idSquare1, ~n_opponent);
                    if(n_flips != 0)
                        score += n_flips + 2;
                }
            }
        }
        
        if(score > bestscore)
            return score;
        
        return bestscore;
        
    }
    
    // if no move were available
    if(bestscore == UNDEF_SCORE) {
        
        if((flipped = do_flips[idSquare1](discs_opponent, discs_player))) {
            ++n_nodes;
            
            n_player = discs_player ^ flipped;
            
            bestscore = 62 - 2*__builtin_popcountll(n_player);
            
            n_flips = count_flips(idSquare2, n_player);
            if(n_flips !=0) {
                bestscore -= n_flips;
            } else {
                
                if(bestscore >= 0) {
                    bestscore += 2;
                    if(bestscore < -alpha) {
                        bestscore += count_flips(idSquare2, ~n_player);
                    }
                } else {
                    if(bestscore < -alpha) {
                        n_flips = count_flips(idSquare2, ~n_player);
                        
                        if(n_flips != 0)
                            bestscore += n_flips + 2;
                    }
                }
            }
            
            if(bestscore >= -alpha)
                return -bestscore;
            
            
        }
        
        // if needed, try to play on the second & last available square
        if((flipped = do_flips[idSquare2](discs_opponent, discs_player))) {
            ++n_nodes;
            
            n_player = discs_player ^ flipped;
            
            int score = 62 - 2*__builtin_popcountll(n_player);
            
            n_flips = count_flips(idSquare1, n_player);
            if(n_flips !=0) {
                score -= n_flips;
            } else {
                
                if(score >= 0) {
                    score += 2;
                    if(score < -alpha) {
                        score += count_flips(idSquare1, ~n_player);
                    }
                } else {
                    if(score < -alpha) {
                        n_flips = count_flips(idSquare1, ~n_player);
                        
                        if(n_flips != 0)
                            score += n_flips + 2;
                    }
                }
            }
            
            if(score > bestscore)
                return -score;
            
        }
        
        if(bestscore == UNDEF_SCORE) {
            
            bestscore = 62 - 2*__builtin_popcountll(discs_player);
            if(bestscore>0)
                bestscore+=2;
            else if (bestscore<0)
                bestscore-=2;
        }
        
        return -bestscore;
        
    }
    
    return bestscore;
}
