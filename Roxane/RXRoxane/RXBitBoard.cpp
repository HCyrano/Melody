/*
 *  RXBitBoard.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 27/06/05.
 *  Copyleft 2005-2025 personnel.
 *
 */

#include "RXBitBoard.hpp"
#include "RXEvaluation.hpp"
#include "RXHashTable.hpp"

#include <cstddef> // define nullptr
#include <iomanip>


const unsigned long long RXBitBoard::hashSquare[64][2] = {
    
    {0X5F4871C661CF2338ULL,  0X33FEA1852435F281ULL},    //H8
    {0XB0CD851861F3B7B6ULL,  0XAD2822A305818B27ULL},
    {0XF71AA37EC76E7E58ULL,  0X790B918AF5599202ULL},
    {0XF899D4E5A6A89530ULL,  0X8B6C247FA77ECE8DULL},
    {0X076CEBC185343BB6ULL,  0XAC75D5F3EEAC23EDULL},
    {0XEE4FFAA05000D7A3ULL,  0XFB0397FEE53662B8ULL},
    {0X339D774EDFF65CDCULL,  0X8DF5C2DF4A2A15B4ULL},
    {0X2C89DA6AD9CF1CE1ULL,  0X4BEB70B395691171ULL},    //A8
    
    {0X96F1CBC65D7D8FD0ULL,  0XA607EB34997673B4ULL},    //H7
    {0XF484677142134734ULL,  0X3CAFC19BD170E9B1ULL},
    {0X0DBAAFDBEE9F073CULL,  0X55FB5E5E7C47E4C4ULL},
    {0X8B9289F7BDFA0A8AULL,  0XA47C8B4039BCB0AAULL},
    {0XEBBC7CB71EB476C1ULL,  0X7B37BFCED13DA3C5ULL},
    {0X06995AE685421F11ULL,  0X1A85D295BD09A404ULL},
    {0X1A7A170899D82293ULL,  0X6A7D78272C960D12ULL},
    {0XBDD43971341B3DD5ULL,  0X0B43C7CF79974871ULL},    //A7
    
    {0XF4FD8BED713AE0E8ULL,  0X316CCD2549DAB6C9ULL},    //H6
    {0X2F58A940D4150E0AULL,  0X39E06B75DE5CE056ULL},
    {0X06AB4E930C3C3B4CULL,  0X5FD90096024DF466ULL},
    {0XE141127AFF5D9D88ULL,  0X10D8EAC48C06DE04ULL},
    {0X0B23D602B5E730EAULL,  0XD3B38CA27A921409ULL},
    {0XF70DDB51ECA37304ULL,  0XD6EC1B708B88569BULL},
    {0XF8F082FDDBDC088FULL,  0X5FAF128D15EA6EC5ULL},
    {0X91FAB6AA406CD6A5ULL,  0XBA20CE704CD47BF8ULL},    //A6
    
    {0X20578A4BAF901694ULL,  0X870EE4BE453C62DCULL},    //H5
    {0X0857FDCD16047C54ULL,  0X05EFC5AC69AB23A9ULL},
    {0XE9805D61927900E1ULL,  0XB9DB7BDA202AF62DULL},
    {0XC166DDDA59B65540ULL,  0X0F17EFBCE39044FCULL},
    {0X3E6B68BDE78C5A9DULL,  0XE6E96506B43E6FB4ULL},
    {0XC40E3C62E1FEA4BCULL,  0X3A978668A91214FCULL},
    {0XB04308D12EEC231FULL,  0X1EAF06CA86DAF90EULL},
    {0X2701859A5F3AF238ULL,  0X53BC2008F2D25753ULL},    //A5
    
    {0X3337CBDC99AA5E0AULL,  0X5017DC3A9B25A0BAULL},    //H4
    {0XBBD04C6669A079B3ULL,  0XF5F2A2476242DC67ULL},
    {0XAD14C7E539D51489ULL,  0X628EB79D013AED72ULL},
    {0XB9BA6F83E508A4C5ULL,  0XE1B6AFE2052E6185ULL},
    {0X62085D596EDD05D2ULL,  0XB675DDD7E48FDDCEULL},
    {0XFA4AAF1082BF8F7FULL,  0XF40449D76ACF7033ULL},
    {0X7F57A1BC6CC0C6A5ULL,  0X3B7B27D177417115ULL},
    {0X43F10D75AF82EBDAULL,  0X46167C438D560803ULL},    //A4
    
    {0XB73929FE1D7104E6ULL,  0XA9755943D41419CCULL},    //H3
    {0X4302A6DC441AD82FULL,  0XE1459CBBB4C45326ULL},
    {0X3B588BDB6258EA64ULL,  0X46CF4DFA45B61335ULL},
    {0XCD6BD259CC4531BBULL,  0X617252D2AC6A3AD3ULL},
    {0X76CC6FE07CF98A2CULL,  0XCA21EC1086C9E72DULL},
    {0X024BD24533582E8BULL,  0X1647A1749D5B9850ULL},
    {0X90B88D84F01528F6ULL,  0X4D6A926EE41512F5ULL},
    {0X457499DA8421ACB2ULL,  0X5933951DAE4A3B3AULL},    //A3
    
    {0XB8844A364A8FD77AULL,  0XDFCA3250484C75AFULL},    //H2
    {0X73580BBFB1F518E3ULL,  0X4921EF8710445B6EULL},
    {0X0D521A9B644685DEULL,  0X8D3331D1D88D7DC2ULL},
    {0X0A1AA93994AA5A9EULL,  0X7E54706B26656D15ULL},
    {0X619F1FAEC1E82DE0ULL,  0XB9594228BEF7E84CULL},
    {0X96F29C5A9B6FD15EULL,  0X8C9205ABE0CB15FAULL},
    {0X668131FA5C735273ULL,  0X27F2A9E3DF582978ULL},
    {0X60DE7E0E3A76FDF0ULL,  0X4397F9C76A4D8E62ULL},    //A2
    
    {0XBA0543A47E6FF2A9ULL,  0XA96D02C2645A0456ULL},    //H1
    {0X9F8AA3178F2B1394ULL,  0XCDA4D30C92EC5F45ULL},
    {0X03CCD1127F12F62DULL,  0X0C2CF1A1C4AD6D13ULL},
    {0XAE6327AFFB44559EULL,  0X41A36D679A564A91ULL},
    {0X5D1A65EE28CACBF8ULL,  0X996955F15F1650CBULL},
    {0X6DE130CFBD5E4B03ULL,  0X3DF572416D1530A5ULL},
    {0X008F6EF4453495A8ULL,  0X6A1AD5C0D79E1AE4ULL},
    {0XC79BFEF60AF9617CULL,  0X0429197C2FC669EEULL},    //A1
};




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


