//
//  RXBBGenerateFlips_NEON.cpp
//  Roxane
//  adaptation flip_neon_bitscan from Edax
//  Created by Causse Bruno on 15/08/2024.
//  Copyleft © 2025 personnel.
//
#include "RXSetting.hpp"

#if ARCH == ARCH_ARM_NEON

#include <stdio.h>

#include "RXBitBoard.hpp"
#include "RXTools.hpp"




void RXBitBoard::generate_flips_A1(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0080808080808080) & P;
    flipped  = (outflank_v * -2) & 0x0080808080808080;

    outflank_d9 = outflank_right(O, 0x0040201008040201) & P;
    flipped |= (outflank_d9 * -2) & 0x0040201008040201;

    outflank_h = outflank_right_H((unsigned int) (O >> 57) << 26) & (unsigned int) (P >> 31);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 31;
        
    move.flipped = flipped;
    move.square = 0X8000000000000000ULL;
    move.position = A1;
    
    

}

void RXBitBoard::generate_flips_B1(RXMove& move) const {
    
    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0040404040404040) & P;
    flipped  = (outflank_v * -2) & 0x0040404040404040;

    outflank_d9 = outflank_right(O, 0x0020100804020100) & P;
    flipped |= (outflank_d9 * -2) & 0x0020100804020100;

    outflank_h = outflank_right_H((unsigned int) (O>> 57) << 27) & (unsigned int) (P >> 30);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 30;

    
        
    move.flipped = flipped;
    move.square = 0X4000000000000000ULL;
    move.position = B1;
    
    

}

void RXBitBoard::generate_flips_C1(RXMove& move) const {
    
    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d7;

    outflank_v = outflank_right(O, 0x0020202020202020) & P;
    flipped  = (outflank_v * -2) & 0x0020202020202020;

    outflank_d7 = outflank_right(O, 0x0010080402010000) & P;
    flipped |= (outflank_d7 * -2) & 0x0010080402010000;

    flipped |= (((P << 7) & 0x0040000000000000) | ((P >> 1) & 0x4000000000000000)) & O;

    outflank_h = outflank_right_H((unsigned int) (O >> 57) << 28) & (unsigned int) (P >> 29);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 29;

    move.flipped = flipped;
    move.square = 0X2000000000000000ULL;
    move.position = C1;
    
    

}

void RXBitBoard::generate_flips_D1(RXMove& move) const {
    
    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = outflank_right(O, 0x0010101010101010) & P;
    flipped  = (outflank_v * -2) & 0x0010101010101010;

    outflank_d = OUTFLANK_4[(((unsigned int) (O >> 32) & 0x10284402) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x1028448201000000) * 0x0101010101010101) >> 54;    // hgfed[cbahg]f0dcba...
    flipped |= FLIPPED_4_H[outflank_d] & 0x1028440200000000;    // A4E8H5

    outflank_h = OUTFLANK_4[(O >> 57) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 56), 2);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_4_H[outflank_h] << 56;

    move.flipped = flipped;
    move.square = 0X1000000000000000ULL;
    move.position = D1;
    
    

}

void RXBitBoard::generate_flips_E1(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = outflank_right(O, 0x0008080808080808) & P;
    flipped  = (outflank_v * -2) & 0x0008080808080808;

    outflank_d = OUTFLANK_3[(((unsigned int) (O >> 32) & 0x08142240) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0814224180000000) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]e0cba...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0814224000000000;    // A5D8H4

    outflank_h = OUTFLANK_3[(O >> 57) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 56), 3);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_3_H[outflank_h] << 56;

    
        
    move.flipped = flipped;
    move.square = 0X0800000000000000ULL;
    move.position = E1;
    
    

}

void RXBitBoard::generate_flips_F1(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_h, outflank_d7;

    outflank_v = outflank_right(O, 0x0004040404040404) & P;
    flipped  = (outflank_v * -2) & 0x0004040404040404;

    outflank_d7 = outflank_right(O, 0x0008102040800000) & P;
    flipped |= (outflank_d7 * -2) & 0x0008102040800000;

    flipped |= (((P << 9) & 0x0002000000000000) | ((P << 1) & 0x0200000000000000)) & O;

    outflank_h = (O + 0x0800000000000000) & P;
    flipped |= (outflank_h - (outflank_h >> 8)) & 0x7800000000000000;

    
        
    move.flipped = flipped;
    move.square = 0X0400000000000000ULL;
    move.position = F1;
    
    

}

void RXBitBoard::generate_flips_G1(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_h, outflank_d7;

    outflank_v = outflank_right(O, 0x0002020202020202) & P;
    flipped  = (outflank_v * -2) & 0x0002020202020202;

    outflank_d7 = outflank_right(O, 0x0004081020408000) & P;
    flipped |= (outflank_d7 * -2) & 0x0004081020408000;

    outflank_h = (O + 0x0400000000000000) & P;
    flipped |= (outflank_h - (outflank_h >> 8)) & 0x7c00000000000000;

    
        
    move.flipped = flipped;
    move.square = 0X0200000000000000ULL;
    move.position = G1;
    
    

}

void RXBitBoard::generate_flips_H1(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_h, outflank_d7;

    outflank_v = outflank_right(O, 0x0001010101010101) & P;
    flipped  = (outflank_v * -2) & 0x0001010101010101;

    outflank_d7 = outflank_right(O, 0x0002040810204080) & P;
    flipped |= (outflank_d7 * -2) & 0x0002040810204080;

    outflank_h = (O + 0x0200000000000000) & P;
    flipped |= (outflank_h - (outflank_h >> 8)) & 0x7e00000000000000;

    move.flipped = flipped;
    move.square = 0X0100000000000000ULL;
    move.position = H1;
    
    

}

void RXBitBoard::generate_flips_A2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0000808080808080) & P;
    flipped  = (outflank_v * -2) & 0x0000808080808080;

    outflank_d9 = outflank_right(O, 0x0000402010080402) & P;
    flipped |= (outflank_d9 * -2) & 0x0000402010080402;

    outflank_h = outflank_right_H((unsigned int) (O>> 49) << 26) & (unsigned int) (P >> 23);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 23;

    
        
    move.flipped = flipped;
    move.square = 0X0080000000000000ULL;
    move.position = A2;
    
    

}

