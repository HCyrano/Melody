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

#ifndef RXENGINE_HPP
#define RXENGINE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <ostream>
#include <locale>
#include <cstring>
#include <atomic>
#include <cmath>
#include <cassert>

#include "RXBBPatterns.hpp"
#include "RXBitBoard.hpp"
#include "RXMove.hpp"
#include "RXHashTable.hpp"
#include "RXHashShallow.hpp"
#include "RXEvaluation.hpp"
#include "RXSearch.hpp"

#include "RXHelper.hpp"

#include "RXTools.hpp"


struct NullBuffer : public std::streambuf {
    int overflow(int c) override { return c; }   // ignore tout
};

struct NullStream : public std::ostream {
    NullStream() : std::ostream(&nb) {}
private:
    NullBuffer nb;
};



extern "C"
void* init_threadHelper(void* pt);

extern "C"
void* init_pthreadMain(void* pt);

class RXRoxane;


class My_punct:public std::numpunct<char> {
public:
    
    
    explicit My_punct(size_t r=0):std::numpunct<char>(r) {}
    
protected:
    //bug ??? Parse ggs entre '.' & ','
    char do_decimal_point() const {return ',';}
    char do_thousands_sep() const {return ' ';}
    std::string do_grouping() const {return "\003";}
};


class RXSplitPoint {
    
public:
    
    enum t_callBackSearch {MID_ROOT, MID_PVS, MID_XPROBCUT, END_ROOT, END_PVS, END_XPROBCUT, END_ETC_MOBILITY};
    
    RXSplitPoint* parent;
    
    RXBBPatterns* sBoard;
    // Not copy-assignable, but no reallocation occurs at runtime — works as intended
    std::vector<RXBBPatterns> sBoardStack;
    
    RXMove* list;
    
    t_callBackSearch CBSearch;
    
    bool pv;
    int pvDev;
    int depth;
    int depth_reduction;
    int selectivity;
    int alpha, beta, bestscore, bestmove;
    
    
    mutable pthread_mutex_t lock;
    
    
    unsigned int master;
    std::atomic<unsigned int> n_Slaves;
    
    std::vector<bool>  slaves;
    
    std::atomic_bool explored;
    
    RXSplitPoint(int maxThreads) : parent(nullptr), sBoard(nullptr), sBoardStack(maxThreads), slaves(maxThreads),
    list(nullptr) {
        n_Slaves = 0;
        explored = false;
        pthread_mutex_init(&lock, nullptr);
    }
    
    // Copy constructor — called once at init by std::vector(count, value), never at runtime
    RXSplitPoint(const RXSplitPoint& o) :
        parent(o.parent), sBoard(o.sBoard), sBoardStack(o.sBoardStack),
        list(o.list), CBSearch(o.CBSearch),
        pv(o.pv), pvDev(o.pvDev), depth(o.depth), depth_reduction(o.depth_reduction), selectivity(o.selectivity),
        alpha(o.alpha), beta(o.beta), bestscore(o.bestscore), bestmove(o.bestmove),
        master(o.master), n_Slaves(o.n_Slaves.load()),
        slaves(o.slaves), explored(o.explored.load())
    {
        // Le mutex est réinitialisé plutôt que copié — ce qui est correct,
        // car copier un mutex en pleine utilisation serait dangereux.
        pthread_mutex_init(&lock, nullptr);
    }
    
    // Move constructor
    // Uses std::move() on vectors to transfer memory without copying (more efficient)
    // noexcept is required so that STL containers (std::vector) can use this constructor during reallocation
    RXSplitPoint(RXSplitPoint&& o) noexcept :
        parent(o.parent), sBoard(o.sBoard), sBoardStack(std::move(o.sBoardStack)),
        list(o.list), CBSearch(o.CBSearch),
        pv(o.pv), pvDev(o.pvDev), depth(o.depth), depth_reduction(o.depth_reduction), selectivity(o.selectivity),
        alpha(o.alpha), beta(o.beta), bestscore(o.bestscore), bestmove(o.bestmove),
        master(o.master), n_Slaves(o.n_Slaves.load()),
        slaves(std::move(o.slaves)), explored(o.explored.load())
    {
        // Pour le mutex, il transfère la valeur de o.lock puis réinitialise celui de o
        // geste défensif pour laisser o dans un état valide
        lock = o.lock;
        pthread_mutex_init(&o.lock, nullptr);
    }

