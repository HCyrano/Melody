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

#if ARCH == ARCH_X86_GENERIC

#include "RXBitBoard.hpp"


const unsigned char COUNT_H[256] = {
 0,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 8,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
10,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 8,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
12,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 8,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
10,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 8,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
 6,  0,  0,  0,  2,  0,  0,  0,  4,  0,  0,  0,  2,  0,  0,  0, 
};

const unsigned char COUNT_G[256] = {
 0,  0,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 6,  6,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 8,  8,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 6,  6,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
10, 10,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 6,  6,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 8,  8,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 6,  6,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
 4,  4,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_F[256] = {
0,  2,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
4,  6,  4,  4,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
6,  8,  6,  6,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
4,  6,  4,  4,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
8, 10,  8,  8,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
4,  6,  4,  4,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
6,  8,  6,  6,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
4,  6,  4,  4,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
2,  4,  2,  2,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_E[256] = {
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
4,  8,  6,  6,  4,  4,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
6, 10,  8,  8,  6,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
4,  8,  6,  6,  4,  4,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  4,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_D[256] = {
0,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  8,  6,  6,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
4, 10,  8,  8,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  4, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2,  8,  6,  6,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  6,  4,  4,  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_C[256] = {
0,  8,  6,  6,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  8,  6,  6,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
2, 10,  8,  8,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  4, 
2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  8,  6,  6,  4,  4,  4,  4,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_B[256] = {
0, 10,  8,  8,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  4, 
2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0, 10,  8,  8,  6,  6,  6,  6,  4,  4,  4,  4,  4,  4,  4,  4, 
2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};

const unsigned char COUNT_A[256] = {
0, 12, 10, 10,  8,  8,  8,  8,  6,  6,  6,  6,  6,  6,  6,  6, 
4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4, 
2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 
2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 
};



int RXBitBoard::count_flips_A1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[((discs_player & 0X0040201008040201ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_A[ (discs_player & 0X7F00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_B1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0040404040404040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_B[((discs_player & 0X00A0100804020100ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_B[ (discs_player & 0XBF00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_C1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0020202020202020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0050880402010000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0XDF00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_D1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0010101010101010ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0028448201000000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0XEF00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_E1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0008080808080808ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0014224180000000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0XF700000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_F1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0004040404040404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X000A112040800000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0XFB00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_G1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0002020202020202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_G[((discs_player & 0X0005081020408000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_G[ (discs_player & 0XFD00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_H1(const unsigned long long& discs_player) {
	
	return COUNT_A[((discs_player & 0X0001010101010101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_H[((discs_player & 0X0002040810204080ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_H[ (discs_player & 0XFE00000000000000ULL) >> 56];
}


int RXBitBoard::count_flips_A2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X8000808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[((discs_player & 0X0000402010080402ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_A[ (discs_player & 0X007F000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_B2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X4000404040404040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_B[((discs_player & 0X0000A01008040201ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_B[ (discs_player & 0X00BF000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_C2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X2000202020202020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0000508804020100ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X00DF000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_D2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X1000101010101010ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0000284482010000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0X00EF000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_E2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X0800080808080808ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0000142241800000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0X00F7000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_F2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X0400040404040404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X00000A1120408000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X00FB000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_G2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X0200020202020202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_G[((discs_player & 0X0000050810204080ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_G[ (discs_player & 0X00FD000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_H2(const unsigned long long& discs_player) {
	
	return COUNT_B[((discs_player & 0X0100010101010101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_H[((discs_player & 0X0000020408102040ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_H[ (discs_player & 0X00FE000000000000ULL) >> 48];
}


int RXBitBoard::count_flips_A3(const unsigned long long& discs_player) {
	
	return COUNT_C[  ((discs_player & 0X8080008080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_C[((((discs_player & 0X2040004020100804ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[   (discs_player & 0X00007F0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_B3(const unsigned long long& discs_player) {
	
	return COUNT_C[  ((discs_player & 0X4040004040404040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_C[((((discs_player & 0X1020002010080402ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_B[   (discs_player & 0X0000BF0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_C3(const unsigned long long& discs_player) {
	
	return COUNT_C[((discs_player & 0X2020002020202020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X8040001008040201ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0810004080000000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X0000DF0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_D3(const unsigned long long& discs_player) {
	
	return COUNT_C[((discs_player & 0X1010001010101010ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X4020000804020100ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0408002040800000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0X0000EF0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_E3(const unsigned long long& discs_player) {
	
	return COUNT_C[((discs_player & 0X0808000808080808ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X2010000402010000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0204001020408000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0X0000F70000000000ULL) >> 40];
}


int RXBitBoard::count_flips_F3(const unsigned long long& discs_player) {
	
	return COUNT_C[((discs_player & 0X0404000404040404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X1008000201000000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[((discs_player & 0X0102000810204080ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X0000FB0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_G3(const unsigned long long& discs_player) {
	
	return COUNT_C[  ((discs_player & 0X0202000202020202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_C[((((discs_player & 0X0804000408102040ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_G[   (discs_player & 0X0000FD0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_H3(const unsigned long long& discs_player) {
	
	return COUNT_C[  ((discs_player & 0X0101000101010101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_C[((((discs_player & 0X0402000204081020ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_H[   (discs_player & 0X0000FE0000000000ULL) >> 40];
}


int RXBitBoard::count_flips_A4(const unsigned long long& discs_player) {
	
	return COUNT_D[  ((discs_player & 0X8080800080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_D[((((discs_player & 0X1020400040201008ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[   (discs_player & 0X0000007F00000000ULL) >> 32];
}


int RXBitBoard::count_flips_B4(const unsigned long long& discs_player) {
	
	return COUNT_D[  ((discs_player & 0X4040400040404040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_D[((((discs_player & 0X0810200020100804ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_B[   (discs_player & 0X000000BF00000000ULL) >> 32];
}


int RXBitBoard::count_flips_C4(const unsigned long long& discs_player) {
	
	return COUNT_D[((discs_player & 0X2020200020202020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0080400010080402ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0408100040800000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X000000DF00000000ULL) >> 32];
}


int RXBitBoard::count_flips_F4(const unsigned long long& discs_player) {
	
	return COUNT_D[((discs_player & 0X0404040004040404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X2010080002010000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[((discs_player & 0X0001020008102040ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X000000FB00000000ULL) >> 32];
}


int RXBitBoard::count_flips_G4(const unsigned long long& discs_player) {
	
	return COUNT_D[  ((discs_player & 0X0202020002020202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_D[((((discs_player & 0X1008040004081020ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_G[   (discs_player & 0X000000FD00000000ULL) >> 32];
}


int RXBitBoard::count_flips_H4(const unsigned long long& discs_player) {
	
	return COUNT_D[  ((discs_player & 0X0101010001010101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_D[((((discs_player & 0X0804020002040810ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_H[   (discs_player & 0X000000FE00000000ULL) >> 32];
}


int RXBitBoard::count_flips_A5(const unsigned long long& discs_player) {
	
	return COUNT_E[  ((discs_player & 0X8080808000808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_E[((((discs_player & 0X0810204000402010ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[   (discs_player & 0X000000007F000000ULL) >> 24];
}


int RXBitBoard::count_flips_B5(const unsigned long long& discs_player) {
	
	return COUNT_E[  ((discs_player & 0X4040404000404040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_E[((((discs_player & 0X0408102000201008ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_B[   (discs_player & 0X00000000BF000000ULL) >> 24];
}


int RXBitBoard::count_flips_C5(const unsigned long long& discs_player) {
	
	return COUNT_E[((discs_player & 0X2020202000202020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0000804000100804ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0204081000408000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X00000000DF000000ULL) >> 24];
}


int RXBitBoard::count_flips_F5(const unsigned long long& discs_player) {
	
	return COUNT_E[((discs_player & 0X0404040400040404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X4020100800020100ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[((discs_player & 0X0000010200081020ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X00000000FB000000ULL) >> 24];
}


int RXBitBoard::count_flips_G5(const unsigned long long& discs_player) {
	
	return COUNT_E[  ((discs_player & 0X0202020200020202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_E[((((discs_player & 0X2010080400040810ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_G[   (discs_player & 0X00000000FD000000ULL) >> 24];
}


int RXBitBoard::count_flips_H5(const unsigned long long& discs_player) {
	
	return COUNT_E[  ((discs_player & 0X0101010100010101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_E[((((discs_player & 0X1008040200020408ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_H[   (discs_player & 0X00000000FE000000ULL) >> 24];
}


int RXBitBoard::count_flips_A6(const unsigned long long& discs_player) {
	
	return COUNT_F[  ((discs_player & 0X8080808080008080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_F[((((discs_player & 0X0408102040004020ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[   (discs_player & 0X00000000007F0000ULL) >> 16];
}


int RXBitBoard::count_flips_B6(const unsigned long long& discs_player) {
	
	return COUNT_F[  ((discs_player & 0X4040404040004040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_F[((((discs_player & 0X0204081020002010ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_B[   (discs_player & 0X0000000000BF0000ULL) >> 16];
}


int RXBitBoard::count_flips_C6(const unsigned long long& discs_player) {
	
	return COUNT_F[((discs_player & 0X2020202020002020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0000008040001008ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0102040810004080ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X0000000000DF0000ULL) >> 16];
}


int RXBitBoard::count_flips_D6(const unsigned long long& discs_player) {
	
	return COUNT_F[((discs_player & 0X1010101010001010ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0000804020000804ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0001020408002040ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0X0000000000EF0000ULL) >> 16];
}


int RXBitBoard::count_flips_E6(const unsigned long long& discs_player) {
	
	return COUNT_F[((discs_player & 0X0808080808000808ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0080402010000402ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0000010204001020ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0X0000000000F70000ULL) >> 16];
}


int RXBitBoard::count_flips_F6(const unsigned long long& discs_player) {
	
	return COUNT_F[((discs_player & 0X0404040404000404ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X8040201008000201ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[((discs_player & 0X0000000102000810ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X0000000000FB0000ULL) >> 16];
}


int RXBitBoard::count_flips_G6(const unsigned long long& discs_player) {
	
	return COUNT_F[  ((discs_player & 0X0202020202000202ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_F[((((discs_player & 0X4020100804000408ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_G[   (discs_player & 0X0000000000FD0000ULL) >> 16];
}


int RXBitBoard::count_flips_H6(const unsigned long long& discs_player) {
	
	return COUNT_F[  ((discs_player & 0X0101010101000101ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_F[((((discs_player & 0X2010080402000204ULL) + 0x7F7F7F7F7F7F7F7FULL) & 0x8080808080808080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_H[   (discs_player & 0X0000000000FE0000ULL) >> 16];
}


int RXBitBoard::count_flips_A7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X8080808080800080ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[((discs_player & 0X0204081020400000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_A[ (discs_player & 0X0000000000007F00ULL) >> 8];
}


int RXBitBoard::count_flips_B7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X4040404040400040ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_B[((discs_player & 0X0102040810A00000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_B[ (discs_player & 0X000000000000BF00ULL) >> 8];
}


int RXBitBoard::count_flips_C7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X2020202020200020ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0001020488500000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X000000000000DF00ULL) >> 8];
}


int RXBitBoard::count_flips_D7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X1010101010100010ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0000018244280000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0X000000000000EF00ULL) >> 8];
}


int RXBitBoard::count_flips_E7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X0808080808080008ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0000804122140000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0X000000000000F700ULL) >> 8];
}


int RXBitBoard::count_flips_F7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X0404040404040004ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X00804020110A0000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X000000000000FB00ULL) >> 8];
}


int RXBitBoard::count_flips_G7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X0202020202020002ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_G[((discs_player & 0X8040201008050000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_G[ (discs_player & 0X000000000000FD00ULL) >> 8];
}


int RXBitBoard::count_flips_H7(const unsigned long long& discs_player) {
	
	return COUNT_G[((discs_player & 0X0101010101010001ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_H[((discs_player & 0X4020100804020000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_H[ (discs_player & 0X000000000000FE00ULL) >> 8];
}


int RXBitBoard::count_flips_A8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X8080808080808000ULL) * 0X0002040810204081ULL) >> 56]
	     + COUNT_A[((discs_player & 0X0102040810204000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_A[ (discs_player & 0X000000000000007FULL) >> 0];
}


int RXBitBoard::count_flips_B8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X4040404040404000ULL) * 0X0004081020408102ULL) >> 56]
	     + COUNT_B[((discs_player & 0X000102040810A000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_B[ (discs_player & 0X00000000000000BFULL) >> 0];
}


int RXBitBoard::count_flips_C8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X2020202020202000ULL) * 0X0008102040810204ULL) >> 56]
	     + COUNT_C[((discs_player & 0X0000010204885000ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_C[ (discs_player & 0X00000000000000DFULL) >> 0];
}


int RXBitBoard::count_flips_D8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X1010101010101000ULL) * 0X0010204081020408ULL) >> 56]
	     + COUNT_D[((discs_player & 0X0000000182442800ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_D[ (discs_player & 0X00000000000000EFULL) >> 0];
}


int RXBitBoard::count_flips_E8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X0808080808080800ULL) * 0X0020408102040810ULL) >> 56]
	     + COUNT_E[((discs_player & 0X0000008041221400ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_E[ (discs_player & 0X00000000000000F7ULL) >> 0];
}


int RXBitBoard::count_flips_F8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X0404040404040400ULL) * 0X0040810204081020ULL) >> 56]
	     + COUNT_F[((discs_player & 0X0000804020110A00ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_F[ (discs_player & 0X00000000000000FBULL) >> 0];
}


int RXBitBoard::count_flips_G8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X0202020202020200ULL) * 0X0081020408102040ULL) >> 56]
	     + COUNT_G[((discs_player & 0X0080402010080500ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_G[ (discs_player & 0X00000000000000FDULL) >> 0];
}


int RXBitBoard::count_flips_H8(const unsigned long long& discs_player) {
	
	return COUNT_H[((discs_player & 0X0101010101010100ULL) * 0X0102040810204080ULL) >> 56]
	     + COUNT_H[((discs_player & 0X8040201008040200ULL) * 0x0101010101010101ULL) >> 56]
	     + COUNT_H[ (discs_player & 0X00000000000000FEULL) >> 0];
}


RXBitBoard::type_count_flips const RXBitBoard::count_flips_X86[] = {
RXBitBoard::count_flips_H8, RXBitBoard::count_flips_G8, RXBitBoard::count_flips_F8, RXBitBoard::count_flips_E8, RXBitBoard::count_flips_D8, RXBitBoard::count_flips_C8, RXBitBoard::count_flips_B8, RXBitBoard::count_flips_A8,
RXBitBoard::count_flips_H7, RXBitBoard::count_flips_G7, RXBitBoard::count_flips_F7, RXBitBoard::count_flips_E7, RXBitBoard::count_flips_D7, RXBitBoard::count_flips_C7, RXBitBoard::count_flips_B7, RXBitBoard::count_flips_A7,
RXBitBoard::count_flips_H6, RXBitBoard::count_flips_G6, RXBitBoard::count_flips_F6, RXBitBoard::count_flips_E6, RXBitBoard::count_flips_D6, RXBitBoard::count_flips_C6, RXBitBoard::count_flips_B6, RXBitBoard::count_flips_A6,
RXBitBoard::count_flips_H5, RXBitBoard::count_flips_G5, RXBitBoard::count_flips_F5,                       nullptr,                       nullptr, RXBitBoard::count_flips_C5, RXBitBoard::count_flips_B5, RXBitBoard::count_flips_A5,
RXBitBoard::count_flips_H4, RXBitBoard::count_flips_G4, RXBitBoard::count_flips_F4,                       nullptr,                       nullptr, RXBitBoard::count_flips_C4, RXBitBoard::count_flips_B4, RXBitBoard::count_flips_A4,
RXBitBoard::count_flips_H3, RXBitBoard::count_flips_G3, RXBitBoard::count_flips_F3, RXBitBoard::count_flips_E3, RXBitBoard::count_flips_D3, RXBitBoard::count_flips_C3, RXBitBoard::count_flips_B3, RXBitBoard::count_flips_A3,
RXBitBoard::count_flips_H2, RXBitBoard::count_flips_G2, RXBitBoard::count_flips_F2, RXBitBoard::count_flips_E2, RXBitBoard::count_flips_D2, RXBitBoard::count_flips_C2, RXBitBoard::count_flips_B2, RXBitBoard::count_flips_A2,
RXBitBoard::count_flips_H1, RXBitBoard::count_flips_G1, RXBitBoard::count_flips_F1, RXBitBoard::count_flips_E1, RXBitBoard::count_flips_D1, RXBitBoard::count_flips_C1, RXBitBoard::count_flips_B1, RXBitBoard::count_flips_A1
};

#endif
