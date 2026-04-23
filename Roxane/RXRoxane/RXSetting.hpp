//
//  RXSetting.hpp
//  Roxane
//
//  Created by Causse Bruno on 15/07/2025.
//


#ifndef RXSETTING_HPP
#define RXSETTING_HPP

//#define RELEASE

//login actif / inactif
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
//#define USE_SPLIT_AT_ETC_MOBILITY

//not efficient
//#define PV_EXTENSION


#define USE_ETC

#define USE_STABILITY
#define USE_ENHANCED_STABLILITY

//for record solver
//#define SOLVER_DRIVER
//#define SOLVER_SETTING

//check pv in endgame at 100%
//#define EG_CHECK_PV

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