void RXBitBoard::generate_flips_B2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0000404040404040) & P;
    flipped  = (outflank_v * -2) & 0x0000404040404040;

    outflank_d9 = outflank_right(O, 0x0000201008040201) & P;
    flipped |= (outflank_d9 * -2) & 0x0000201008040201;

    outflank_h = outflank_right_H((unsigned int) (O >> 49) << 27) & (unsigned int) (P >> 22);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 22;

    
        
    move.flipped = flipped;
    move.square = 0X0040000000000000ULL;
    move.position = B2;
    
    

}

void RXBitBoard::generate_flips_C2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d7;

    outflank_v = outflank_right(O, 0x0000202020202020) & P;
    flipped  = (outflank_v * -2) & 0x0000202020202020;

    outflank_d7 = outflank_right(O, 0x0000100804020100) & P;
    flipped |= (outflank_d7 * -2) & 0x0000100804020100;

    flipped |= (((P << 7) & 0x0000400000000000) | ((P >> 1) & 0x0040000000000000)) & O;

    outflank_h = outflank_right_H((unsigned int) (O >> 49) << 28) & (unsigned int) (P >> 21);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 21;

    move.flipped = flipped;
    move.square = 0X0020000000000000ULL;
    move.position = C2;
    
    

}

void RXBitBoard::generate_flips_D2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = outflank_right(O, 0x0000101010101010) & P;
    flipped  = (outflank_v * -2) & 0x0000101010101010;

    outflank_d = OUTFLANK_4[(((unsigned int) (O >> 24) & 0x10284402) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0010284482010000) * 0x0101010101010101) >> 54;    // hgfed[cbahg]...
    flipped |= FLIPPED_4_H[outflank_d] & 0x0010284402000000;    // A3E7H4

    outflank_h = OUTFLANK_4[(O >> 49) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 48), 2);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_4_H[outflank_h] << 48;


    
        
    move.flipped = flipped;
    move.square = 0X0010000000000000ULL;
    move.position = D2;
    
    

}

void RXBitBoard::generate_flips_E2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = outflank_right(O, 0x0000080808080808) & P;
    flipped  = (outflank_v * -2) & 0x0000080808080808;

    outflank_d = OUTFLANK_3[(((unsigned int) (O >> 24) & 0x08142240) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0008142241800000) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0008142240000000;    // A4D7H3

    outflank_h = OUTFLANK_3[(O >> 49) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 48), 3);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_3_H[outflank_h] << 48;

    
        
    move.flipped = flipped;
    move.square = 0X0008000000000000ULL;
    move.position = E2;
    
    

}

void RXBitBoard::generate_flips_F2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_d7;
    unsigned int outflank_h;

    outflank_v = outflank_right(O, 0x0000040404040404) & P;
    flipped  = (outflank_v * -2) & 0x0000040404040404;

    outflank_d7 = outflank_right(O, 0x0000081020408000) & P;
    flipped |= (outflank_d7 * -2) & 0x0000081020408000;

    flipped |= (((P << 9) & 0x0000020000000000) | ((P << 1) & 0x0002000000000000)) & O;

    outflank_h = ((unsigned int) (O >> 24) + 0x08000000) & (unsigned int) (P >> 24);
    flipped |= (((unsigned long long) outflank_h << 24) - outflank_h) & 0x0078000000000000;

    
        
    move.flipped = flipped;
    move.square = 0X0004000000000000ULL;
    move.position = F2;
    
    

}

void RXBitBoard::generate_flips_G2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_d7;
    unsigned int outflank_h;

    outflank_v = outflank_right(O, 0x0000020202020202) & P;
    flipped  = (outflank_v * -2) & 0x0000020202020202;

    outflank_d7 = outflank_right(O, 0x0000040810204080) & P;
    flipped |= (outflank_d7 * -2) & 0x0000040810204080;

    outflank_h = ((unsigned int) (O >> 24) + 0x04000000) & (unsigned int) (P >> 24);
    flipped |= (((unsigned long long) outflank_h << 24) - outflank_h) & 0x007c000000000000;

    move.flipped = flipped;
    move.square = 0X0002000000000000ULL;
    move.position = G2;
    
    

}

void RXBitBoard::generate_flips_H2(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned long long flipped, outflank_v, outflank_d7;
    unsigned int outflank_h;

    outflank_v = outflank_right(O, 0x0000010101010101) & P;
    flipped  = (outflank_v * -2) & 0x0000010101010101;

    outflank_d7 = outflank_right(O, 0x0000020408102040) & P;
    flipped |= (outflank_d7 * -2) & 0x0000020408102040;

    outflank_h = ((unsigned int) (O >> 24) + 0x02000000) & (unsigned int) (P >> 24);
    flipped |= (((unsigned long long) outflank_h << 24) - outflank_h) & 0x007e000000000000;

    
        
    move.flipped = flipped;
    move.square = 0X0001000000000000ULL;
    move.position = H2;
    
    

}

