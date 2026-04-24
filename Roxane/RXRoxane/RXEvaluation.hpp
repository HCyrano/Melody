/*
 *  RXEvaluation.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 06/08/05.
 *  Copyleft 2005-2025 personnel.
 *
 */

#ifndef RXEVALUATION_HPP
#define RXEVALUATION_HPP

#include <iostream>

#include "RXConstantes.hpp"
#include "RXPattern.hpp"
#include "RXSetting.hpp"


#ifdef FACT_MACH

static constexpr unsigned int RANK = 16;

struct alignas(64) Vec_short {
    int squares[RANK]; // Precomputed square of each factor:
    short data[RANK];
    
    //short& operator[](int i)             { return data[i]; }
    const short& operator[](int i) const { return data[i]; }
};

#endif

class alignas(64) RXEvaluation {
    
    static constexpr unsigned int N_FEATURES = 14;
    static constexpr unsigned int sizes[N_FEATURES] = {24, 24, 243, 729, 2187, 6561, 59049, 59049, 59049, 59049, 6561, 6561, 6561, 177147};
    
    public :

    enum Feature {
        MOB_P = 0, MOB_O,
        DIAG5, DIAG6, DIAG7, DIAG8,
        EDGE1, EDGE2, EDGE3, EDGE4,
        HV2, HV3, HV4,
        CORNER};

    // trick for iteration
    static constexpr Feature all_features[] = {
            MOB_P, MOB_O,
            DIAG5, DIAG6, DIAG7, DIAG8,
            EDGE1, EDGE2, EDGE3, EDGE4,
            HV2, HV3, HV4,
            CORNER
        };
    
    static constexpr std::string names[] = {
        "MOB_P", "MOB_O", "DIAG5", "DIAG6", "DIAG7", "DIAG8",
        "EDGE1", "EDGE2", "EDGE3", "EDGE4", "HV2", "HV3", "HV4", "CORNER"
    };


    static void load();
    static void unload();
    
    static std::string get_version() {
            return "FM_R16i16 2026-04-10";
        }

    
    alignas(64) static inline short* eval_w[60][N_FEATURES] = {};
    
#ifdef FACT_MACH
    
    alignas(64) static inline int eval_w0[60];
    alignas(64) static inline Vec_short* eval_V[N_FEATURES] = {};

    // FM static global pointer declarations
    static inline const Vec_short* gVMob_P  = nullptr;
    static inline const Vec_short* gVMob_O  = nullptr;
    static inline const Vec_short* gVDiag5  = nullptr;
    static inline const Vec_short* gVDiag6  = nullptr;
    static inline const Vec_short* gVDiag7  = nullptr;
    static inline const Vec_short* gVDiag8  = nullptr;
    static inline const Vec_short* gVEdge1  = nullptr;  // EDGE2X
    static inline const Vec_short* gVEdge2  = nullptr;  // EDGE64
    static inline const Vec_short* gVEdge3  = nullptr;  // EDGE2*(3/2)
    static inline const Vec_short* gVEdge4  = nullptr;  // EDGE2*5
    static inline const Vec_short* gVHv2    = nullptr;
    static inline const Vec_short* gVHv3    = nullptr;
    static inline const Vec_short* gVHv4    = nullptr;
    static inline const Vec_short* gVCorner = nullptr;

#endif





        
};


#endif


