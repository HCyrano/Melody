/*
 *  RXBitBoard.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 27/06/05.
 *  many ideas from Toshihiko Okuhara [http://www.amy.hi-ho.ne.jp/okuhara/edaxopt.htm]
 *  Copyleft 2005-2025 personnel.
 *
 */

#ifndef RXBITBOARD_HPP
#define RXBITBOARD_HPP

#include <iostream>
#include <string>
#include <vector>

#include "RXConstantes.hpp"
#include "RXMove.hpp"
#include "RXTools.hpp"
#include "RXSetting.hpp"

#ifdef __ARM_NEON
#include "arm_neon.h"
#endif

class alignas(32) RXSquareList {
    
    public :
    
    int position;
    RXSquareList *previous;
    RXSquareList *next;

    RXSquareList(): position(NOMOVE), previous(nullptr), next(nullptr) {};

};



class alignas(32) RXBitBoard {
        
    private :
    static const unsigned long long hashSquare[64][2];
    
    // 2 camps, 8 octets par board, 256 valeurs possibles
    static unsigned long long hashByte[2][8][256];
    
    static unsigned char EDGE_STABILITY[256*256]; //unsigned char
    static int find_edge_stable(const int old_P, const int old_O, int stable);
    
    static void init_hashcodeTable();
    static void edge_stability_init();

    // move functions
#define func(pos) static unsigned long long do_flips_##pos(const unsigned long long& discs_player, const unsigned long long& discs_opponent); \
void generate_flips_##pos(RXMove& move) const \

    func(A1); func(B1); func(C1); func(D1); func(E1); func(F1); func(G1); func(H1);
    func(A2); func(B2); func(C2); func(D2); func(E2); func(F2); func(G2); func(H2);
    func(A3); func(B3); func(C3); func(D3); func(E3); func(F3); func(G3); func(H3);
    func(A4); func(B4); func(C4);                      func(F4); func(G4); func(H4);
    func(A5); func(B5); func(C5);                      func(F5); func(G5); func(H5);
    func(A6); func(B6); func(C6); func(D6); func(E6); func(F6); func(G6); func(H6);
    func(A7); func(B7); func(C7); func(D7); func(E7); func(F7); func(G7); func(H7);
    func(A8); func(B8); func(C8); func(D8); func(E8); func(F8); func(G8); func(H8);
    
#undef func
        
#ifdef __ARM_NEON
    
    static const unsigned char OUTFLANK_3[];
    static const unsigned char OUTFLANK_4[];
    static const unsigned long long FLIPPED_3_H[];
    static const unsigned long long FLIPPED_4_H[];
    
    static const unsigned char COUNT_FLIP[8][256];
    static const uint64x2_t mask_dvhd[64][2];
    
    
#else
    
    static const unsigned char COUNT_A[];
    static const unsigned char COUNT_B[];
    static const unsigned char COUNT_C[];
    static const unsigned char COUNT_D[];
    static const unsigned char COUNT_E[];
    static const unsigned char COUNT_F[];
    static const unsigned char COUNT_G[];
    static const unsigned char COUNT_H[];
    
#define func(pos) static int count_flips_##pos(const unsigned long long& discs_player)\

    func(A1); func(B1); func(C1); func(D1); func(E1); func(F1); func(G1); func(H1);
    func(A2); func(B2); func(C2); func(D2); func(E2); func(F2); func(G2); func(H2);
    func(A3); func(B3); func(C3); func(D3); func(E3); func(F3); func(G3); func(H3);
    func(A4); func(B4); func(C4);                     func(F4); func(G4); func(H4);
    func(A5); func(B5); func(C5);                     func(F5); func(G5); func(H5);
    func(A6); func(B6); func(C6); func(D6); func(E6); func(F6); func(G6); func(H6);
    func(A7); func(B7); func(C7); func(D7); func(E7); func(F7); func(G7); func(H7);
    func(A8); func(B8); func(C8); func(D8); func(E8); func(F8); func(G8); func(H8);
    
#undef func
    
    typedef int (*type_count_flips)(const unsigned long long& discs_player);

    
#endif


    public :
    