void RXBitBoard::generate_flips_A3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0000008080808080) & P;
    flipped  = (outflank_v * -2) & 0x0000008080808080;

    outflank_d9 = outflank_right(O, 0x0000004020100804) & P;
    flipped |= (outflank_d9 * -2) & 0x0000004020100804;

    outflank_h = outflank_right_H((unsigned int) (O >> 41) << 26) & (unsigned int) (P >> 15);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 15;

    flipped |= (((P >> 7) & 0x0040000000000000) | ((P >> 8) & 0x0080000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000800000000000ULL;
    move.position = A3;
    
    

}

void RXBitBoard::generate_flips_B3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0000004040404040) & P;
    flipped  = (outflank_v * -2) & 0x0000004040404040;

    outflank_d9 = outflank_right(O, 0x0000002010080402) & P;
    flipped |= (outflank_d9 * -2) & 0x0000002010080402;

    outflank_h = outflank_right_H((unsigned int) (O >> 41) << 27) & (unsigned int) (P >> 14);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 14;

    flipped |= (((P >> 7) & 0x0020000000000000) | ((P >> 8) & 0x0040000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000400000000000ULL;
    move.position = B3;
    
    

}

void RXBitBoard::generate_flips_C3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d9;

    outflank_v = outflank_right(O, 0x0000002020202020) & P;
    flipped  = (outflank_v * -2) & 0x0000002020202020;

    outflank_d9 = outflank_right(O, 0x0000001008040201) & P;
    flipped |= (outflank_d9 * -2) & 0x0000001008040201;

    outflank_h = outflank_right_H((unsigned int) (O >> 41) << 28) & (unsigned int) (P >> 13);
    flipped |= (unsigned long long) (outflank_h * (unsigned int) -2) << 13;

    flipped |= ((((P >> 7) | (P << 7)) & 0x0010004000000000) | ((P >> 8) & 0x0020000000000000)
        | ((P >> 9) & 0x0040000000000000) | ((P >> 1) & 0x0000400000000000)) & O;

    move.flipped = flipped;
    move.square = 0X0000200000000000ULL;
    move.position = C3;
    
    

}

void RXBitBoard::generate_flips_D3(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d;

    outflank_v = outflank_right(O, 0x0000001010101010) & P;
    flipped  = (outflank_v * -2) & 0x0000001010101010;

    outflank_h = OUTFLANK_4[(O >> 41) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 40), 2);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_4_H[outflank_h] << 40;

    outflank_d = OUTFLANK_4[(((unsigned int) (O >> 16) & 0x10284402) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000102844820100) * 0x0101010101010101) >> 54;    // hgfed[cbahg]...
    flipped |= FLIPPED_4_H[outflank_d] & 0x0000102844020000;    // A2E6H3

    flipped |= (((P >> 9) & 0x0020000000000000) | ((P >> 8) & 0x0010000000000000) | ((P >> 7) & 0x0008000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000100000000000ULL;
    move.position = D3;
    
    

}

void RXBitBoard::generate_flips_E3(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d;

    outflank_v = outflank_right(O, 0x0000000808080808) & P;
    flipped  = (outflank_v * -2) & 0x0000000808080808;

    outflank_h = OUTFLANK_3[(O >> 41) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 40), 3);
    flipped |= (unsigned long long)(unsigned char) FLIPPED_3_H[outflank_h] << 40;

    outflank_d = OUTFLANK_3[(((unsigned int) (O >> 16) & 0x08142240) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000081422418000) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0000081422400000;    // A3D6H2

    flipped |= (((P >> 9) & 0x0010000000000000) | ((P >> 8) & 0x0008000000000000) | ((P >> 7) & 0x0004000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000080000000000ULL;
    move.position = E3;
    
    

}

void RXBitBoard::generate_flips_F3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d7;

    outflank_v = outflank_right(O, 0x0000000404040404) & P;
    flipped  = (outflank_v * -2) & 0x0000000404040404;

    outflank_d7 = outflank_right(O, 0x0000000810204080) & P;
    flipped |= (outflank_d7 * -2) & 0x0000000810204080;

    outflank_h = ((unsigned int) (O >> 16) + 0x08000000) & (unsigned int) (P >> 16);
    flipped |= (((unsigned long long) outflank_h << 16) - outflank_h) & 0x0000780000000000;

    flipped |= ((((P >> 9) | (P << 9)) & 0x0008000200000000) | ((P >> 8) & 0x0004000000000000)
        | ((P >> 7) & 0x0002000000000000) | ((P << 1) & 0x0000020000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000040000000000ULL;
    move.position = F3;
    
    

}

void RXBitBoard::generate_flips_G3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d7;

    outflank_v = outflank_right(O, 0x0000000202020202) & P;
    flipped  = (outflank_v * -2) & 0x0000000202020202;

    outflank_d7 = outflank_right(O, 0x0000000408102040) & P;
    flipped |= (outflank_d7 * -2) & 0x0000000408102040;

    outflank_h = ((unsigned int) (O >> 16) + 0x04000000) & (unsigned int) (P >> 16);
    flipped |= (((unsigned long long) outflank_h << 16) - outflank_h) & 0x00007c0000000000;

    flipped |= (((P >> 8) & 0x0002000000000000) | ((P >> 9) & 0x0004000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000020000000000ULL;
    move.position = G3;
    
    

}

void RXBitBoard::generate_flips_H3(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h;
    unsigned long long flipped, outflank_v, outflank_d7;

    outflank_v = outflank_right(O, 0x0000000101010101) & P;
    flipped  = (outflank_v * -2) & 0x0000000101010101;

    outflank_d7 = outflank_right(O, 0x0000000204081020) & P;
    flipped |= (outflank_d7 * -2) & 0x0000000204081020;

    outflank_h = ((unsigned int) (O >> 16) + 0x02000000) & (unsigned int) (P >> 16);
    flipped |= (((unsigned long long) outflank_h << 16) - outflank_h) & 0x00007e0000000000;

    flipped |= (((P >> 8) & 0x0001000000000000) | ((P >> 9) & 0x0002000000000000)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000010000000000ULL;
    move.position = H3;
    
    

}

void RXBitBoard::generate_flips_A4(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint32x4_t outflankL;
    uint64x2_t flip, outflankH;
    const uint64x2_t maskL = { 0x0000007f80808080, 0x0000000040201008 };
    const uint64x2_t maskH = { 0x8080800000000000, 0x1020400000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vclzq_s32(vreinterpretq_s32_u64(not_O_in_mask(maskL, O)))));
    outflankL = vandq_u32(outflankL, vreinterpretq_u32_u64(PP));
    flip = vandq_u64(maskL, vreinterpretq_u64_s32(vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = not_O_in_mask(maskH, O);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped =vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);
        
    move.flipped = flipped;
    move.square = 0X0000008000000000ULL;
    move.position = A4;
    
    

}

void RXBitBoard::generate_flips_B4(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint32x4_t outflankL;
    uint64x2_t flip, outflankH;
    const uint64x2_t maskL = { 0x0000003f40404040, 0x0000000020100804 };
    const uint64x2_t maskH = { 0x4040400000000000, 0x0810200000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vclzq_s32(vreinterpretq_s32_u64(not_O_in_mask(maskL, O)))));
    outflankL = vandq_u32(outflankL, vreinterpretq_u32_u64(PP));
    flip = vandq_u64(maskL, vreinterpretq_u64_s32(vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = not_O_in_mask(maskH, O);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000004000000000ULL;
    move.position = B4;
    
    

}

void RXBitBoard::generate_flips_C4(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PL = vsetq_lane_u32(vgetq_lane_u32(PP, 0), PP, 3);    // LLHL
    uint32x4_t OL = vsetq_lane_u32(vgetq_lane_u32(OO, 0), OO, 3);
    uint32x4_t outflankL, outflankH, flippedH4, flippedL;
    uint32x2x2_t flippedLH;
    uint64x1_t flip;
    const uint64x2_t maskL = { 0x0000001f20202020, 0x4080000010080402 };
    const uint64x2_t maskH = { 0x000000c020202000, 0x0408100000804000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flippedL = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), vtrnq_u32(OO, OO).val[1]);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), vtrnq_u32(PP, PP).val[1]);
    flippedH4 = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrn_u32(vget_high_u32(flippedL), vorr_u32(vget_low_u32(flippedH4), vget_high_u32(flippedH4)));
    flip = vreinterpret_u64_u32(vorr_u32(vorr_u32(flippedLH.val[0], flippedLH.val[1]), vget_low_u32(flippedL)));

    uint64_t flipped = vget_lane_u64(flip, 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000002000000000ULL;
    move.position = C4;
    
    

}

void RXBitBoard::generate_flips_F4(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PL = vsetq_lane_u32(vgetq_lane_u32(PP, 0), PP, 3);    // LLHL
    uint32x4_t OL = vsetq_lane_u32(vgetq_lane_u32(OO, 0), OO, 3);
    uint32x4_t outflankL, outflankH, flippedH4, flippedL;
    uint32x2x2_t flippedLH;
    uint64x1_t flip;
    const uint64x2_t maskL = { 0x0000000304040404, 0x0810204002010000 };
    const uint64x2_t maskH = { 0x000000f804040400, 0x0001020020100800 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flippedL = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), vtrnq_u32(OO, OO).val[1]);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), vtrnq_u32(PP, PP).val[1]);
    flippedH4 = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrn_u32(vget_high_u32(flippedL), vorr_u32(vget_low_u32(flippedH4), vget_high_u32(flippedH4)));
    flip = vreinterpret_u64_u32(vorr_u32(vorr_u32(flippedLH.val[0], flippedLH.val[1]), vget_low_u32(flippedL)));


    uint64_t flipped = vget_lane_u64(flip, 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000400000000ULL;
    move.position = F4;
    
    

}

void RXBitBoard::generate_flips_G4(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PH = vtrnq_u32(PP, PP).val[1];
    uint32x4_t OH = vtrnq_u32(OO, OO).val[1];
    uint32x4_t outflankL, outflankH, flippedL, flippedH;
    uint32x4x2_t flippedLH;
    uint64x2_t flip;
    const uint64x2_t maskL = { 0x0000000002020202, 0x0000000004081020 };
    const uint64x2_t maskH = { 0x02020200000000fc, 0x1008040000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OO))))), PP);
    flippedL = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OH);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PH);
    flippedH = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrnq_u32(flippedL, flippedH);
    flip = vreinterpretq_u64_u32(vorrq_u32(flippedLH.val[0], flippedLH.val[1]));

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000200000000ULL;
    move.position = G4;
    
    

}