    // Required by std::vector<RXSplitPoint>, never called at runtime
    RXSplitPoint& operator=(const RXSplitPoint&) = delete;
    
    ~RXSplitPoint() {
        pthread_mutex_destroy(&lock);
    }
    
    
};


class RXThread {
    
public:
    
    enum thread_state {
        UNINITIALISED,
        INITIALIZING,
        SEARCHING,
        AVAILABLE,
        RESERVED,
        WORKISWAITING,
        TERMINATED
    };
    
    
    RXSplitPoint* splitPoint;
    
    uint activeSplitPoints;
    //std::atomic<uint> activeSplitPoints;
    
    //non copiableAssignable, mais il n'y a pas de redimensionnenent (semble fonctionner)
    //soluce : remplacer le vector par un tableau static a taille fixe :-(
    std::vector<RXSplitPoint> splitPointStack;
    
    RXMove _move[61][34];
    
    
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    
    
    std::atomic<thread_state> state{UNINITIALISED};
    //std::atomic<thread_state> state{UNINITIALISED};
    
    //le parametre maxThread est utile pour splitPointStack
    RXThread(int maxThreads, int maxActiveSplitPoint = 8) : splitPoint(nullptr), activeSplitPoints(0),
    splitPointStack(maxActiveSplitPoint, RXSplitPoint(maxThreads)) {
        
        pthread_mutex_init(&lock, nullptr);
        pthread_cond_init(&cond, nullptr);
        
    }
    
    // Non copiable - utiliser emplace_back pour construire en place
    RXThread(const RXThread&) = delete;
    RXThread& operator=(const RXThread&) = delete;
    
    // Déplaçable pour permettre emplace_back
    RXThread(RXThread&& o) noexcept :
        splitPoint(o.splitPoint),
        activeSplitPoints(o.activeSplitPoints),
        splitPointStack(std::move(o.splitPointStack)),
        state(o.state.load())
    {
        std::memcpy(_move, o._move, sizeof(_move));
        // Récupère les mutex/cond de l'objet source
        lock = o.lock;
        cond = o.cond;
        // Invalide la source pour éviter double destroy
        pthread_mutex_init(&o.lock, nullptr);
        pthread_cond_init(&o.cond, nullptr);
    }
    
    ~RXThread() {
        pthread_cond_destroy(&cond);
        pthread_mutex_destroy(&lock);
        
    }
    
    
};


class RXEngine: public Runnable {
    
    /*--------------------------------------------     shared part (RXEngine.cpp)    --------------------------------------------*/
    
    enum t_search {BOOK, MIDGAME, ENDGAME};
    enum probcut_cut {NO_CUT, ALPHA_CUT, BETA_CUT};
    
    enum {
        DEPTH_0 = 0,
        DEPTH_1,
        DEPTH_2,
        DEPTH_3,
        DEPTH_4,
        DEPTH_5,
        DEPTH_6
    };

    enum {
        HASHTABLE = 0,
        INFERIOR,
        EXACT,
        SUPERIOR,
        INTERRUPT,
        GGS_MSG
    };
    
    static constexpr bool WITHOUT_FM = false;
    
    static const int CONFIDENCE[];
    static const float PERCENTILE[];
    static const int DEPTH_BOOSTER;
    static const unsigned int LMR_NO_REDUCTION;
    
    //time manager part
    int time_remaining;
    
    int time_start;
    int dependentTime_start;
    int time_search;
    
    int time_startLevel;
    int time_nextLevel;
    
    int time_move;
    int extratime_move;
    std::atomic<int> extra_time;
    
    std::atomic_bool first_move;
    
