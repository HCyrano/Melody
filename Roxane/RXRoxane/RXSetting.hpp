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


#ifndef RXSETTING_HPP
#define RXSETTING_HPP

#define ARCH_X86_AVX2    0
#define ARCH_ARM_NEON    1

#ifndef ARCH
    #ifdef __AVX2__
        #define ARCH ARCH_X86_AVX2
    #else
        #define ARCH ARCH_ARM_NEON
    #endif
#endif


//login ON / OFF
#define LOGGING_ON

//Matchs avec / sans alpha_cut 91/157/49 [297]
//#define USE_PROBCUT_ALPHA

#define USE_IMPROVE_HELPFUL_MASTER_CONCEPT

//for record fforum-40-59 setting
//Matchs avec / sans :
//69/319/58 [446] s8r14 2:00
//15,5%/71,5%/13%
#define USE_SPLIT_AT_ROOT

//not efficient
//#define PV_EXTENSION

//LMR 10/20
//ELO +13 ±8 LLR 3,1078 (4000 games)
#define USE_LMR

#define USE_ETC

#define USE_STABILITY
#define USE_ENHANCED_STABLILITY

//for record solver
//#define SOLVER_DRIVER

//check pv in endgame at 100%
//#define EG_CHECK_PV
//#define EG_CHECK_SOLVER

//generate data for probcut
//#define TUNE_PROBCUT_MID
//#define TUNE_PROBCUT_END
//#define TUNE_PROBCUT_END2


//generate resultat file
//#define GENERATE_RES_FILE

//base de donnée
//#define GENERATE_RAWDATA

// type of coefficients probcut
//#define SIGMA_1ZONE
#ifdef SIGMA_1ZONE
#define PROBCUT_x2 // undefined use PROBCUT_x3
#endif
//#define SIGMA_2ZONES
#define SIGMA_3ZONES

//factorisation machine
#define FACT_MACH
//#define STATS_FM

#endif