void RXBitBoard::generate_flips_H4(RXMove& move) const {

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PH = vtrnq_u32(PP, PP).val[1];
    uint32x4_t OH = vtrnq_u32(OO, OO).val[1];
    uint32x4_t outflankL, outflankH, flippedL, flippedH;
    uint32x4x2_t flippedLH;
    uint64x2_t flip;
    const uint64x2_t maskL = { 0x0000000001010101, 0x0000000002040810 };
    const uint64x2_t maskH = { 0x01010100000000fe, 0x0804020000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OO))))), PP);
    flippedL = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OH);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PH);
    flippedH = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrnq_u32(flippedL, flippedH);
    flip = vreinterpretq_u64_u32(vorrq_u32(flippedLH.val[0], flippedLH.val[1]));

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000100000000ULL;
    move.position = H4;
    
    

}

void RXBitBoard::generate_flips_A5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    uint32x4_t OL = vtrnq_u32(vreinterpretq_u32_u64(OO), vreinterpretq_u32_u64(OO)).val[0];
    uint32x4_t outflankL;
    uint64x2_t outflankH, flip;
    const uint64x2_t maskL = { 0x7f00000000808080, 0x0000000000402010 };
    const uint64x2_t maskH = { 0x8080808000000000, 0x0810204000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flip = vpaddlq_u32(vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL))))));

    outflankH = vbicq_u64(maskH, OO);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);
        
    move.flipped = flipped;
    move.square = 0X0000000080000000ULL;
    move.position = A5;
    
    

}

void RXBitBoard::generate_flips_B5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    uint32x4_t OL = vtrnq_u32(vreinterpretq_u32_u64(OO), vreinterpretq_u32_u64(OO)).val[0];
    uint32x4_t outflankL;
    uint64x2_t outflankH, flip;
    const uint64x2_t maskL = { 0x3f00000000404040, 0x0000000000201008 };
    const uint64x2_t maskH = { 0x4040404000000000, 0x0408102000000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flip = vpaddlq_u32(vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL))))));

    outflankH = vbicq_u64(maskH, OO);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped =  vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000040000000ULL;
    move.position = B5;
    
    

}