    bool dependent_time;
    
    t_search type_search;
    int select_search;
    
    bool new_search;
    
    RXHashTable* hTable;
    RXHashTable* main_PV;
    RXHashTable* expected_PV;
    RXHashTable* hTable_PV;
    
    RXHashTable::t_hash type_hashtable;
    
    RXHashShallow* hTable_shallow;
    
    
    RXBBPatterns search_sBoard;
    unsigned long long hash_code_search;
    
    RXBestMove best_answer;
    
    RXSearch::t_client search_client;
    
    int search_alpha;
    int search_beta;
    int search_depth;
    int search_selectivity;
    
    RXRoxane* manager;
    
    pthread_t pthreadMain[1];
    pthread_mutex_t mutex;
    
    std::atomic_bool resume_flag;
    
    bool use_pv_ext;
    int depth_pv_extension;
    
    std::ostream* log;
    std::ofstream* logfile;
    NullStream nullStream;
    
    bool probable_timeout(double probable_time_next_level) const;
    int pTime_next_level(RXBitBoard& board, int time_level, int depth = 1, int next_depth = 3) const;
    void determine_move_time(RXBitBoard& board);
    
    int get_current_time() const;
    int get_current_dependentTime() const;
    int time_limit() const;
    
    void set_type_search(t_search ts);
    t_search get_type_search() const;
    
    void set_select_search(int selectivity);
    int get_select_search() const;
    
    
    std::string variationPrincipal(RXBitBoard& sBoard, int depth) const;
    std::string showPV(RXBitBoard& board, int depthLine) const;
    std::string showHashmove(const RXBitBoard& board, RXHashValue& entry) const;
    std::string showBestmove(const int depth, const int selectivity, const int alpha, const int beta, const int score, const unsigned int bestmove) const;
    std::string display(RXBitBoard& board, const int type, const int allowed_display = 0, int score = 0, const int time = 0, const int time_level = 0);
    
    
    inline float sigma(const int n_empty, const int depth, const int depth_probcut) const;
    int probcut_bounds(const RXBitBoard& board, const int selectivity, const int depth, const int depth_probcut, const int pvDev, const int alpha, const int beta, int& lower_bound, int& upper_bound) const;
    
    void sort_moves(const unsigned int threadID, const bool endgame, RXBBPatterns& sBoard, const int depth, const int selectivity, const int alpha, const int beta, RXMove* list);
    
    int probcut(const unsigned int threadID, RXBBPatterns& sBoard, const int selectivity, const int alpha, const int depth, const int depth_probcut, const int lower_probcut, const int upper_probcut, RXMove* list, const bool hashMove);
    
    template <const bool UseFM = true>
    int PVS_last_ply(const unsigned int threadID, RXBBPatterns& sBoard, const int depth, int alpha, const int beta, const bool passed);
    template <const bool UseFM = true>
    int alphabeta_last_three_ply(const unsigned int threadID, RXBBPatterns& sBoard, int alpha, const int beta, const bool passed);
    template <const bool UseFM = true>
    int alphabeta_last_two_ply(const unsigned int threadID, RXBBPatterns& sBoard, int alpha, const int beta, const bool passed);
    
    /*--------------------------------------------     MidGame part (RXMidGame.cpp)    --------------------------------------------*/
    
        
    static const int MG_SELECT;
    
    static const int MG_DEEP_TO_SHALLOW;
    static const int MG_MOVING_WINDOW;
    
    static const bool USE_PV_EXTENSION;
    static const int PV_EXTENSION_DEPTH;
    static const int MIN_DEPTH_USE_PV_EXTENSION;
    
#ifdef USE_LMR
    static const int LMR_MIN_DEPTH;
    static const int LMR_DEEP_DEPTH;
#endif
    
    
    
    void iterative_deepening(RXBBPatterns& sBoard, RXMove* list, int selectivity, int depth, const int max_depth);
    void aspiration_search(RXBBPatterns& sBoard, const int selectivity, const int depth, RXMove* list);
    