    static void static_init();

    
    //neighborhood for each square
    static const unsigned long long NEIGHBOR[];
    
    static const int PRESORTED_POSITION[];

    
    /*! a quadrant id for each square */
    static const unsigned long long QUADRANT_MASK[];
    static const int QUADRANT_SHIFT[];
    static const int QUADRANT_ID[];
    
    
    //structure board
    unsigned long long discs[2];
    int player;
    int n_empty;
    int parity;
    RXSquareList empties_list[62];
    RXSquareList *position_to_empties[64];
    mutable unsigned long long n_nodes;
    
    //public :
    
    void reset();
    
    unsigned long long get_n_nodes() const {
        return n_nodes;
    }
    
    RXBitBoard();
    
    //constructeur par copie
    RXBitBoard(const RXBitBoard& src);
    
    RXBitBoard& operator=(const RXBitBoard& src);
    
    void build(const std::string& init);
    
    friend std::ostream& operator<<(std::ostream& os, RXBitBoard& board);
    
    bool squareIsEmpty(const int position) const;
    bool isPassed();
    bool isEndGame();
        
    typedef unsigned long long  (*type_do_flips)(const unsigned long long& discs_player, const unsigned long long& discs_opponent);
    static type_do_flips const do_flips[];
    void (RXBitBoard::*generate_flips[64])(RXMove& move) const;
    void init_generate_flips();
    
    
#ifdef __ARM_NEON
    
    int count_flips(int pos, unsigned long long P) const;

    static inline uint64x2_t dual_legal_moves(const unsigned long long p, const unsigned long long o);
    inline uint64x2_t dual_count_legal_moves() const;
    static inline uint64x2_t dual_count_legal_moves(const unsigned long long p, const unsigned long long o);

#else
    
    static type_count_flips const count_flips[];
    
    static inline int count_legal_moves(const unsigned long long discs_player, const unsigned long long discs_opponent);
    inline void dual_count_legal_moves(int& mob_P, int& mob_O) const;
    static inline void dual_count_legal_moves(const unsigned long long discs_player, const unsigned long long discs_opponent, int& mob_P, int& mob_O);

    
#endif
    
    
    /* bool check_move(const int position, const int color) const; */

    unsigned int n_moves() const;
    void moves_producing(RXMove* start, unsigned long long exclude_mask = 0) const;
    
    //    static uint64_t calc_legal(const uint64_t P, const uint64_t O);
    inline unsigned long long get_legal_moves() const;
    static unsigned long long get_legal_moves(const unsigned long long discs_player, const unsigned long long discs_opponent);
    
    
    bool isValid_square(const unsigned int pos) const;
    
    static int count_potential_moves(const unsigned long long p_discs, const unsigned long long o_discs);
    static void dual_potential_mobility(const unsigned long long p_discs, const unsigned long long o_discs, int &p_pmob, int &o_pmob);

    

    static inline int get_mobility(const unsigned long long discs_player, const unsigned long long discs_opponent);
    static inline int get_corner_stability(const unsigned long long& discs_player);
    inline int get_edge_stability(const int player) const;
    static unsigned long long get_stable_edge(const unsigned long long  discs_player, const unsigned long long discs_opponent);
    inline int get_stability(const int player) const;
    static inline unsigned int count_stable_edge(const unsigned long long discs_player, const unsigned long long discs_opponent);
    static inline int get_stability(const unsigned long long discs_player, const unsigned long long discs_opponent);
    
    
    void do_move(const RXMove& move);
    void undo_move(const RXMove& move);
    void do_pass();
    
    
    int final_score() const;
    int final_score_1() const;
    int final_score_2(int alpha, const int beta) const;
    int final_score_2(const unsigned long long discs_player, const unsigned long long discs_opponent, const int alpha, const int beta, const int idSquare1, const int idSquare2) const;
    int final_score_3(int alpha, const int beta) const ;
    int final_score_3(const unsigned long long discs_player, const unsigned long long discs_opponent, int alpha, int beta, const unsigned int shuf3, const unsigned int empties3) const;
    int	final_score_4(int alpha, int beta, const bool passed) const;
    int	final_score_4(const unsigned long long discs_player, const unsigned long long discs_opponent, int alpha, const int beta, const bool passed, const unsigned int shuf4, const unsigned int empties4) const;
    
