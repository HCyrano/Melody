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

struct NeonBoardCtx {
    uint64x2_t PP, rPP, OO, rOO, one;

    // Constructeur : initialise tout proprement à la création
    inline NeonBoardCtx(uint64_t P, uint64_t O) {
        PP  = vdupq_n_u64(P);
        OO  = vdupq_n_u64(O);
        one = vdupq_n_u64(1);

        // Ton excellente astuce d'inversion SIMD
        uint64x2_t OP  = vzip1q_u64(PP, OO); // [ O | P ]
        uint64x2_t rOP = vreinterpretq_u64_u8(
                            vrev64q_u8(
                                vrbitq_u8(
                                    vreinterpretq_u8_u64(OP)))); // [ rO | rP ]

        rPP = vdupq_lane_u64(vget_low_u64(rOP), 0);  // Duplique rP -> [ rP | rP ]
        rOO = vdupq_lane_u64(vget_high_u64(rOP), 0); // Duplique rO -> [ rO | rO ]
    }

    // Permet d'alterner les rôles Joueur/Adversaire instantanément
    inline void swap_sides() {
        std::swap(PP, OO);
        std::swap(rPP, rOO);
    }
};

// forward declaration - définie dans RXBBDoFlips_NEON.cpp
unsigned long long do_flip_NEON(const NeonBoardCtx *ctx, int pos);

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

inline unsigned long long RXBitBoard::get_legal_moves(const unsigned long long P, const unsigned long long O) {
    
    constexpr int64x2_t S_H  = { -1,  1}, S2_H  = { -2,  2}, S4_H  = { -4,  4};
    constexpr int64x2_t S_V  = { -8,  8}, S2_V  = {-16, 16}, S4_V  = {-32, 32};
    constexpr int64x2_t S_D7 = { -7,  7}, S2_D7 = {-14, 14}, S4_D7 = {-28, 28};
    constexpr int64x2_t S_D9 = { -9,  9}, S2_D9 = {-18, 18}, S4_D9 = {-36, 36};
    
    const uint64x2_t PP = vdupq_n_u64(P);
    const uint64x2_t OO = vdupq_n_u64(O);
    const uint64x2_t occupied = vdupq_n_u64(P | O);
    
    const uint64x2_t OO_inner = vdupq_n_u64(O & 0x7E7E7E7E7E7E7E7EULL);
    
    auto kogge_stone_step = [](uint64x2_t& flip, int64x2_t shift, uint64x2_t mask) {
        flip = vorrq_u64(flip, vandq_u64(vshlq_u64(flip, shift), mask));
    };
    
    uint64x2_t fH  = vandq_u64(vshlq_u64(PP, S_H),  OO_inner);
    uint64x2_t fV  = vandq_u64(vshlq_u64(PP, S_V),  OO);
    uint64x2_t fD7 = vandq_u64(vshlq_u64(PP, S_D7), OO_inner);
    uint64x2_t fD9 = vandq_u64(vshlq_u64(PP, S_D9), OO_inner);
    
    const uint64x2_t aH  = vandq_u64(OO_inner, vshlq_u64(OO_inner, S_H));
    const uint64x2_t aV  = vandq_u64(OO, vshlq_u64(OO, S_V));
    const uint64x2_t aD7 = vandq_u64(OO_inner, vshlq_u64(OO_inner, S_D7));
    const uint64x2_t aD9 = vandq_u64(OO_inner, vshlq_u64(OO_inner, S_D9));
    
    const uint64x2_t a2H  = vandq_u64(aH,  vshlq_u64(aH,  S2_H));
    const uint64x2_t a2V  = vandq_u64(aV,  vshlq_u64(aV,  S2_V));
    const uint64x2_t a2D7 = vandq_u64(aD7, vshlq_u64(aD7, S2_D7));
    const uint64x2_t a2D9 = vandq_u64(aD9, vshlq_u64(aD9, S2_D9));
    
    kogge_stone_step(fH,  S_H,  OO_inner);
    kogge_stone_step(fV,  S_V,  OO);
    kogge_stone_step(fD7, S_D7, OO_inner);
    kogge_stone_step(fD9, S_D9, OO_inner);
    
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
    result = vbicq_u64(result, occupied);  // BIC = AND NOT, instruction native ARM
    return vgetq_lane_u64(result, 0);
}