    void MG_PVS_root(RXBBPatterns& sBoard, const int selectivity, const int depth,  const int alpha, const int beta, RXMove* list);
    void MG_SP_search_root(RXSplitPoint* sp, const unsigned int threadID);
    
    int MG_PVS_deep(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int selectivity, const int depth, int alpha, const int beta, const bool passed);
    void MG_SP_search_deep(RXSplitPoint* sp, const unsigned int threadID);
    
    int MG_PVS_shallow(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int depth, int alpha, const int beta, const bool passed);
    
    int MG_NWS_XProbCut(const unsigned int threadID, RXBBPatterns& sBoard, const int pvDev, const int selectivity, const int depth, const int alpha, const bool passed);
    void MG_SP_search_XProbcut(RXSplitPoint* sp, const unsigned int threadID);
    
    
    /*--------------------------------------------     EndGame part (RXEndGame.cpp)    --------------------------------------------*/
    
    static const int stability_threshold[];
    
    static const int EG_MEDIUM_TO_SHALLOW;
    static const int EG_MEDIUM_HI_TO_LOW ;
    static const int EG_DEEP_TO_MEDIUM;
    
    static const int MIN_DEPTH_USE_ENDCUT;
    
    static const int EG_HIGH_SELECT;
    
    
    void EG_check_PV(RXBBPatterns& sBoard, const int score);
    bool EG_check_PV(std::vector<unsigned char>& pv, RXBBPatterns& sBoard, const int score);

    
    void EG_driver(RXBBPatterns& board, int selectivity, int end_selectivity, RXMove* list);
    
    void EG_PVS_root(RXBBPatterns& board, const int selectivity, int alpha, const int beta, RXMove* list);
    void EG_SP_search_root(RXSplitPoint* sp, const unsigned int threadID);
    
    int    EG_PVS_deep(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int selectivity, int alpha, const int beta, const bool passed);
    void EG_SP_search_DEEP(RXSplitPoint* sp, const unsigned int threadID);
    
    int EG_PVS_ETC_mobility(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, int alpha, const int beta, const bool passed);
    void EG_SP_search_ETC_Mobility(RXSplitPoint* sp, const unsigned int threadID);
    
    int EG_PVS_hash_mobility(const unsigned int threadID, RXBitBoard& board, const bool pv, int alpha, const int beta, const bool passed);
    int EG_alphabeta_hash_mobility(const unsigned int threadID, RXBitBoard& board, const bool pv, int alpha, const int beta, const bool passed);
    int EG_alphabeta_hash_parity(const unsigned int threadID, RXBitBoard& board, const bool pv, int alpha, const int beta, const bool passed);
    int EG_alphabeta_parity(const unsigned int threadID, RXBitBoard& board, int alpha, int beta, const bool passed);
    
    int EG_NWS_XEndCut(const unsigned int threadID, RXBBPatterns& sBoard, const int pvDev, const int selectivity, const int alpha, const bool passed);
    void EG_SP_search_XEndcut(RXSplitPoint* sp, const unsigned int threadID);
    
    /*--------------------------------------------     Multithreads part (RXEngine.cpp)    --------------------------------------------*/
    
    const unsigned int THREAD_MAX;
    static constexpr unsigned int ACTIVE_SPLITPOINT_MAX = 12;
    static constexpr unsigned int THREAD_PER_SPLITPOINT_MAX = 4; //4
    
    
    //parameter for launch thread
    unsigned int idThread;
    
    
    static const int MIN_DEPTH_SPLITPOINT;
    
    std::vector<RXThread> threads;
    std::atomic<bool> allThreadsShouldExit{false}, allThreadsShouldSleep{true};
    
    
    std::atomic<unsigned int> activeThreads;
    
    pthread_mutex_t MP_sync;
    
    std::atomic_bool abort;
    
    
    void init_threads();
    void stop_threads();
    void wake_sleeping_threads();
    //    void wake_sleeping_thread(unsigned int threadID);
    inline bool idle_thread_exists(unsigned int master);
    bool thread_is_available(unsigned int slave, unsigned int master);
    inline bool thread_should_stop(unsigned int threadID);
    