    std::string cassio_script() const;
    
    /* DEBUG */
    void print_empties_list() const;
    //void check_empties_list() const;
    static void print_64bits(unsigned long long n);
    void print_Board();
    std::string string_rawdata();
    static void print_Board(unsigned long long P, unsigned long long O);
    void print_moves_list(RXMove* MovesList) const;
    
    unsigned long long hashcode() const ;
    unsigned long long hashcode_after_move(RXMove* move)  const;
    
    /* test */
    //static unsigned long long cntbset(unsigned long long n);
    //void build(const unsigned long long disc_player, const unsigned long long disc_opp, const int color);
    
    
};

#ifdef __ARM_NEON
#include "RXBitBoard_NEON.hpp"
#else
#include "RXBitBoard_x86.hpp"

/*
 @brief count all legal moves
 
 @param P                    a bitboard representing player
 @param O                    a bitboard representing opponent
 @return count all legal moves
 */
inline int RXBitBoard::count_legal_moves(const unsigned long long p_discs, const unsigned long long o_discs) {
    
    const unsigned long long legals = get_legal_moves(p_discs, o_discs);
    return __builtin_popcountll(legals);
    
}

#endif


//#define    unpackA2A7(x)    ((((x) & 0x7e) * 0x0000040810204080) & 0x0001010101010100)
//#define    unpackH2H7(x)    ((((x) & 0x7e) * 0x0002040810204000) & 0x0080808080808000)
//#define    packA1A8(X)      ((((X) & 0x0101010101010101ULL) * 0x0102040810204080ULL) >> 56)
//#define    packH1H8(X)      ((((X) & 0x8080808080808080ULL) * 0x0002040810204081ULL) >> 56)

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long unpackA2A7(const unsigned long long x) noexcept {
    return ((x & 0x7eULL) * 0x0000040810204080ULL) & 0x0001010101010100ULL;
}

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long unpackH2H7(const unsigned long long x) noexcept {
    return ((x & 0x7eULL) * 0x0002040810204000ULL) & 0x0080808080808000ULL;
}

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long packA1A8(const unsigned long long x) noexcept {
    return ((x & 0x0101010101010101ULL) * 0x0102040810204080ULL) >> 56;
 }

[[nodiscard]] __attribute__((always_inline))
static constexpr inline unsigned long long packH1H8(const unsigned long long x) noexcept {
    return ((x & 0x8080808080808080ULL) * 0x0002040810204081ULL) >> 56;
 }

__attribute__((always_inline))
inline RXBitBoard& RXBitBoard::operator=(const RXBitBoard& src) {
    // On copie les données scalaires d'un bloc (Clang utilisera NEON ici)
    discs[BLACK] = src.discs[BLACK];
    discs[WHITE] = src.discs[WHITE];
    player = src.player;
    n_empty = src.n_empty;
    parity = src.parity;
    n_nodes = src.n_nodes;

    // Reconstruction de la liste sans branchement
    RXSquareList* __restrict__ current_dest_base = this->empties_list;
    RXSquareList* previous = current_dest_base;
    const RXSquareList* src_curr = src.empties_list->next;

    while(src_curr->position != NOMOVE) {
        // Accès direct via le mapping de position
        RXSquareList* empty = position_to_empties[src_curr->position];
        
        empty->previous = previous;
        previous->next = empty;
        
        previous = empty; // Optimisation registre
        src_curr = src_curr->next;
    }

    // Fermeture de la liste sur la sentinelle [61]
    RXSquareList* sentinel = &current_dest_base[61];
    sentinel->previous = previous;
    previous->next = sentinel;
    
    return *this;
}