alignas(64) unsigned long long RXBitBoard::hashByte[2][8][256];


unsigned char RXBitBoard::EDGE_STABILITY[256*256]; //unsigned char



#ifdef __ARM_NEON

/** rotated outflank array (indexed with inner 6 bits) */
alignas(64) const unsigned char RXBitBoard::OUTFLANK_3[64] = {    // ...bahgf
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x11, 0x09, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x12, 0x0a, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x11, 0x09, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x14, 0x0c, 0x00, 0x00, 0x00, 0x00
};

alignas(64) const unsigned char RXBitBoard::OUTFLANK_4[64] = {    // ...cbahg
    0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x11, 0x11, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x02, 0x02, 0x02, 0x12, 0x12, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/** flip array (indexed with rotated outflank) */
alignas(64) const unsigned long long RXBitBoard::FLIPPED_3_H[21] = {    // ...bahgf
    0x0000000000000000, 0x1010101010101010, 0x3030303030303030, 0x0000000000000000,
    0x7070707070707070, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0606060606060606, 0x1616161616161616, 0x3636363636363636, 0x0000000000000000,
    0x7676767676767676, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0404040404040404, 0x1414141414141414, 0x3434343434343434, 0x0000000000000000,
    0x7474747474747474
};

alignas(64) const unsigned long long RXBitBoard::FLIPPED_4_H[19] = {    // ...cbahg
    0x0000000000000000, 0x2020202020202020, 0x6060606060606060, 0x0000000000000000,
    0x0e0e0e0e0e0e0e0e, 0x2e2e2e2e2e2e2e2e, 0x6e6e6e6e6e6e6e6e, 0x0000000000000000,
    0x0c0c0c0c0c0c0c0c, 0x2c2c2c2c2c2c2c2c, 0x6c6c6c6c6c6c6c6c, 0x0000000000000000,
    0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
    0x0808080808080808, 0x2828282828282828, 0x6868686868686868
};

#endif




void RXBitBoard::init_hashcodeTable() {
    for (int p = 0; p < 2; p++) {
        for (int i = 0; i < 8; i++) { // Pour chaque octet (de 0 à 7)
            for (int v = 0; v < 256; v++) { // Pour chaque valeur (0 à 255)
                unsigned long long h = 0;
                for (int bit = 0; bit < 8; bit++) {
                    if (v & (1 << bit)) {
                        // (i * 8) + bit recrée l'index de 0 à 63
                        h ^= hashSquare[(i * 8) + bit][p];
                    }
                }
                hashByte[p][i][v] = h;
            }
        }
    }
}

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
    init_hashcodeTable();
    edge_stability_init();
}