    bool split(RXBBPatterns& sBoard, bool pv, int pvDev,
               int depth, int depth_reduction, int selectivity, int alpha, int beta, int& bestscore, unsigned int& bestmove,
               RXMove* list, unsigned int master, RXSplitPoint::t_callBackSearch callback);
   
    // Table de lookup statique (partagée par toutes les instances)
    struct alignas(64) WeightLUT {
        static constexpr int MAX_EMPTY = 60;
        std::array<float, MAX_EMPTY> early;
        std::array<float, MAX_EMPTY> mid;
        std::array<float, MAX_EMPTY> end;
        
        // Constructeur pour initialiser la table
        WeightLUT() {
            constexpr double inv_teta = 1.0 / 288.0;
            for (int n = 0; n < MAX_EMPTY; ++n) {
                double w_early_raw = std::exp(-((n - 50) * (n - 50)) * inv_teta);
                double w_mid_raw   = std::exp(-((n - 30) * (n - 30)) * inv_teta);
                double w_end_raw   = std::exp(-((n - 10) * (n - 10)) * inv_teta);
                
                // Normalisation
                double total = w_early_raw + w_mid_raw + w_end_raw;
                    
                early[n] = static_cast<float>(w_early_raw / total);
                mid[n]   = static_cast<float>(w_mid_raw / total);
                end[n]   = static_cast<float>(w_end_raw / total);

            }
        }
    };
    
    // Instance statique unique de la table
    static inline const WeightLUT s_weight_lut;

    
public:
    
    struct ThreadLaunchArgs {
        RXEngine* engine;
        unsigned int threadID;
    };

    void* idle_loop(const unsigned int threadID, RXSplitPoint* waitSp);

    
    static const int NO_SELECT;
    static unsigned int confidence_to_selectivity(int c);
    static unsigned int selectivity_to_confidence(int s);

    RXEngine(RXRoxane* _manager, std::string _id, int maxThread = 1);
    ~RXEngine();
    
    RXBBPatterns& get_board();
    
    void writeLog(std::string s); //log
    
    void get_move(RXSearch& s);
    //    void verif_sBoard(RXSearch& s);
    
    virtual void* run();
    
    void stop(std::string msg);
    void resume();
    
    //MidGame part (RXMidGame.cpp)
    
    
    //EndGame part (RXEndGame.cpp)
    
    
    //Multithread part
    
    int get_THREAD_MAX();

#ifdef TUNE_PROBCUT_MID

    void probcut_mid_data(RXHashTable* HT, RXHashTable* PV);

#endif
    
#ifdef TUNE_PROBCUT_END

    void probcut_end_data(RXHashTable* HT, RXHashTable* PV);

#endif

#ifdef TUNE_PROBCUT_END2

    void probcut_end2_data(const std::string& file_name, RXHashTable* HT, RXHashTable* PV);

#endif

};



inline int RXEngine::get_THREAD_MAX() {
    return THREAD_MAX;
}


inline int RXEngine::get_current_time() const {
    return get_system_time() - time_start;
}

inline int RXEngine::get_current_dependentTime() const {
    return get_system_time() - dependentTime_start;
}

inline int RXEngine::time_limit() const {
    
    int time = time_move;
    
    if(first_move.load())
        time = (66*time)/100;
    
    if(extra_time)
        time += extratime_move;
    
    return time;
}

#ifdef SIGMA_3ZONES

#if ARCH == ARCH_ARM_NEON