inline void RXBitBoard::moves_producing(RXMove* start, unsigned long long exclude_mask) const {
    RXMove *list = start + 1, *previous = start;
    
    unsigned long long remaining = get_legal_moves(discs[player], discs[player^1]);
    remaining &= ~exclude_mask;
    
    for(RXSquareList* empties = empties_list->next;
        remaining && empties->position != NOMOVE;
        empties = empties->next)
    {
        const int pos = empties->position;
        const unsigned long long bit = 0x1ULL << pos;
        if(remaining & bit) {
            remaining ^= bit;
            ((this)->*(generate_flips[pos]))(*list);
            list->score = 0;
            previous = previous->next = list++;
        }
    }
    
    previous->next = nullptr;
}

__attribute__((always_inline))
inline unsigned long long RXBitBoard::hashcode() const {
    const unsigned long long p = discs[player];
    const unsigned long long o = discs[player ^ 1];

    unsigned long long h = hashByte[PLAYER][0][p & 0xFF]
                         ^ hashByte[PLAYER][1][(p >> 8) & 0xFF]
                         ^ hashByte[PLAYER][2][(p >> 16) & 0xFF]
                         ^ hashByte[PLAYER][3][(p >> 24) & 0xFF]
                         ^ hashByte[PLAYER][4][(p >> 32) & 0xFF]
                         ^ hashByte[PLAYER][5][(p >> 40) & 0xFF]
                         ^ hashByte[PLAYER][6][(p >> 48) & 0xFF]
                         ^ hashByte[PLAYER][7][(p >> 56) & 0xFF];

    h ^= hashByte[OPPONENT][0][o & 0xFF]
       ^ hashByte[OPPONENT][1][(o >> 8) & 0xFF]
       ^ hashByte[OPPONENT][2][(o >> 16) & 0xFF]
       ^ hashByte[OPPONENT][3][(o >> 24) & 0xFF]
       ^ hashByte[OPPONENT][4][(o >> 32) & 0xFF]
       ^ hashByte[OPPONENT][5][(o >> 40) & 0xFF]
       ^ hashByte[OPPONENT][6][(o >> 48) & 0xFF]
       ^ hashByte[OPPONENT][7][(o >> 56) & 0xFF];

    return h;
}

__attribute__((always_inline))
inline unsigned long long RXBitBoard::hashcode_after_move(RXMove* move) const {
    const unsigned long long o = discs[player] | (move->flipped | move->square);
    const unsigned long long p = discs[player ^ 1] ^ move->flipped;

    unsigned long long h = hashByte[PLAYER][0][p & 0xFF]
                         ^ hashByte[PLAYER][1][(p >> 8) & 0xFF]
                         ^ hashByte[PLAYER][2][(p >> 16) & 0xFF]
                         ^ hashByte[PLAYER][3][(p >> 24) & 0xFF]
                         ^ hashByte[PLAYER][4][(p >> 32) & 0xFF]
                         ^ hashByte[PLAYER][5][(p >> 40) & 0xFF]
                         ^ hashByte[PLAYER][6][(p >> 48) & 0xFF]
                         ^ hashByte[PLAYER][7][(p >> 56) & 0xFF];

    h ^= hashByte[OPPONENT][0][o & 0xFF]
       ^ hashByte[OPPONENT][1][(o >> 8) & 0xFF]
       ^ hashByte[OPPONENT][2][(o >> 16) & 0xFF]
       ^ hashByte[OPPONENT][3][(o >> 24) & 0xFF]
       ^ hashByte[OPPONENT][4][(o >> 32) & 0xFF]
       ^ hashByte[OPPONENT][5][(o >> 40) & 0xFF]
       ^ hashByte[OPPONENT][6][(o >> 48) & 0xFF]
       ^ hashByte[OPPONENT][7][(o >> 56) & 0xFF];

    return h;
}


__attribute__((always_inline))
inline void RXBitBoard::do_move(const RXMove& move) {
    
    discs[player] |= (move.flipped | move.square);
    player ^=1;
    discs[player] ^= move.flipped;
    
    --n_empty;
    parity ^= QUADRANT_ID[move.position];
    
    const RXSquareList *remove = position_to_empties[move.position];
    remove->previous->next = remove->next;
    remove->next->previous = remove->previous;
    
    ++n_nodes;
}