// ============================================================
// Kogge-Stone helpers — fonctions libres, shift positif
// ============================================================
template<int S>
__attribute__((always_inline))
static inline uint64x2_t ks_pos(uint64x2_t P, uint64x2_t mask)
{
    const uint64x2_t a  = vandq_u64(mask, vshlq_n_u64(mask, S));
    const uint64x2_t a2 = vandq_u64(a,    vshlq_n_u64(a,    S * 2));

    uint64x2_t f = vandq_u64(vshlq_n_u64(P, S), mask);
    f = vorrq_u64(f, vandq_u64(vshlq_n_u64(f, S),     mask));
    f = vorrq_u64(f, vandq_u64(vshlq_n_u64(f, S * 2), a));
    f = vorrq_u64(f, vandq_u64(vshlq_n_u64(f, S * 4), a2));
    return vshlq_n_u64(f, S);
}

// ============================================================
// Kogge-Stone helpers — fonctions libres, shift négatif
// ============================================================
template<int S>
__attribute__((always_inline))
static inline uint64x2_t ks_neg(uint64x2_t P, uint64x2_t mask)
{
    const uint64x2_t a  = vandq_u64(mask, vshrq_n_u64(mask, S));
    const uint64x2_t a2 = vandq_u64(a,    vshrq_n_u64(a,    S * 2));

    uint64x2_t f = vandq_u64(vshrq_n_u64(P, S), mask);
    f = vorrq_u64(f, vandq_u64(vshrq_n_u64(f, S),     mask));
    f = vorrq_u64(f, vandq_u64(vshrq_n_u64(f, S * 2), a));
    f = vorrq_u64(f, vandq_u64(vshrq_n_u64(f, S * 4), a2));
    return vshrq_n_u64(f, S);
}

// ============================================================
// Calcule les coups légaux des deux joueurs en une seule passe
// lane0 = légaux de P
// lane1 = légaux de O
// ============================================================
inline uint64x2_t RXBitBoard::dual_legal_moves(
        const unsigned long long P,
        const unsigned long long O)
{
    // lane0 = noir, lane1 = blanc
    const uint64x2_t PO        = {P, O};
    const uint64x2_t OP        = {O, P};
    const uint64x2_t OO_inner  = vandq_u64(OP, vdupq_n_u64(0x7E7E7E7E7E7E7E7EULL));
    const uint64x2_t occupied = vorrq_u64(PO, OP);

    // 8 chaînes indépendantes — ILP maximal sur les 2 unités SIMD du M4
    const uint64x2_t legals = vorrq_u64(
        vorrq_u64(
            vorrq_u64(ks_pos<1>(PO, OO_inner), ks_neg<1>(PO, OO_inner)),  // E  / W
            vorrq_u64(ks_pos<8>(PO, OP),       ks_neg<8>(PO, OP))),        // N  / S
        vorrq_u64(
            vorrq_u64(ks_pos<7>(PO, OO_inner), ks_neg<7>(PO, OO_inner)),  // NE / SW
            vorrq_u64(ks_pos<9>(PO, OO_inner), ks_neg<9>(PO, OO_inner)))); // NW / SE

    // vbicq_u64 = AND NOT natif ARM (BIC) — masque occupied sans transition GPR
    return vbicq_u64(legals, occupied);
}


inline uint64x2_t RXBitBoard::dual_count_legal_moves() const {
    return dual_count_legal_moves(discs[player], discs[player^1]);
}

inline uint64x2_t RXBitBoard::dual_count_legal_moves(
        const unsigned long long P,
        const unsigned long long O)
{
    const uint64x2_t legals = dual_legal_moves(P, O);

    // Popcount NEON : canonique et optimal
    const uint8x16_t  cnt8  = vcntq_u8(vreinterpretq_u8_u64(legals));
    const uint16x8_t  sum16 = vpaddlq_u8(cnt8);
    const uint32x4_t  sum32 = vpaddlq_u16(sum16);
    return vpaddlq_u32(sum32);
}



