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

#include <fstream>

#include "RXEvaluation.hpp"

//#define STATS_EVAL



static void check_read(const std::ifstream& stream, const std::string filename, unsigned int pattern, unsigned int stage = 0) {
    if (!stream) {
        std::cerr << "CRITICAL ERROR: Lecture échouée dans " << filename
                  << " (pattern=" << pattern << ", stage=" << stage << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


void RXEvaluation::load() {
    
    const std::string path = "./";

    // --- weight.bin ---
    const std::string filename_W = "weight_v11.bin";
    std::ifstream from_w(path + filename_W, std::ios::binary);

    if (!from_w) {
        std::cerr << "CRITICAL ERROR: Impossible de charger " << filename_W << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (unsigned int iStage = 0; iStage < N_STAGES_EVAL; iStage++) {
        for (Feature f : all_features) {

            eval_w[iStage][f] = new short[sizes[f]];
            from_w.read(reinterpret_cast<char*>(eval_w[iStage][f]), sizeof(short) * sizes[f]);
            check_read(from_w, filename_W, f, iStage);

            if (f != MOB_P && f != MOB_O)
                eval_w[iStage][f] += sizes[f] / 2;
        }
    }

    from_w.close();
    
#ifdef STATS_EVAL
    
    std::cout << "\n=== Diagnostic eval_W ===\n" << std::endl;
    
    // Ligne d'en-tête (Titres)
    std::cout << std::left  << std::setw(10) << "Feature" << "  "
              << std::right << std::setw(8)  << "zeros%"  << "  "
              << std::setw(8)  << "min"     << "  "
              << std::setw(8)  << "max"     << std::endl;

    // Ligne de séparation (Tirets)
    std::cout << std::left  << std::setw(10) << "-------" << "  "
              << std::right << std::setw(8)  << "------"  << "  "
              << std::setw(8)  << "---"     << "  "
              << std::setw(8)  << "---"     << std::endl;



    // Après load(), pour chaque feature :
    for (Feature f : all_features) {
        int interval = sizes[f] / 2;
        long long zeros = 0, total = 0;
        short fmin = 32767, fmax = -32768;
        
        for (int stage = 0; stage < 60; stage++) {
            for (int idx = -interval; idx <= interval; idx++) {
                short v = eval_w[stage][f][idx];
                if (v == 0) zeros++;
                total++;
                fmin = std::min(fmin, v);
                fmax = std::max(fmax, v);
            }
        }
        std::cout << std::left  << std::setw(10) << names[f]
                  << " " << std::right << std::fixed << std::setprecision(1) << std::setw(8) << (100.0 * zeros / total) << "%"
                  << "  " << std::setw(8) << fmin
                  << "  " << std::setw(8) << fmax
                  << std::endl;
    }
    
#endif
    
#ifdef FACT_MACH
    
    // --- weight0.txt ---
    const std::string filename_W0 = "weight_0_v12.txt";
    std::ifstream from_w0(path + filename_W0);

    if (!from_w0) {
        std::cerr << "CRITICAL ERROR: Impossible de charger " << filename_W0 << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (unsigned int iStage = 0; iStage < 60; iStage++) {
        if (!(from_w0 >> eval_w0[iStage])) {
            std::cerr << "CRITICAL ERROR: " << filename_W0 << " contient moins de 60 entiers (arrêt à l'index "
                      << iStage << ")" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    from_w0.close();


    // --- weight_V.bin ---

    for(unsigned int stage = 0; stage < N_STAGES_FM; ++stage) {
        
        const std::string filename_V_arr[] = {"weight_Va_v12.bin", "weight_Vb_v12.bin"};
        const std::string filename_V = filename_V_arr[stage];

        std::ifstream from_V(path + filename_V, std::ios::binary);
        
        if (!from_V) {
            std::cerr << "CRITICAL ERROR: Impossible de charger " << filename_V << std::endl;
            std::exit(EXIT_FAILURE);
        }

        for (Feature f :all_features) {
            
            eval_V[stage][f] = new Vec_short[sizes[f]];
            // Lecture donnée par donnée pour ignorer le padding
            for (unsigned int i = 0; i < sizes[f]; ++i) {
                from_V.read(reinterpret_cast<char*>(eval_V[stage][f][i].data), sizeof(short) * RANK);
                check_read(from_V, filename_V, f);
                
                // Précalculer les carrés
                for (unsigned int r = 0; r < RANK; ++r) {
                    short v = eval_V[stage][f][i].data[r];
                    eval_V[stage][f][i].squares[r] = static_cast<int>(v) * v;
                }
            }
            
            if (f != MOB_P && f != MOB_O)
                eval_V[stage][f] += sizes[f] / 2;
        }
        
        from_V.close();
        
        // Static global pointers
        gVMob_P[stage]  = RXEvaluation::eval_V[stage][MOB_P];
        gVMob_O[stage]  = RXEvaluation::eval_V[stage][MOB_O];
        gVDiag5[stage]  = RXEvaluation::eval_V[stage][DIAG5];
        gVDiag6[stage]  = RXEvaluation::eval_V[stage][DIAG6];
        gVDiag7[stage]  = RXEvaluation::eval_V[stage][DIAG7];
        gVDiag8[stage]  = RXEvaluation::eval_V[stage][DIAG8];
        gVEdge1[stage]  = RXEvaluation::eval_V[stage][EDGE1];
        gVEdge2[stage]  = RXEvaluation::eval_V[stage][EDGE2];
        gVEdge3[stage]  = RXEvaluation::eval_V[stage][EDGE3];
        gVEdge4[stage]  = RXEvaluation::eval_V[stage][EDGE4];
        gVHv2[stage]    = RXEvaluation::eval_V[stage][HV2];
        gVHv3[stage]    = RXEvaluation::eval_V[stage][HV3];
        gVHv4[stage]    = RXEvaluation::eval_V[stage][HV4];
        gVCorner[stage] = RXEvaluation::eval_V[stage][CORNER];

    }
    

    
        
#ifdef STATS_EVAL
    
    // Après load(), diagnostic eval_V
    // Titre principal
    std::cout << "\n=== Diagnostic eval_V (RANK=" << RANK << ") ===\n" << std::endl;
    
    for(unsigned int stage = 0; stage < N_STAGES_FM; ++stage) {
        
        std::cout << "stage : " << stage << std::endl;
        
        // Ligne d'en-tête (Titres)
        std::cout << std::left  << std::setw(10) << "Feature" << "  "
        << std::right << std::setw(8)  << "zeros%"  << "  "
        << std::setw(13) << "in[-127,127]%" << "  " // Note: 13 pour correspondre au tiret ci-dessous
        << std::setw(8)  << "min"     << "  "
        << std::setw(8)  << "max"     << "  "
        << std::setw(8)  << "std"     << std::endl;
        
        // Ligne de séparation (Tirets)
        std::cout << std::left  << std::setw(10) << "-------" << "  "
        << std::right << std::setw(8)  << "------"  << "  "
        << std::setw(13) << "-------------" << "  "
        << std::setw(8)  << "---"     << "  "
        << std::setw(8)  << "---"     << "  "
        << std::setw(8)  << "---"     << std::endl;
        
        for (Feature f : all_features) {
            int      interval = (f == MOB_P || f == MOB_O) ? 0 : sizes[f] / 2;
            long long zeros   = 0, in127 = 0, total = 0;
            short    fmin = 32767, fmax = -32768;
            long long fsum = 0, fsumsq = 0;
            
            for (int idx = -interval; idx <= (int)(sizes[f] - 1 - interval); idx++) {
                for (unsigned int r = 0; r < RANK; r++) {
                    short v = eval_V[stage][f][idx][r];
                    if (v == 0)             zeros++;
                    if (v >= -127 && v <= 127) in127++;
                    if (v < fmin) fmin = v;
                    if (v > fmax) fmax = v;
                    fsum   += v;
                    fsumsq += (long long)v * v;
                    total++;
                }
            }
            
            double mean   = (double)fsum   / total;
            double stddev = std::sqrt((double)fsumsq / total - mean * mean);
            
            std::cout << std::left  << std::setw(10) << names[f] << "  "
            << std::right << std::fixed    << std::setprecision(1)
            << std::setw(7)  << (100.0 * zeros / total) << "%  "
            << std::setw(12) << (100.0 * in127 / total) << "%    "
            << std::setw(6)  << fmin << "    "
            << std::setw(6)  << fmax << "    "
            << std::setw(6)  << std::setprecision(1) << stddev
            << std::endl;
        }
        
        std::cout << std::endl;
    }
    
#endif
    
#endif
    
    
}


void RXEvaluation::unload() {

    for (unsigned int iStage = 0; iStage < 60; iStage++) {
        for (Feature f : all_features) {

            if (f != MOB_P && f != MOB_O)
                eval_w[iStage][f] -= sizes[f] / 2;
            delete[] eval_w[iStage][f];
            eval_w[iStage][f] = nullptr;
        }
    }

#ifdef FACT_MACH
    for(int stage = 0; stage <2; ++stage) {
        
        for (Feature f : all_features) {
            
            if (f != MOB_P && f != MOB_O)
                eval_V[stage][f] -= sizes[f] / 2;
            delete[] eval_V[stage][f];
            eval_V[stage][f] = nullptr;
        }
        
        // Invalidate FM global pointers
        gVMob_P[stage] = gVMob_O[stage] = nullptr;
        gVDiag5[stage] = gVDiag6[stage] = gVDiag7[stage] = gVDiag8[stage] = nullptr;
        gVEdge1[stage] = gVEdge2[stage] = gVEdge3[stage] = gVEdge4[stage] = nullptr;
        gVHv2[stage]   = gVHv3[stage]   = gVHv4[stage]   = nullptr;
        gVCorner[stage] = nullptr;
        
    }
    
#endif
    
}