__attribute__((always_inline))
inline void RXBitBoard::undo_move(const RXMove& move) {
    
    RXSquareList *insert = position_to_empties[move.position];
    insert->previous->next = insert;
    insert->next->previous = insert;
    
    parity ^= QUADRANT_ID[move.position];
    ++n_empty;
    
    discs[player] |= move.flipped;
    player ^=1;
    discs[player] ^= (move.flipped | move.square);
    
}

__attribute__((always_inline))
inline void RXBitBoard::do_pass() {
    player ^= 1;
}

inline unsigned long long RXBitBoard::get_legal_moves() const {
    return get_legal_moves(discs[player], discs[player^1]);
}



/*
 @brief count all legal moves with twice for the corners
 
 @param P                    a bitboard representing player
 @param O                    a bitboard representing opponent
 @return count all legal moves
 */
inline int RXBitBoard::get_mobility(const unsigned long long p_discs, const unsigned long long o_discs) {
    
    const unsigned long long legals = get_legal_moves(p_discs, o_discs);
    return __builtin_popcountll(legals)  +  __builtin_popcountll(legals & 0x8100000000000081ULL);
    
}


/// retourne un pseudo (sous evalué) score de pions stables
/// la 1ere partie determine les lignes (dans les 4 directions) pleines
/// si un pions est dans les 4 lignes et appartient a la color, il est stable
/// la deuxieme partie trouve les pions stables adgacents au pions stables precedents (dans les 4 directions)
/// - Parameters:
///   - player: joueur
inline int RXBitBoard::get_stability(const int p) const {
    return RXBitBoard::get_stability(discs[p], discs[p^1]);
}

__attribute__((always_inline))
inline int RXBitBoard::get_edge_stability(const int p) const {
    return __builtin_popcountll(RXBitBoard::get_stable_edge(discs[p], discs[p^1]));
}

/**
 * @brief Get stable edge.
 *
 * This function uses precomputed exact stable edge table to accelerate
 * the computation.
 *
 * @param P bitboard with player's discs.
 * @param O bitboard with opponent's discs.
 * @return a bitboard with (some of) player's stable discs.
 *
 */
__attribute__((always_inline))
inline unsigned long long RXBitBoard::get_stable_edge(const unsigned long long P, const unsigned long long O) {
    
    // compute the exact stable edges (from precomputed tables)
    return EDGE_STABILITY[(P & 0xffULL) * 256 + (O & 0xffULL)]
    |  (static_cast<unsigned long long>(EDGE_STABILITY[(P >> 56) * 256 + (O >> 56)])) << 56
    |  unpackA2A7(EDGE_STABILITY[packA1A8(P) * 256 + packA1A8(O)])
    |  unpackH2H7(EDGE_STABILITY[packH1H8(P) * 256 + packH1H8(O)]);
    
    
}


__attribute__((always_inline))
inline int RXBitBoard::get_corner_stability(const unsigned long long& discs_player) {
    
    unsigned long long stables = discs_player & 0x8100000000000081ULL;
    
    stables |= (discs_player & (stables << 1)) & 0x0200000000000002ULL;
    stables |= (discs_player & (stables >> 1)) & 0x4000000000000040ULL;
    stables |= (discs_player & (stables << 8)) & 0x0000000000008100ULL;
    stables |= (discs_player & (stables >> 8)) & 0x0081000000000000ULL;
    
    return __builtin_popcountll(stables);
    
}


inline int RXBitBoard::final_score_2(int alpha, const int beta) const {
    --n_nodes; // removes the duplicate
    return final_score_2(discs[player], discs[player^1], alpha, beta,  empties_list->next->position,  empties_list->next->next->position);
}


inline int RXBitBoard::final_score_3(int alpha, const int beta) const {
    
    unsigned int
    empties3  = (empties_list->next->position << 16);
    empties3 |= (empties_list->next->next->position << 8);
    empties3 |=  empties_list->next->next->next->position;
    
    --n_nodes; // removes the duplicate
    
    return final_score_3(discs[player], discs[player^1], alpha, beta, 0xE4UL, empties3);
}