#ifdef __ARM_NEON


//not very efficient
int RXBitBoard::count_potential_moves(const unsigned long long p_discs, const unsigned long long o_discs) {
        
    //use tab for speed
    static const uint64x2_t mask[] = {  {0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL},
                                        {0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL}};
    
    static const int64x2_t shift[] = {{ 1, 8}, {-1,-8}, { 7, 9}, {-7,-9}};
    
    const uint64x2_t oo = vdupq_n_u64(o_discs);
    
    uint64x2_t hv = vandq_u64(oo, mask[0]);
    hv = vorrq_u64(vshlq_u64(hv, shift[0]),vshlq_u64(hv, shift[1])) ;
    
    uint64x2_t dg = vandq_u64(oo,  mask[1]);
    dg = vorrq_u64(vshlq_u64(dg, shift[2]),vshlq_u64(dg, shift[3])) ;
    
    hv = vorrq_u64(hv, dg);
        
    return __builtin_popcountll((vgetq_lane_u64(hv, 0) | vgetq_lane_u64(hv, 1)) & ~(p_discs|o_discs));

}

void RXBitBoard::dual_potential_mobility(const unsigned long long p_discs, const unsigned long long o_discs, int &p_pmob, int &o_pmob) {
    uint64x2_t opp_pair = {o_discs, p_discs};
    uint64x2_t occupied = vdupq_n_u64(p_discs | o_discs);
    
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
    p_pmob = __builtin_popcountll(vgetq_lane_u64(final_pot, 0));
    o_pmob = __builtin_popcountll(vgetq_lane_u64(final_pot, 1));
}

int RXBitBoard::final_score_1 () const {
    int score = 63-2*__builtin_popcountll(discs[player^1]);
    
    const int pos = empties_list->next->position;
    int nFlips;
    
    if((nFlips = count_flips(pos, discs[player]))>0) {
        score += nFlips+1;
    } else if((nFlips = count_flips(pos, discs[player^1]))>0) {
        score -= nFlips+1;
    } else if(score<0)
        --score;
    else if (score>0)
        ++score;
    
    return score;
}


#else

int RXBitBoard::count_potential_moves(const unsigned long long p_discs, const unsigned long long o_discs) {

    unsigned long long
    inner_opp = o_discs & 0x7E7E7E7E7E7E7E7EULL;
    unsigned long long
    pot_moves = (inner_opp << 1 | inner_opp >> 1);

    inner_opp = o_discs & 0x00FFFFFFFFFFFF00ULL;
    pot_moves |= (inner_opp << 8 | inner_opp >> 8);

    inner_opp = o_discs & 0x007E7E7E7E7E7E00ULL;
    pot_moves |= (inner_opp << 7 | inner_opp >> 7);
    pot_moves |= (inner_opp << 9 | inner_opp >> 9);
    
    pot_moves &= ~(p_discs|o_discs);
    
    return __builtin_popcountll(pot_moves);

}

int RXBitBoard::final_score_1 () const {
    int score = 63-2*__builtin_popcountll(discs[player^1]);
    
    const int pos = empties_list->next->position;
    int nFlips;
    
    if((nFlips = count_flips[pos](discs[player]))>0) {
        score += nFlips+1;
    } else if((nFlips = count_flips[pos](discs[player^1]))>0) {
        score -= nFlips+1;
    } else if(score<0)
        --score;
    else if (score>0)
        ++score;
    
    return score;
}


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
    
    
    init_generate_flips();
    

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



