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
    // 1. Détection des architectures x86 / x64
    #if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
        #ifdef __AVX2__
            #define ARCH ARCH_X86_AVX2
        #else
            // Sécurité absolue pour Windows / Linux : on refuse de compiler sans AVX2
            #error "Erreur: AVX2 est le minimum requis pour cette architecture. Activez /arch:AVX2 sous MSVC ou -mavx2 sous GCC/Clang."
        #endif

    // 2. Détection des architectures ARM / ARM64
    #elif defined(__aarch64__) || defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
        #define ARCH ARCH_ARM_NEON

    // 3. Cas inconnu
    #else
        #error "Architecture non supportée (AVX2 ou ARM Neon requis)."
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
//--- Calculateur de Performance ELO + SPRT (W/D/L) ---
//Nombre de Victoires (W) : 2073
//Nombre de Nuls (D)      : 3927
//Nombre de Défaites (L)  : 2000
//--------------------------------------------------
//Performance ELO : +3 pts
//Incertitude (σ) : 6 pts
//Plage de confiance (95%) : [-9 , 15]
//--------------------------------------------------
//SPRT (H0: Elo=0, H1: Elo=+5, α=0.05, β=0.05)
//LLR        : 0.4365  (bornes : [-2.9444, 2.9444])
//Progression: 57.4%  vers la décision
//Verdict    : ⏳ INDÉCIS      — continuer les tests
//--------------------------------------------------
//#define PV_EXTENSION

//LMR 10/20
//--- Calculateur de Performance ELO + SPRT (W/D/L) ---
//Nombre de Victoires (W) : 871
//Nombre de Nuls (D)      : 2376
//Nombre de Défaites (L)  : 753
//--------------------------------------------------
//Performance ELO : +10 pts
//Incertitude (σ) : 7 pts
//Plage de confiance (95%) : [-4 , 24]
//--------------------------------------------------
//SPRT (H0: Elo=0, H1: Elo=+5, α=0.05, β=0.05)
//LLR        : 3.1691  (bornes : [-2.9444, 2.9444])
//Progression: 103.8%  vers la décision
//Verdict    : ✅ H1 ACCEPTÉE  — amélioration significative détectée
//--------------------------------------------------
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
//#define SIGMA_2ZONES
#define SIGMA_3ZONES

#if defined(SIGMA_1ZONE) + defined(SIGMA_2ZONES) + defined(SIGMA_3ZONES) != 1
#error "Exactement une option SIGMA_xZONE doit être définie"
#endif

#ifdef SIGMA_1ZONE
#define PROBCUT_x2 // undefined use PROBCUT_x3
#endif

//factorisation machine
#define FACT_MACH
//#define STATS_FM

#endif
