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

#ifndef RXEVALUATION_HPP
#define RXEVALUATION_HPP

#include <iostream>
#include <string_view>

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
    
    static constexpr unsigned int N_STAGES_EVAL = 60;
    static constexpr unsigned int N_STAGES_FM = 2;


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
    
    static constexpr std::string_view names[] = {
        "MOB_P", "MOB_O", "DIAG5", "DIAG6", "DIAG7", "DIAG8",
        "EDGE1", "EDGE2", "EDGE3", "EDGE4", "HV2", "HV3", "HV4", "CORNER"
    };


    static void load();
    static void unload();
    
    static std::string get_version() {
            return "FM_R16i16 2026-05-28";
        }

    
    alignas(64) static inline short* eval_w[N_STAGES_EVAL][N_FEATURES] = {};
    
#ifdef FACT_MACH
    
    alignas(64) static inline int eval_w0[N_STAGES_EVAL];
    alignas(64) static inline Vec_short* eval_V[N_STAGES_FM][N_FEATURES] = {};

    // FM static global pointer declarations
    static inline const Vec_short* gVMob_P[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVMob_O[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVDiag5[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVDiag6[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVDiag7[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVDiag8[N_STAGES_FM]  = {nullptr, nullptr};
    static inline const Vec_short* gVEdge1[N_STAGES_FM]  = {nullptr, nullptr};  // EDGE2X
    static inline const Vec_short* gVEdge2[N_STAGES_FM]  = {nullptr, nullptr};  // EDGE64
    static inline const Vec_short* gVEdge3[N_STAGES_FM]  = {nullptr, nullptr};  // EDGE2*(3/2)
    static inline const Vec_short* gVEdge4[N_STAGES_FM]  = {nullptr, nullptr};  // EDGE2*5
    static inline const Vec_short* gVHv2[N_STAGES_FM]    = {nullptr, nullptr};
    static inline const Vec_short* gVHv3[N_STAGES_FM]    = {nullptr, nullptr};
    static inline const Vec_short* gVHv4[N_STAGES_FM]    = {nullptr, nullptr};
    static inline const Vec_short* gVCorner[N_STAGES_FM] = {nullptr, nullptr};

#endif





        
};


#endif