RXBitBoard::RXBitBoard(const RXBitBoard& src) {
	
	discs[BLACK] = src.discs[BLACK];
	discs[WHITE] = src.discs[WHITE];
	
	player = src.player;
	n_empty = src.n_empty;
    parity = src.parity;
	n_nodes = src.n_nodes;
	
	/* create emptiesList */
	RXSquareList* iEmpties = empties_list;	//empties[0]
	iEmpties->position = NOMOVE;			//sentinel
	iEmpties->previous = nullptr;				//nullptr
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
	iEmpties->position = NOMOVE;			//sentinel
	iEmpties->previous = iEmpties - 1; 
	iEmpties->next = 0;						//nullptr
	
	/*BE CAREFULL*/
	/*copy actual empties list */
	RXSquareList* previous = empties_list;
	for(RXSquareList* empties = src.empties_list->next; empties->position != NOMOVE; empties = empties->next) {
		RXSquareList* empty = position_to_empties[empties->position];
		empty->previous = previous;
		previous->next = empty;
		previous = previous->next;
	}
	empties_list[61].previous = previous;
	previous->next = &empties_list[61];
	
	init_generate_flips();
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
			os << "\tNoirs: " << __builtin_popcountll(board.discs[BLACK]) << "\tBlancs: " << __builtin_popcountll(board.discs[WHITE]);
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
        
	int score = __builtin_popcountll(discs[player]) -__builtin_popcountll(discs[player^1]);
	if(score<0)
		score -= n_empty;
	else if(score>0)
		score += n_empty;
		
	return score;
}


unsigned int RXBitBoard::n_moves() const {
    
    const unsigned long long legal_movesBB = get_legal_moves(discs[player], discs[player^1]);
    return __builtin_popcountll(legal_movesBB);
    
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
inline bool dir_valid(unsigned long long square, unsigned long long p_discs, unsigned long long o_discs) {
    unsigned long long x;
    
    if constexpr (Shift > 0) {
        // Shift left (vers le sud/est)
        x = (square << Shift) & Mask & o_discs;
        x |= (x << Shift) & Mask & o_discs;
        x |= (x << (2 * Shift)) & Mask & o_discs;
        x |= (x << (4 * Shift)) & Mask & o_discs;
        return (x << Shift) & Mask & p_discs;
    } else {
        // Shift right (vers le nord/ouest)
        constexpr int S = -Shift;
        x = (square >> S) & Mask & o_discs;
        x |= (x >> S) & Mask & o_discs;
        x |= (x >> (2 * S)) & Mask & o_discs;
        x |= (x >> (4 * S)) & Mask & o_discs;
        return (x >> S) & Mask & p_discs;
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
        
        const unsigned long long p_discs = discs[player];
        const unsigned long long o_discs = discs[player^1];
        
        
        if ((p_discs | o_discs) & square) return false; // case occupée
        
        
        return
            dir_valid< 1, MASK_LEFT >(square, p_discs, o_discs) ||  // Est
            dir_valid<-1, MASK_RIGHT>(square, p_discs, o_discs) ||  // Ouest
            dir_valid< 8, MASK_ALL  >(square, p_discs, o_discs) ||  // Sud
            dir_valid<-8, MASK_ALL  >(square, p_discs, o_discs) ||  // Nord
            dir_valid< 9, MASK_LEFT >(square, p_discs, o_discs) ||  // Sud-Est
            dir_valid<-9, MASK_RIGHT>(square, p_discs, o_discs) ||  // Nord-Ouest
            dir_valid< 7, MASK_RIGHT>(square, p_discs, o_discs) ||  // Sud-Ouest
            dir_valid<-7, MASK_LEFT >(square, p_discs, o_discs);    // Nord-Est
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
            std::cout << "\tNoirs: " << __builtin_popcountll(discs[BLACK]) << "\tBlancs: " << __builtin_popcountll(discs[WHITE]);
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
            line +="X";
        } else if((discs[player^1] & _mask) !=0) {
            line += "O";
        } else {
            line += "-";
        }
        
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