//borner sur [2.7f : 30.0f]
inline float RXEngine::sigma(const int n_empty, const int depth, const int depth_probcut) const {
    // Indices des zones
    constexpr int early = 0;
    constexpr int mid   = 1;
    constexpr int end   = 2;
    
    alignas(16) constexpr float probcut_a[] = {0.29736871, 0.09169649, 0.41091810, 0.0f};
    alignas(16) constexpr float probcut_b[] = {-0.15243659, -0.08251643, -0.44106809, 0.0f};
    alignas(16) constexpr float probcut_c[] = {0.36970516, 0.08281715, -0.00975311, 0.0f};
    alignas(16) constexpr float probcut_d[] = {0.01847852, 1.44446884, 0.06738396, 0.0f};
    alignas(16) constexpr float probcut_e[] = {-1.10292167, -15.87003934, -0.77841377, 0.0f};
    alignas(16) constexpr float probcut_f[] = {21.26408154, 57.60294113, 2.90508020, 0.0f};
    alignas(16) constexpr float probcut_g[] = {-130.30442704, -69.06822646, 8.92764751, 0.0f};

    
    // Récupérer les poids depuis la LUT
    float w_early = s_weight_lut.early[n_empty];
    float w_mid   = s_weight_lut.mid[n_empty];
    float w_end   = s_weight_lut.end[n_empty];
    
    // ==================== VERSION NEON ====================
    // Charger les valeurs communes (dupliquées sur 4 lanes)
    float32x4_t n_empty_v = vdupq_n_f32(static_cast<float>(n_empty));
    float32x4_t depth_probcut_v = vdupq_n_f32(static_cast<float>(depth_probcut));
    float32x4_t depth_v = vdupq_n_f32(static_cast<float>(depth));
    
    // Charger les coefficients pour les 3 zones en parallèle
    float32x4_t a_vec = vld1q_f32(probcut_a); // [a_early, a_mid, a_end, 0]
    float32x4_t b_vec = vld1q_f32(probcut_b);
    float32x4_t c_vec = vld1q_f32(probcut_c);
    float32x4_t d_vec = vld1q_f32(probcut_d);
    float32x4_t e_vec = vld1q_f32(probcut_e);
    float32x4_t f_vec = vld1q_f32(probcut_f);
    float32x4_t g_vec = vld1q_f32(probcut_g);
    
    // Calculer r = a*n_empty + b*depth_probcut + c*depth (pour les 3 zones)
    float32x4_t r = vmulq_f32(a_vec, n_empty_v);
    r = vmlaq_f32(r, b_vec, depth_probcut_v);  // r += b * depth_probcut
    r = vmlaq_f32(r, c_vec, depth_v);          // r += c * depth
    
    // Calculer r² et r³
    float32x4_t r2 = vmulq_f32(r, r);
    float32x4_t r3 = vmulq_f32(r2, r);
    
    // Calculer sigma = d*r³ + e*r² + f*r + g (polynôme degré 3)
    float32x4_t sigma = vmulq_f32(d_vec, r3);
    sigma = vmlaq_f32(sigma, e_vec, r2);
    sigma = vmlaq_f32(sigma, f_vec, r);
    sigma = vaddq_f32(sigma, g_vec);
    
    // Extraire les 3 résultats
    alignas(16) float results[4];
    vst1q_f32(results, sigma);
    
    // Combinaison pondérée finale
    float res = results[early] * w_early +
                results[mid] * w_mid +
                results[end] * w_end;
    
    return std::min(30.0f, std::max(2.7f, res));
}


#else