void RXBitBoard::generate_flips_C5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PH = vsetq_lane_u32(vgetq_lane_u32(PP, 1), PP, 2);    // HHHL
    uint32x4_t OH = vsetq_lane_u32(vgetq_lane_u32(OO, 1), OO, 2);
    uint32x4_t outflankL, outflankH, flippedL4, flippedH;
    uint32x2x2_t flippedLH;
    uint64x1_t flip;
    const uint64x2_t maskL = { 0x002020201f000000, 0x0040800000100804 };
    const uint64x2_t maskH = { 0x20202020c0000000, 0x0204081000008040 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), vtrnq_u32(OO, OO).val[0])))));
    outflankL = vandq_u32(outflankL, vtrnq_u32(PP, PP).val[0]);
    flippedL4 = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OH);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PH);
    flippedH = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrn_u32(vorr_u32(vget_low_u32(flippedL4), vget_high_u32(flippedL4)), vget_high_u32(flippedH));
    flip = vreinterpret_u64_u32(vorr_u32(vorr_u32(flippedLH.val[0], flippedLH.val[1]), vget_low_u32(flippedH)));

    uint64_t flipped = vget_lane_u64(flip, 0);
        
    move.flipped = flipped;
    move.square = 0X0000000020000000ULL;
    move.position = C5;
    
    

}

void RXBitBoard::generate_flips_F5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint32x4_t PH = vsetq_lane_u32(vgetq_lane_u32(PP, 1), PP, 2);    // HHHL
    uint32x4_t OH = vsetq_lane_u32(vgetq_lane_u32(OO, 1), OO, 2);
    uint32x4_t outflankL, outflankH, flippedL4, flippedH;
    uint32x2x2_t flippedLH;
    uint64x1_t flip;
    const uint64x2_t maskL = { 0x0004040403000000, 0x0008102000020100 };
    const uint64x2_t maskH = { 0x04040404f8000000, 0x0000010240201008 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), vtrnq_u32(OO, OO).val[0])))));
    outflankL = vandq_u32(outflankL, vtrnq_u32(PP, PP).val[0]);
    flippedL4 = vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OH);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PH);
    flippedH = vandq_u32(vreinterpretq_u32_u64(maskH), vqsubq_u32(outflankH, one));

    flippedLH = vtrn_u32(vorr_u32(vget_low_u32(flippedL4), vget_high_u32(flippedL4)), vget_high_u32(flippedH));
    flip = vreinterpret_u64_u32(vorr_u32(vorr_u32(flippedLH.val[0], flippedLH.val[1]), vget_low_u32(flippedH)));

    uint64_t flipped = vget_lane_u64(flip, 0);
        
    move.flipped = flipped;
    move.square = 0X0000000004000000ULL;
    move.position = F5;
    
    

}

void RXBitBoard::generate_flips_G5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint64x2_t flip;
    uint32x4_t outflankL, outflankH;
    const uint64x2_t maskL = { 0x0000000000020202, 0x0000000000040810 };
    const uint64x2_t maskH = { 0x02020202fc000000, 0x2010080400000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OO)))));
    outflankL = vandq_u32(outflankL, PP);
    flip = vandq_u64(maskL, vreinterpretq_u64_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OO);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vreinterpretq_u64_u32(vqsubq_u32(outflankH, one)), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000002000000ULL;
    move.position = G5;
    
    

}

void RXBitBoard::generate_flips_H5(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint32x4_t PP = vreinterpretq_u32_u64(vdupq_n_u64(P));
    uint32x4_t OO = vreinterpretq_u32_u64(vdupq_n_u64(O));
    uint64x2_t flip;
    uint32x4_t outflankL, outflankH;
    const uint64x2_t maskL = { 0x0000000000010101, 0x0000000000020408 };
    const uint64x2_t maskH = { 0x01010101fe000000, 0x1008040200000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint32x4_t one = vdupq_n_u32(1);

    outflankL = vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OO)))));
    outflankL = vandq_u32(outflankL, PP);
    flip = vandq_u64(maskL, vreinterpretq_u64_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL)))));

    outflankH = vbicq_u32(vreinterpretq_u32_u64(maskH), OO);
    outflankH = vandq_u32(vbicq_u32(outflankH, vsubq_u32(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vreinterpretq_u64_u32(vqsubq_u32(outflankH, one)), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);
        
    move.flipped = flipped;
    move.square = 0X0000000001000000ULL;
    move.position = H5;
    
    

}

void RXBitBoard::generate_flips_A6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    uint32x4_t OL = vtrnq_u32(vreinterpretq_u32_u64(OO), vreinterpretq_u32_u64(OO)).val[0];
    uint32x4_t outflankL;
    uint64x2_t outflankH, flip;
    const uint64x2_t maskL = { 0x007f000000008080, 0x0000000000004020 };
    const uint64x2_t maskH = { 0x8080808080000000, 0x0408102040000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flip = vpaddlq_u32(vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL))))));

    outflankH = vbicq_u64(maskH, OO);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000000800000ULL;
    move.position = A6;
    
    

}

void RXBitBoard::generate_flips_B6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    uint32x4_t OL = vtrnq_u32(vreinterpretq_u32_u64(OO), vreinterpretq_u32_u64(OO)).val[0];
    uint32x4_t outflankL;
    uint64x2_t outflankH, flip;
    const uint64x2_t maskL = { 0x003f000000004040, 0x0000000000002010 };
    const uint64x2_t maskH = { 0x4040404040000000, 0x0204081020000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flip = vpaddlq_u32(vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL))))));

    outflankH = vbicq_u64(maskH, OO);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0);

    
        
    move.flipped = flipped;
    move.square = 0X0000000000400000ULL;
    move.position = B6;
    
    

}