//unroll
inline int RXBitBoard::final_score_3(const unsigned long long discs_player, const unsigned long long discs_opponent, int alpha, int beta, const unsigned int shuf3, const unsigned int empties3) const {
    
    int score, bestscore = UNDEF_SCORE;
    ++n_nodes;
    
    unsigned long long flipped;
        
    const int idSquare1 = (empties3 >> ((shuf3 & 0x30) >> 1)) & 0xFF;
    const int idSquare2 = (empties3 >> ((shuf3 & 0x0C) << 1)) & 0xFF;
    const int idSquare3 = (empties3 >> ((shuf3 & 0x03) * 8))  & 0xFF;

    
    if ((discs_opponent & NEIGHBOR[idSquare1]) && (flipped = do_flips[idSquare1](discs_player, discs_opponent))){
        
        bestscore = -final_score_2(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare1), -beta, -alpha, idSquare2, idSquare3);
        
        if(bestscore>=beta)
            return bestscore;
        
        if(bestscore>alpha)
            alpha = bestscore;
        
    }
    
    if ((discs_opponent & NEIGHBOR[idSquare2]) && (flipped = do_flips[idSquare2](discs_player, discs_opponent))){
        
        score = -final_score_2(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare2), -beta, -alpha, idSquare1, idSquare3);
        
        if(score>=beta)
            return score;
        
        if (score > bestscore) {
            bestscore = score;
            if (bestscore > alpha)
                alpha = bestscore;
        }
        
    }
    
    if ((discs_opponent & NEIGHBOR[idSquare3]) && (flipped = do_flips[idSquare3](discs_player, discs_opponent))){
        
        score = -final_score_2(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare3), -beta, -alpha, idSquare1, idSquare2);
        
        if(score > bestscore)
            return score;
        
        return bestscore;
    }
    
    if (bestscore == UNDEF_SCORE) {
                
        if ((discs_player & NEIGHBOR[idSquare1]) && (flipped = do_flips[idSquare1](discs_opponent, discs_player))){
            
            bestscore = -final_score_2(discs_player ^ flipped , discs_opponent ^ (flipped | 0x1ULL<<idSquare1), alpha, beta, idSquare2, idSquare3);
            
            if(bestscore>=-alpha)
                return -bestscore;
            
            if(bestscore>-beta)
                beta = -bestscore;
            
        }
        
        if ((discs_player & NEIGHBOR[idSquare2]) && (flipped = do_flips[idSquare2](discs_opponent, discs_player))){
            
            score = -final_score_2(discs_player ^ flipped , discs_opponent ^ (flipped | 0x1ULL<<idSquare2), alpha, beta, idSquare1, idSquare3);
            
            if(score>=-alpha)
                return -score;
            
            if (score > bestscore) {
                bestscore = score;
                if (bestscore > -beta)
                    beta = -bestscore;
            }
            
            
        }
        
        if ((discs_player & NEIGHBOR[idSquare3]) && (flipped = do_flips[idSquare3](discs_opponent, discs_player))){
            
            score = -final_score_2(discs_player ^ flipped , discs_opponent ^ (flipped | 0x1ULL<<idSquare3), alpha, beta, idSquare1, idSquare2);
            
            if(score > bestscore)
                return -score;
        }
        
        if(bestscore == UNDEF_SCORE) {
            bestscore = 61-2*__builtin_popcountll(discs_player);
            if(bestscore>0)
                bestscore+=3;
            else
                bestscore-=3;
        }
        
        
        bestscore = -bestscore;
    }
    
    
    return bestscore;
}