inline float RXEngine::sigma(const int n_empty, const int depth, const int depth_probcut) const
{

    constexpr int early = 0;
    constexpr int mid   = 1;
    constexpr int end   = 2;


    constexpr float probcut_a[] = {0.29736871, 0.09169649, 0.41091810};
    constexpr float probcut_b[] = {-0.15243659, -0.08251643, -0.44106809};
    constexpr float probcut_c[] = {0.36970516, 0.08281715, -0.00975311};
    constexpr float probcut_d[] = {0.01847852, 1.44446884, 0.06738396};
    constexpr float probcut_e[] = {-1.10292167, -15.87003934, -0.77841377};
    constexpr float probcut_f[] = {21.26408154, 57.60294113, 2.90508020};
    constexpr float probcut_g[] = {-130.30442704, -69.06822646, 8.92764751};


    //Cloches gaussiennes

    /*
    // Formule : exp( - (w - mean)^2 / (2 * teta^2))
    //les zones 50/30/10 donc une distance de 20 entre les centres des zones
    //on prend 12 = teta ce qui provoque un chevauchement de zones
    static const double inv_teta = 1.0/288.0; //2 * std::pow(12, 2);

     float w_early_raw = std::exp(-((n_empty - 50)*(n_empty - 50)) * inv_teta);
     float w_mid_raw   = std::exp(-((n_empty - 30)*(n_empty - 30)) * inv_teta);
     float w_end_raw   = std::exp(-((n_empty - 10)*(n_empty - 10)) * inv_teta);

     // Normalisation
     float total = w_early_raw + w_mid_raw + w_end_raw;

     float w_early = w_early_raw / total;
     float w_mid   = w_mid_raw / total;
     float w_end   = w_end_raw / total;
     */



    //table lookup
    float w_early = s_weight_lut.early[n_empty];
    float w_mid   = s_weight_lut.mid[n_empty];
    float w_end   = s_weight_lut.end[n_empty];


    // Fonction polynomiale par zone
    auto sigma = [&](int i) {
        float r = probcut_a[i] * n_empty + probcut_b[i] * depth_probcut + probcut_c[i] * depth;
        return probcut_d[i] * r * r * r +
               probcut_e[i] * r * r +
               probcut_f[i] * r +
               probcut_g[i];
    };

    // Combinaison douce
    float sig_early = sigma(early);
    float sig_mid   = sigma(mid);
    float sig_end   = sigma(end);

    float res = w_early*sig_early + w_mid * sig_mid + w_end * sig_end;

    return std::max(2.7f, res);

}


#endif

#endif



#ifdef SIGMA_2ZONES

// --- Modèle principal : probabilité (sigma) ---
inline float RXEngine::sigma(const int n_empty, const int depth, const int depth_probcut) const
{
    
    constexpr int mid = 0;
    constexpr int end = 1;

    
    // --- Coefficients appris (issus du script Python) ---
    // version 6: 119 779 393
    // const float RXEngine::PERCENTILE[] = {1.00f, 1.10f, 1.35f, 1.70f, 2.20f, 2.80f, 3.60f};
    // s8r14 2:00 Edmond vs edax
    // w88 d121 l68
    /*
    constexpr double probcut_a[] = {-0.00906229, 0.00483133};
    constexpr double probcut_b[] = {0.12115910, 0.07975382};
    constexpr double probcut_c[] = {-0.12071974, -0.01763140};
    constexpr double probcut_d[] = {-0.47638522, -3.58586370};
    constexpr double probcut_e[] = {-2.93341676, 9.99659776};
    constexpr double probcut_f[] = {-5.44314615, -9.52512272};
    constexpr double probcut_g[] = {0.01672187, 6.76910942};
    */
    
    //probcut v9.1
    constexpr float probcut_a[] = {0.21497775, 0.00738301};
    constexpr float probcut_b[] = {2.77883844, 0.07297002};
    constexpr float probcut_c[] = {-0.89648633, -0.01412426};
    constexpr float probcut_d[] = {-0.00016011, -4.83205149};
    constexpr float probcut_e[] = {0.01197217, 15.37476478};
    constexpr float probcut_f[] = {-0.34131886, -15.79339521};
    constexpr float probcut_g[] = {6.12897090, 8.46698928}; // {+1.0, +0.5}

    
    // Transitions progressives entre zones
    float s1 = sigmoid(n_empty, 35.0, 0.3); // base 40.0, 0,3

    float w_mid   = s1;
    float w_end   = 1.0 - s1;

    // Fonction polynomiale par zone
    auto sigma = [&](int i) {
        float r = probcut_a[i] * n_empty + probcut_b[i] * depth_probcut + probcut_c[i] * depth;
        return probcut_d[i] * r * r * r +
               probcut_e[i] * r * r +
               probcut_f[i] * r +
               probcut_g[i];
    };

    // Combinaison douce
    float sig_mid = sigma(mid);
    float sig_end = sigma(end);

    float res = w_mid * sig_mid + w_end * sig_end;

    return std::max(2.7f, res);
}