void RXBitBoard::generate_flips_C6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    uint32x4_t OL = vtrnq_u32(vreinterpretq_u32_u64(OO), vreinterpretq_u32_u64(OO)).val[0];
    uint32x4_t outflankL;
    uint64x2_t outflankH, flip;
    const uint64x2_t maskL = { 0x001f000000002020, 0x0000408000001008 };
    const uint64x2_t maskH = { 0x2020202020000000, 0x0102040810000000 };
    const uint32x4_t msb = vdupq_n_u32(0x80000000);
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned long long flipped_g3g4;

    outflankL = vandq_u32(vshlq_u32(msb, vnegq_s32(vreinterpretq_s32_u32(
        vclzq_u32(vbicq_u32(vreinterpretq_u32_u64(maskL), OL))))), PL);
    flip = vpaddlq_u32(vandq_u32(vreinterpretq_u32_u64(maskL), vreinterpretq_u32_s32(
        vnegq_s32(vreinterpretq_s32_u32(vaddq_u32(outflankL, outflankL))))));

    outflankH = vbicq_u64(maskH, OO);
    outflankH = vandq_u64(vbicq_u64(outflankH, vsubq_u64(outflankH, one)), PP);
    flip = vbslq_u64(maskH, vqsubq_u64(outflankH, one), flip);

    flipped_g3g4 = (((P >> 9) & 0x0000000040000000) | ((P >> 1) & 0x000000000400000)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_g3g4;
        
    move.flipped = flipped;
    move.square = 0X0000000000200000ULL;
    move.position = C6;
    
    

}

void RXBitBoard::generate_flips_D6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x1010101010000000;
    outflank_v = (outflank_v & -outflank_v) & 0x1010101010000000 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x1010101010000000;

    outflank_h = OUTFLANK_4[(O >> 17) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 16), 2);
    flipped |= (unsigned char) FLIPPED_4_H[outflank_h] << 16;

    outflank_d = OUTFLANK_4[(((unsigned int) (O >> 16) & 0x02442810) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0001824428100000) * 0x0101010101010101) >> 54;    // hgfed[cbahg]...
    flipped |= FLIPPED_4_H[outflank_d] & 0x0000024428100000;    // A7E3H6

    flipped |= (((P << 7) & 0x0000000000002000) | ((P << 8) & 0x000000000001000) | ((P << 9) & 0x000000000000800)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000100000ULL;
    move.position = D6;
    
    

}

void RXBitBoard::generate_flips_E6(RXMove& move) const {
    


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x0808080808000000;
    outflank_v = (outflank_v & -outflank_v) & 0x0808080808000000 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x0808080808000000;

    outflank_h = OUTFLANK_3[(O >> 17) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 16), 3);
    flipped |= (unsigned char) FLIPPED_3_H[outflank_h] << 16;

    outflank_d = OUTFLANK_3[(((unsigned int) (O >> 16) & 0x40221408) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0080412214080000) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0000402214080000;    // A6D3H7

    flipped |= (((P << 7) & 0x0000000000001000) | ((P << 8) & 0x000000000000800) | ((P << 9) & 0x000000000000400)) & O;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000080000ULL;
    move.position = E6;
    
    

}

void RXBitBoard::generate_flips_F6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t PP = vdupq_n_u64(P);
    uint64x2_t OO = vdupq_n_u64(O);
    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x0404040404000000, 0x8040201008000000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;
    // uint32x4_t PL = vtrnq_u32(vreinterpretq_u32_u64(PP), vreinterpretq_u32_u64(PP)).val[0];
    // const int32x4_t shiftL = { 1, 7, 8, 9 };
    // const uint32x4_t maskL = { 0x00020000, 0x00000800, 0x00000400, 0x00000200 };

    outflank_vd = vbicq_u64(mask, OO);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), PP);
    flip = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = ((O & 0x00780000) + 0x00080000) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x00780000;
#if 1 //trick
    flipped_h |= (((P << 9) & 0x00000200) | ((P << 8) & 0x00000400) | ((P << 1) & 0x00020000)
        | (((P >> 7) | (P << 7)) & 0x02000800)) & O;
#else
    flip = vbslq_u64(vpaddlq_u32(vandq_u32(vshlq_u32(PL, shiftL), maskL)), OO, flip);
    flipped_h |= (P >> 7) & 0x02000000 & O;
#endif
    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000040000ULL;
    move.position = F6;
    
    

}

void RXBitBoard::generate_flips_G6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0202020202000000, 0x4020100804000000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = ((O & 0x007c0000) + 0x00040000) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x007c0000;

    flipped_h |= ((((unsigned int) P << 8) & 0x00000200) | (((unsigned int) P << 7) & 0x00000400)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000020000ULL;
    move.position = G6;
    
    

}

void RXBitBoard::generate_flips_H6(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0101010101000000, 0x2010080402000000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = ((O & 0x007e0000) + 0x00020000) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x007e0000;

    flipped_h |= ((((unsigned int) P << 8) & 0x00000100) | (((unsigned int) P << 7) & 0x00000200)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000010000ULL;
    move.position = H6;
    
    

}

void RXBitBoard::generate_flips_A7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x8080808080800000, 0x0204081020400000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H(((unsigned int) O >> 9) << 26) & ((unsigned int) P << 17);
    flipped_h = (outflank_h * (unsigned int) -2) >> 17;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000008000ULL;
    move.position = A7;
    
    

}

void RXBitBoard::generate_flips_B7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x4040404040400000, 0x0102040810200000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H(((unsigned int) O >> 9) << 27) & ((unsigned int) P << 18);
    flipped_h = (outflank_h * (unsigned int) -2) >> 18;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;
        
    move.flipped = flipped;
    move.square = 0X0000000000004000ULL;
    move.position = B7;
    
    

}

void RXBitBoard::generate_flips_C7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x2020202020200000, 0x0001020408100000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H(((unsigned int) O >> 9) << 28) & ((unsigned int) P << 19);
    flipped_h = (outflank_h * (unsigned int) -2) >> 19;

    flipped_h |= ((((unsigned int) P >> 1) & 0x00004000) | (((unsigned int) P >> 9) & 0x00400000)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000002000ULL;
    move.position = C7;
    
    

}