/// final score at 2 empties
/// - Parameters:
///   - alpha: alpha bound
///   - beta: beta bound
///   - passed: true if  previous move is pass
inline int RXBitBoard::final_score_4(int alpha, int beta, const bool passed) const {
    
#ifdef USE_STABILITY
    
    int diff_discs = (2*__builtin_popcountll(discs[player]) - 60);
    
    if (beta >= 6|| (beta >= 0 && (diff_discs <= beta - 6))) {
        
        int stability_bound = 64- 2 * get_stability(player^1);
        if ( stability_bound <= alpha )
            return stability_bound; //alpha
        
        if ( stability_bound < beta )
            beta = stability_bound;
        
        
    } else  if (alpha <= -6|| (alpha <= 0 && (-diff_discs <= alpha + 6))) {
        
        int stability_bound = 2 * get_stability(player) - 64;
        if ( stability_bound >= beta )
            return stability_bound; //beta
        
    }

#endif
    
    
    
    const int sq_1 = empties_list->next->position;
    const int sq_2 = empties_list->next->next->position;
    const int sq_3 = empties_list->next->next->next->position;
    const int sq_4 = empties_list->next->next->next->next->position;
    
    //Sorting on the parity (4 empties square) **Bright
    
    // parity sort
    static const unsigned char parity_case[64] = {    /* idQuadrant sq_4sq_3sq_2sq_1 = */
        /*0000*/  0, /*0001*/  0, /*0010*/  1, /*0011*/  9, /*0100*/  2, /*0101*/ 10, /*0110*/ 11, /*0111*/  3,
        /*0002*/  0, /*0003*/  0, /*0012*/  0, /*0013*/  0, /*0102*/  4, /*0103*/  4, /*0112*/  5, /*0113*/  5,
        /*0020*/  1, /*0021*/  0, /*0030*/  1, /*0031*/  0, /*0120*/  6, /*0121*/  7, /*0130*/  6, /*0131*/  7,
        /*0022*/  9, /*0023*/  0, /*0032*/  0, /*0033*/  9, /*0122*/  8, /*0123*/  0, /*0132*/  0, /*0133*/  8,
        /*0200*/  2, /*0201*/  4, /*0210*/  6, /*0211*/  8, /*0300*/  2, /*0301*/  4, /*0310*/  6, /*0311*/  8,
        /*0202*/ 10, /*0203*/  4, /*0212*/  7, /*0213*/  0, /*0302*/  4, /*0303*/ 10, /*0312*/  0, /*0313*/  7,
        /*0220*/ 11, /*0221*/  5, /*0230*/  6, /*0231*/  0, /*0320*/  6, /*0321*/  0, /*0330*/ 11, /*0331*/  5,
        /*0222*/  3, /*0223*/  5, /*0232*/  7, /*0233*/  8, /*0322*/  8, /*0323*/  7, /*0332*/  5, /*0333*/  3
    };

    
    static const unsigned int sort4_shuf[] = {
        0x3978b4e4,    //  0: 1(x1) 3(x2 x3 x4),      1(x1) 1(x2) 2(x3 x4), 1 1 1 1, 4        x4x1x2x3-x3x1x2x4-x2x1x3x4-x1x2x3x4
        0x3978e4b4,    //  1: 1(x2) 3(x1 x3 x4)       x4x1x2x3-x3x1x2x4-x1x2x3x4-x2x1x3x4
        0x39b4e478,    //  2: 1(x3) 3(x1 x2 x4)       x4x1x2x3-x2x1x3x4-x1x2x3x4-x3x1x2x4
        0x78b4e439,    //  3: 1(x4) 3(x1 x2 x3)       x3x1x2x4-x2x1x3x4-x1x2x3x4-x4x1x2x3
        0x39b478d8,    //  4: 1(x1) 1(x3) 2(x2 x4)    x4x1x2x3-x2x1x3x4-x3x1x2x4-x1x3x2x4
        0x78b439c9,    //  5: 1(x1) 1(x4) 2(x2 x3)    x3x1x2x4-x2x1x3x4-x4x1x2x3-x1x4x2x3
        0x39e46c9c,    //  6: 1(x2) 1(x3) 2(x1 x4)    x4x1x2x3-x1x2x3x4-x3x2x1x4-x2x3x1x4
        0x78e42d8d,    //  7: 1(x2) 1(x4) 2(x1 x3)    x3x1x2x4-x1x2x3x4-x4x2x1x3-x2x4x1x3
        0xb4e41e4e,    //  8: 1(x3) 1(x4) 2(x1 x2)    x2x1x3x4-x1x2x3x4-x4x3x1x2-x3x4x1x2
        0x1e4eb4e4,    //  9: 2(x1 x2) 2(x3 x4)       x4x3x1x2-x3x4x1x2-x2x1x3x4-x1x2x3x4
        0x2d788dd8,    // 10: 2(x1 x3) 2(x2 x4)       x4x2x1x3-x3x1x2x4-x2x4x1x3-x1x3x2x4
        0x396c9cc9     // 11: 2(x1 x4) 2(x2 x3)       x4x1x2x3-x3x2x1x4-x2x3x1x4-x1x4x2x3
    };

    // parity based move sorting.
    // The following hole sizes are possible:
    // 4 - 1 3 - 2 2 - 1 1 2 - 1 1 1 1
    // the 1 1 2 case needs move sorting on this ply.
    // prefer 1 empty over 3 empties, 1 3 case also needs sorting.
    int paritysort = parity_case[((sq_3 ^ sq_4) & 0x24) + (((sq_2 ^ sq_4) & 0x24) >> 1) + (((sq_1 ^ sq_4) & 0x24) >> 2)];
    unsigned int shuf4 = sort4_shuf[paritysort];
    unsigned int empties4 = (sq_1 << 24) | (sq_2 << 16) | (sq_3 << 8) | sq_4;

    
    
    
    return final_score_4(discs[player], discs[player^1], alpha, beta, passed, shuf4, empties4);
}

