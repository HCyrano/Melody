/*
 *  RXEvaluation.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 06/08/05.
 *  Copyleft 2005-2025 personnel.
 *
 */

#include <fstream>

#include "RXEvaluation.hpp"

//#define STATS_EVAL



static void check_read(const std::ifstream& stream, const char* filename, unsigned int pattern, unsigned int stage = 0) {
    if (!stream) {
        std::cerr << "CRITICAL ERROR: Lecture échouée dans " << filename
                  << " (pattern=" << pattern << ", stage=" << stage << ")" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}


void RXEvaluation::load() {

    // --- weight_v12.bin ---
    std::ifstream from_w("/Users/caussebruno/Documents/developpement/Roxane/build/weight_v12.bin", std::ios::binary);
    if (!from_w) {
        std::cerr << "CRITICAL ERROR: Impossible de charger weight_v12.bin" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (unsigned int iStage = 0; iStage < 60; iStage++) {
        for (Feature f : all_features) {

            eval_w[iStage][f] = new short[sizes[f]];
            from_w.read(reinterpret_cast<char*>(eval_w[iStage][f]), sizeof(short) * sizes[f]);
            check_read(from_w, "weight_v12.bin", f, iStage);

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
    // --- fm_w0.txt ---
    std::ifstream from_w0("/Users/caussebruno/Documents/developpement/Roxane/build/fm_w0_v12.txt");
    if (!from_w0) {
        std::cerr << "CRITICAL ERROR: Impossible de charger fm_w0_v12.txt" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (unsigned int iStage = 0; iStage < 60; iStage++) {
        if (!(from_w0 >> eval_w0[iStage])) {
            std::cerr << "CRITICAL ERROR: fm_w0.txt contient moins de 60 entiers (arrêt à l'index "
                      << iStage << ")" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    from_w0.close();


    // --- fm_V.bin ---
    std::ifstream from_V("/Users/caussebruno/Documents/developpement/Roxane/build/fm_V_v12.bin", std::ios::binary);
    if (!from_V) {
        std::cerr << "CRITICAL ERROR: Impossible de charger fm_V_v12.bin" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    for (Feature f :all_features) {

        eval_V[f] = new Vec_short[sizes[f]];
        // Lecture donnée par donnée pour ignorer le padding
        for (unsigned int i = 0; i < sizes[f]; ++i) {
            from_V.read(reinterpret_cast<char*>(eval_V[f][i].data), sizeof(short) * RANK);
        }
        check_read(from_V, "fm_V.bin", f);

        if (f != MOB_P && f != MOB_O)
            eval_V[f] += sizes[f] / 2;
    }

    from_V.close();
    
    // Static global pointers
    gVMob_P  = RXEvaluation::eval_V[MOB_P];
    gVMob_O  = RXEvaluation::eval_V[MOB_O];
    gVDiag5  = RXEvaluation::eval_V[DIAG5];
    gVDiag6  = RXEvaluation::eval_V[DIAG6];
    gVDiag7  = RXEvaluation::eval_V[DIAG7];
    gVDiag8  = RXEvaluation::eval_V[DIAG8];
    gVEdge1  = RXEvaluation::eval_V[EDGE2X];
    gVEdge2  = RXEvaluation::eval_V[EDGE64];
    gVEdge3  = RXEvaluation::eval_V[EDGE5];
    gVHv2    = RXEvaluation::eval_V[HV2];
    gVHv3    = RXEvaluation::eval_V[HV3];
    gVHv4    = RXEvaluation::eval_V[HV4];
    gVCorner = RXEvaluation::eval_V[CORNER];

    
        
#ifdef STATS_EVAL
    
    // Après load(), diagnostic eval_V
    // Titre principal
    std::cout << "\n=== Diagnostic eval_V (RANK=" << RANK << ") ===\n" << std::endl;

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
                short v = eval_V[f][idx][r];
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
    for (Feature f : all_features) {

        if (f != MOB_P && f != MOB_O)
            eval_V[f] -= sizes[f] / 2;
        delete[] eval_V[f];
        eval_V[f] = nullptr;
    }
    
    // Invalidate FM global pointers
    gVMob_P = gVMob_O = nullptr;
    gVDiag5 = gVDiag6 = gVDiag7 = gVDiag8 = nullptr;
    gVEdge1 = gVEdge2 = gVEdge3 = nullptr;
    gVHv2   = gVHv3   = gVHv4   = nullptr;
    gVCorner = nullptr;
    
#endif
    
}