void RXBitBoard::generate_flips_D7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x1010101010100000;
    outflank_v = (outflank_v & -outflank_v) & 0x1010101010100000 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x1010101010100000;

    outflank_d = OUTFLANK_4[(((unsigned int) (O >> 8) & 0x02442810) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000018244281000) * 0x0101010101010101) >> 54;    // hgfed[cbahg]...
    flipped |= FLIPPED_4_H[outflank_d] & 0x0000000244281000;    // A6E2H5

    outflank_h = OUTFLANK_4[(O >> 9) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 8), 2);
    flipped |= (unsigned char) FLIPPED_4_H[outflank_h] << 8;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000001000ULL;
    move.position = D7;
    
    

}

void RXBitBoard::generate_flips_E7(RXMove& move) const {
    

    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x0808080808080000;
    outflank_v = (outflank_v & -outflank_v) & 0x0808080808080000 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x0808080808080000;

    outflank_d = OUTFLANK_3[(((unsigned int) (O >> 8) & 0x40221408) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000804122140800) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0000004022140800;    // A5D2H6

    outflank_h = OUTFLANK_3[(O >> 9) & 0x3f] & std::rotl(static_cast<uint8_t>(P >> 8), 3);
    flipped |= (unsigned char) FLIPPED_3_H[outflank_h] << 8;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000000800ULL;
    move.position = E7;
    
    

}

void RXBitBoard::generate_flips_F7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0404040404040000, 0x0080402010080000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned short) (O + 0x0800) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x7800;

    flipped_h |= ((((unsigned int) P << 1) & 0x00000200) | (((unsigned int) P >> 7) & 0x00020000)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    move.flipped = flipped;
    move.square = 0X0000000000000400ULL;
    move.position = F7;
    
    

}

void RXBitBoard::generate_flips_G7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0202020202020000, 0x8040201008040000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned short) (O + 0x0400) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x7c00;

    uint64_t flipped =  vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;
        
    move.flipped = flipped;
    move.square = 0X0000000000000200ULL;
    move.position = G7;
    
    

}

void RXBitBoard::generate_flips_H7(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0101010101010000, 0x4020100804020000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned short) (O + 0x0200) & P;
    flipped_h = (outflank_h - (outflank_h >> 8)) & 0x7e00;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000000100ULL;
    move.position = H7;
    
    

}

void RXBitBoard::generate_flips_A8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x8080808080808000, 0x0102040810204000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H((unsigned int) O << 25) & ((unsigned int) P << 25);
    flipped_h = (outflank_h * (unsigned int) -2) >> 25;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000000080ULL;
    move.position = A8;
    
    

}

void RXBitBoard::generate_flips_B8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x4040404040404000, 0x0001020408102000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H((unsigned int) O << 26) & ((unsigned int) P << 26);
    flipped_h = (outflank_h * (unsigned int) -2) >> 26;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    
        
    move.flipped = flipped;
    move.square = 0X0000000000000040ULL;
    move.position = B8;
    
    

}

void RXBitBoard::generate_flips_C8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flip;
    const uint64x2_t mask = { 0x2020202020202000, 0x0000010204081000 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flip = vandq_u64(vqsubq_u64(outflank_vd, one), mask);

    outflank_h = outflank_right_H((unsigned int) O << 27) & ((unsigned int) P << 27);
    flipped_h = (outflank_h * (unsigned int) -2) >> 27;

    flipped_h |= ((((unsigned int) P >> 1) & 0x00000040) | (((unsigned int) P >> 9) & 0x00004000)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flip), vget_high_u64(flip)), 0) | flipped_h;

    move.flipped = flipped;
    move.square = 0X0000000000000020ULL;
    move.position = C8;
}

void RXBitBoard::generate_flips_D8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x1010101010101000;
    outflank_v = (outflank_v & -outflank_v) & 0x1010101010101000 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x1010101010101000;

    outflank_d = OUTFLANK_4[(((unsigned int) O & 0x02442810) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000000182442810) * 0x0101010101010101) >> 54;    // hgfed[cbahg]...
    flipped |= FLIPPED_4_H[outflank_d] & 0x0000000002442810;    // A5E1H4

    outflank_h = OUTFLANK_4[(O >> 1) & 0x3f] & std::rotl(static_cast<uint8_t>(P), 2);
    flipped |= (unsigned char) FLIPPED_4_H[outflank_h];
        
    move.flipped = flipped;
    move.square = 0X0000000000000010ULL;
    move.position = D8;
}

void RXBitBoard::generate_flips_E8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    unsigned int outflank_h, outflank_d;
    unsigned long long flipped, outflank_v;

    outflank_v = ~O & 0x0808080808080800;
    outflank_v = (outflank_v & -outflank_v) & 0x0808080808080800 & P;
    flipped = OutflankToFlipmask(outflank_v) & 0x0808080808080800;

    outflank_d = OUTFLANK_3[(((unsigned int) O & 0x40221408) * 0x01010101) >> 25];
    outflank_d &= ((P & 0x0000008041221408) * 0x0101010101010101) >> 53;    // hgfedc[bahgf]...
    flipped |= FLIPPED_3_H[outflank_d] & 0x0000000040221408;    // A4D1H5

    outflank_h = OUTFLANK_3[(O >> 1) & 0x3f] & std::rotl(static_cast<uint8_t>(P), 3);
    flipped |= (unsigned char) FLIPPED_3_H[outflank_h];
        
    move.flipped = flipped;
    move.square = 0X0000000000000008ULL;
    move.position = E8;
}

void RXBitBoard::generate_flips_F8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0404040404040400, 0x0000804020100800 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned char) (O + 0x08) & P;
    flipped_h = outflank_h - ((unsigned int) (outflank_h != 0) << 3);

    flipped_h |= ((((unsigned int) P << 1) & 0x00000002) | (((unsigned int) P >> 7) & 0x00000200)) & O;

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    move.flipped = flipped;
    move.square = 0X0000000000000004ULL;
    move.position = F8;
    
    

}