inline int RXBitBoard::final_score_4(const unsigned long long discs_player, const unsigned long long discs_opponent, int alpha, const int beta, const bool passed, const unsigned int shuf4, const unsigned int empties4) const {
    
    
    int score, bestscore = UNDEF_SCORE;
    
    unsigned long long flipped;
    
    int idSquare1 = (empties4 >> ((shuf4 >> (6 - 3)) & 0x18)) & 0xFF;
    if ((discs_opponent & NEIGHBOR[idSquare1]) && (flipped = do_flips[idSquare1](discs_player, discs_opponent))){
        
        bestscore = -final_score_3(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare1), -beta, -alpha, shuf4, empties4);
        
        if(bestscore>=beta)
            return bestscore;
        
        if(bestscore>alpha)
            alpha = bestscore;
        
        
    }
    
    int idSquare2 = (empties4 >> ((shuf4 >> (14 - 3)) & 0x18)) & 0xFF;
    if ((discs_opponent & NEIGHBOR[idSquare2]) && (flipped = do_flips[idSquare2](discs_player, discs_opponent))){
        
        score = -final_score_3(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare2), -beta, -alpha, shuf4>>8, empties4);
        
        if(score>=beta)
            return score;
        
        if (score > bestscore) {
            bestscore = score;
            if (bestscore > alpha)
                alpha = bestscore;
        }
        
        
    }
    
    int idSquare3 = (empties4 >> ((shuf4 >> (22 - 3)) & 0x18)) & 0xFF;
    if ((discs_opponent & NEIGHBOR[idSquare3]) && (flipped = do_flips[idSquare3](discs_player, discs_opponent))) {
        
        score = -final_score_3(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare3), -beta, -alpha, shuf4>>16, empties4);
        
        if(score>=beta)
            return score;
        
        if (score > bestscore) {
            bestscore = score;
            if (bestscore > alpha)
                alpha = bestscore;
        }
        
        
    }
    
    int idSquare4 = (empties4 >> ((shuf4 >> 30) * 8)) & 0xFF;
    if ((discs_opponent & NEIGHBOR[idSquare4]) && (flipped = do_flips[idSquare4](discs_player, discs_opponent))){
        
        score = -final_score_3(discs_opponent ^ flipped, discs_player ^ (flipped | 0x1ULL<<idSquare4), -beta, -alpha, shuf4>>24, empties4);
        
        if(score > bestscore)
            return score;
        
        return bestscore;
        
    }
    
    // if no _move4 were available
    if(bestscore == UNDEF_SCORE) {
        if (passed) {
            bestscore = 60-2*__builtin_popcountll(discs_opponent);
            if(bestscore>0)
                bestscore+=4;
            else if (bestscore<0)
                bestscore-=4;
            
        } else {
            bestscore = -final_score_4(discs_opponent, discs_player, -beta, -alpha, true, shuf4, empties4);
        }
    }
    
    return bestscore;
}




#endif