#endif

#ifdef SIGMA_1ZONE


inline float RXEngine::sigma(const int n_empty, const int depth, const int depth_probcut) const {
    
    float sigma;
    
#ifdef PROBCUT_x2
    //polynome 2d
    
    //edax coefficients
    constexpr float probcut_a = -0.10026799;
    constexpr float probcut_b = 0.31027733;
    constexpr float probcut_c = -0.57772603;
    constexpr float probcut_d = 0.07585621;
    constexpr float probcut_e = 1.16492647;
    constexpr float probcut_f = 5.9171698;
    
     
    sigma= probcut_a * n_empty + probcut_b * depth_probcut + probcut_c * depth;
    sigma = probcut_d * sigma * sigma + probcut_e * sigma + probcut_f;
    
#else
    //polynome 3d
    
    
    constexpr float probcut_a = -0.0017319169860170334;
    constexpr float probcut_b = 0.046028020390195414;
    constexpr float probcut_c = -0.02121182792010099;
    constexpr float probcut_d = -5.248427428319801;
    constexpr float probcut_e = 10.287391794385476;
    constexpr float probcut_f = -5.173813504774759;
    constexpr float probcut_g = 3.8344983118071387;
    
    
    
    sigma = probcut_a * n_empty + probcut_b * depth_probcut + probcut_c * depth;
    sigma =   probcut_d * sigma * sigma * sigma
            + probcut_e * sigma * sigma
            + probcut_f * sigma
            + probcut_g;

    
#endif
    
    //sigma with lower bound at 2,7
    return std::max(2.7f, sigma);

}

#endif

inline int RXEngine::probcut_bounds(const RXBitBoard& board, const int selectivity, const int depth, const int depth_probcut,  const int pvDev, const int alpha, const int beta, int& lower_bound, int& upper_bound) const {
    
    float coeff_pv = 1.15f/(1.0f + 0.05f*pvDev);

    // Performance ELO : +12 pts ±7 : LLR: 3.7458  (bornes : [-2.94, 2.94])
    float coeff_sc = 1.0f + (std::abs(alpha)/160.0f - 0.1f);

    //error evaluation with lower bound at 3
    int eval_error = static_cast<int>(std::round(sigma(board.n_empty, depth, depth_probcut) * coeff_pv * coeff_sc * PERCENTILE[selectivity]));
    
    lower_bound = std::max(-MAX_SCORE, alpha - eval_error);
    upper_bound = std::min(+MAX_SCORE, beta  + eval_error);
    
    return eval_error;

}


inline void RXEngine::set_type_search(t_search ts) {
    type_search = ts;
}

inline RXEngine::t_search RXEngine::get_type_search() const {
    return type_search;
}

inline void RXEngine::set_select_search(int selectivity) {
    select_search = selectivity;
}

inline int RXEngine::get_select_search() const {
    return select_search;
}

inline RXBBPatterns& RXEngine::get_board() {
    
    return search_sBoard;
}

// idle_thread_exists() tries to find an idle thread which is available as
// a slave for the thread with threadID "master".

inline bool RXEngine::idle_thread_exists(unsigned int master) {
    
    //    assert(master >= 0 && master < activeThreads);
    //    assert(activeThreads > 1);
    
    for(unsigned int i = 0; i < activeThreads; i++)
        if(thread_is_available(i, master))
            return true;
    return false;
}


// thread_should_stop() checks whether the thread with a given threadID has
// been asked to stop, directly or indirectly.  This can happen if a beta
// cutoff has occured in thre thread's currently active split point, or in
// some ancestor of the current split point.

inline bool RXEngine::thread_should_stop(unsigned int threadID) {
    
    RXSplitPoint* sp = threads[threadID].splitPoint;
    
    //emptie loop
    while(sp != nullptr && sp->explored == false)
        sp = sp->parent;
    
    return sp != nullptr;
    
}


#endif