void RXBitBoard::generate_flips_G8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0202020202020200, 0x0080402010080400 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned char) (O + 0x04) & P;
    flipped_h = outflank_h - ((unsigned int) (outflank_h != 0) << 2);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    move.flipped = flipped;
    move.square = 0X0000000000000002ULL;
    move.position = G8;
    
    

}

void RXBitBoard::generate_flips_H8(RXMove& move) const {


    const unsigned long long P = discs[player];
    const unsigned long long O = discs[player^1];

    uint64x2_t outflank_vd, flipped_vd;
    const uint64x2_t mask = { 0x0101010101010100, 0x8040201008040200 };
    const uint64x2_t one = vdupq_n_u64(1);
    unsigned int outflank_h, flipped_h;

    outflank_vd = not_O_in_mask(mask, O);
    outflank_vd = vandq_u64(vbicq_u64(outflank_vd, vsubq_u64(outflank_vd, one)), vdupq_n_u64(P));
    flipped_vd = vandq_u64(mask, vqsubq_u64(outflank_vd, one));

    outflank_h = (unsigned char) (O + 0x02) & P;
    flipped_h = outflank_h - ((unsigned int) (outflank_h != 0) << 1);

    uint64_t flipped = vget_lane_u64(vorr_u64(vget_low_u64(flipped_vd), vget_high_u64(flipped_vd)), 0) | flipped_h;

    move.flipped = flipped;
    move.square = 0X0000000000000001ULL;
    move.position = H8;
    
    

}



void RXBitBoard::init_generate_flips() {

    generate_flips[A1] = &RXBitBoard::generate_flips_A1;
    generate_flips[A2] = &RXBitBoard::generate_flips_A2;
    generate_flips[A3] = &RXBitBoard::generate_flips_A3;
    generate_flips[A4] = &RXBitBoard::generate_flips_A4;
    generate_flips[A5] = &RXBitBoard::generate_flips_A5;
    generate_flips[A6] = &RXBitBoard::generate_flips_A6;
    generate_flips[A7] = &RXBitBoard::generate_flips_A7;
    generate_flips[A8] = &RXBitBoard::generate_flips_A8;

    generate_flips[B1] = &RXBitBoard::generate_flips_B1;
    generate_flips[B2] = &RXBitBoard::generate_flips_B2;
    generate_flips[B3] = &RXBitBoard::generate_flips_B3;
    generate_flips[B4] = &RXBitBoard::generate_flips_B4;
    generate_flips[B5] = &RXBitBoard::generate_flips_B5;
    generate_flips[B6] = &RXBitBoard::generate_flips_B6;
    generate_flips[B7] = &RXBitBoard::generate_flips_B7;
    generate_flips[B8] = &RXBitBoard::generate_flips_B8;
    
    generate_flips[C1] = &RXBitBoard::generate_flips_C1;
    generate_flips[C2] = &RXBitBoard::generate_flips_C2;
    generate_flips[C3] = &RXBitBoard::generate_flips_C3;
    generate_flips[C4] = &RXBitBoard::generate_flips_C4;
    generate_flips[C5] = &RXBitBoard::generate_flips_C5;
    generate_flips[C6] = &RXBitBoard::generate_flips_C6;
    generate_flips[C7] = &RXBitBoard::generate_flips_C7;
    generate_flips[C8] = &RXBitBoard::generate_flips_C8;
    
    generate_flips[D1] = &RXBitBoard::generate_flips_D1;
    generate_flips[D2] = &RXBitBoard::generate_flips_D2;
    generate_flips[D3] = &RXBitBoard::generate_flips_D3;
    generate_flips[D6] = &RXBitBoard::generate_flips_D6;
    generate_flips[D7] = &RXBitBoard::generate_flips_D7;
    generate_flips[D8] = &RXBitBoard::generate_flips_D8;

    generate_flips[E1] = &RXBitBoard::generate_flips_E1;
    generate_flips[E2] = &RXBitBoard::generate_flips_E2;
    generate_flips[E3] = &RXBitBoard::generate_flips_E3;
    generate_flips[E6] = &RXBitBoard::generate_flips_E6;
    generate_flips[E7] = &RXBitBoard::generate_flips_E7;
    generate_flips[E8] = &RXBitBoard::generate_flips_E8;

    generate_flips[F1] = &RXBitBoard::generate_flips_F1;
    generate_flips[F2] = &RXBitBoard::generate_flips_F2;
    generate_flips[F3] = &RXBitBoard::generate_flips_F3;
    generate_flips[F4] = &RXBitBoard::generate_flips_F4;
    generate_flips[F5] = &RXBitBoard::generate_flips_F5;
    generate_flips[F6] = &RXBitBoard::generate_flips_F6;
    generate_flips[F7] = &RXBitBoard::generate_flips_F7;
    generate_flips[F8] = &RXBitBoard::generate_flips_F8;

    generate_flips[G1] = &RXBitBoard::generate_flips_G1;
    generate_flips[G2] = &RXBitBoard::generate_flips_G2;
    generate_flips[G3] = &RXBitBoard::generate_flips_G3;
    generate_flips[G4] = &RXBitBoard::generate_flips_G4;
    generate_flips[G5] = &RXBitBoard::generate_flips_G5;
    generate_flips[G6] = &RXBitBoard::generate_flips_G6;
    generate_flips[G7] = &RXBitBoard::generate_flips_G7;
    generate_flips[G8] = &RXBitBoard::generate_flips_G8;

    generate_flips[H1] = &RXBitBoard::generate_flips_H1;
    generate_flips[H2] = &RXBitBoard::generate_flips_H2;
    generate_flips[H3] = &RXBitBoard::generate_flips_H3;
    generate_flips[H4] = &RXBitBoard::generate_flips_H4;
    generate_flips[H5] = &RXBitBoard::generate_flips_H5;
    generate_flips[H6] = &RXBitBoard::generate_flips_H6;
    generate_flips[H7] = &RXBitBoard::generate_flips_H7;
    generate_flips[H8] = &RXBitBoard::generate_flips_H8;

}

#endif
