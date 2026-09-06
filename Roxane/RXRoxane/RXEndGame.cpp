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

#include <iostream>
#include <cstdint>
#include <bit>


#include "RXEngine.hpp"
#include "RXRoxane.hpp"
#include "RXBitBoard.hpp"
#include "RXSetting.hpp"


// ─────────────────────────────────────────────────────────────────────────────
// LocalTT — table de transposition locale par thread, sans spinlock
// Adapted from acepck's pull request for Egaroucid.
//
// Couvre les Methodes EG_alphabeta_parity < n_empty <= EG_PVS_ETC_LTT
// soit typiquement 5 < n_empty <= 13
//
// Dimensionnement :
//   DEPTH_RANGE = EG_MEDIUM_HI_TO_LOW_MAX - EG_MEDIUM_TO_SHALLOW_MAX -1
//               = 14 - 7 - 1 = 8 niveaux  (indices 0..7 pour n_empty 6..13)
//   SIZE        = 2048 entrées par niveau (~384 ko par thread, 0 contention)
// ─────────────────────────────────────────────────────────────────────────────

// RXEngine constants mirror (required for compilation)
static constexpr int LTT_END_FAST  = RXEngine::EG_MEDIUM_TO_SHALLOW-1; // 7-1 = 6
static constexpr int LTT_END_DEEP  = RXEngine::EG_MEDIUM_HI_TO_LOW;  // 14

struct LocalTT {

    static constexpr int DEPTH_RANGE = LTT_END_DEEP - LTT_END_FAST; // 8 niveaux : 6-13 cases vides
    static constexpr int SIZE        = 2048;
    static constexpr int MASK        = SIZE - 1;

    //24 bytes (18 data + 6 padding)
    struct Entry {
        unsigned long long player   = 0;
        unsigned long long opponent = 0;
        int8_t   lower    = -64;
        int8_t   upper    =  64;
    };

    // Défini dans .cpp
    thread_local static Entry table[DEPTH_RANGE][SIZE];
    
    struct LookupResult {
        Entry*  entry;      // nullptr = miss
        int     alpha;      // alpha potentiellement resserré
        int     beta;       // beta  potentiellement resserré
        bool    cutoff;     // true = retourner score immédiatement
        int     score;      // score à retourner si cutoff == true
    };

    // Retourne l'entrée si cache hit, nullptr si cache miss
    static Entry* lookup(const unsigned long long P, const unsigned long long O, const unsigned int n_empty,  const unsigned long long hash) {
        const int idx = n_empty - LTT_END_FAST;
//        assert(idx >= 0 && idx < DEPTH_RANGE);
        Entry* e = &table[idx][hash & MASK];
        return (e->player == P &&
                e->opponent == O) ? e : nullptr;
    }

    // Retourne l'entrée si hit, nullptr si miss
    static LookupResult probe(const RXBitBoard& board,
                                     const unsigned long long hash,
                                     const int alpha, const int beta) {
        
        Entry* e = lookup(board.discs[board.player], board.discs[board.player^1], board.n_empty, hash);
        if (!e)
            return {nullptr, alpha, beta, false, 0};

        if (e->lower == e->upper) return {e, alpha, beta, true,  e->lower};
        if (e->lower >= beta)     return {e, alpha, beta, true,  e->lower};
        if (e->upper <= alpha)    return {e, alpha, beta, true,  e->upper};

        int new_alpha = (e->lower > alpha) ? e->lower : alpha;
        int new_beta  = (e->upper < beta)  ? e->upper : beta;
        return {e, new_alpha, new_beta, false, 0};
    }

    // Retourne l'entrée si hit, nullptr si miss
    static LookupResult probe(const unsigned long long P,
                              const unsigned long long O,
                              const unsigned int n_empty,
                              const unsigned long long hash,
                              const int alpha, const int beta) {
        
        Entry* e = lookup(P, O, n_empty, hash);
        if(e)
            return {e, e->lower, e->upper, false, 0};
        return {nullptr, alpha, beta, false, 0};


    }

    // e == nullptr → collision (reset + réécriture), sinon mise à jour incrémentale
    // Passer alpha originale (avant resserrement), beta courant
    static void store(Entry* e, unsigned long long hash,
                      const RXBitBoard& board, int score, int alpha, int beta) {
        if (!e) {
            const int idx = board.n_empty - LTT_END_FAST;
            e = &table[idx][hash & MASK];
            e->player   = board.discs[board.player];
            e->opponent = board.discs[board.player^1];
            e->lower    = -64;
            e->upper    =  64;
        }
        if (score <= alpha) {
            if (score < e->upper) e->upper = static_cast<int8_t>(score);
        } else if (score >= beta) {
            if (score > e->lower) e->lower = static_cast<int8_t>(score);
        } else {
            e->lower = e->upper = static_cast<int8_t>(score);
        }
    }
};


// ─────────────────────────────────────────────
// Définition obligatoire du stockage thread_local
// (une seule TU, sinon linker error)
// ─────────────────────────────────────────────
thread_local LocalTT::Entry LocalTT::table[LocalTT::DEPTH_RANGE][LocalTT::SIZE];


const int RXEngine::stability_threshold[] =
{   65, 65, 65, 65,  6,  8, 10, 14,
    16, 18, 22, 24, 26, 28, 30, 32,
    34, 36, 38, 40, 40, 42, 42, 44,
    44, 46, 46, 48, 48, 50, 52, 52,
    54, 54, 56, 56, 58, 58, 60, 60,
    62, 62, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65};




/*!
 * \brief  Evaluate a position using a shallow Alphabeta.
 *
 * This function is used when there are 5 or 6 empty squares on the board. Here,
 * optimizations are in favour of speed instead of efficiency. A simple
 * alphabeta is used because of the low branching factor that makes PVS less
 * efficient.
 * \param board   board.
 * \param alpha   lower bound.
 * \param beta    upper bound.
 * \param passed  a flag indicating if previous move was a pass.
 * \return        the final score, as a disc difference.
 */

int RXEngine::EG_alphabeta_parity(const unsigned int threadID, RXBitBoard& board, int alpha, int beta, const bool passed) {
    
    int score, bestscore = UNDEF_SCORE;
    
#ifdef USE_STABILITY
    if ( !passed && beta >= stability_threshold[board.n_empty]) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= alpha )
            return stability_bound;
        if ( stability_bound < beta )
            beta = stability_bound;
    }
#endif
    
    
    unsigned long long legal_movesBB = board.get_legal_moves();
    
    if(legal_movesBB) {
               
        if(board.n_empty == 5) {
            
            const unsigned long long current_P = board.discs[board.player];
            const unsigned long long current_O = board.discs[board.player^1];
            
            if(std::popcount(legal_movesBB) == 1) { //only 1 move
                
                const int pos = std::countr_zero(legal_movesBB);  // Get the index of the lowest set bit

                const unsigned long long flipped = board.do_flips(pos, current_P, current_O);
                
                const unsigned long long next_O = current_P | (flipped | legal_movesBB);
                const unsigned long long next_P = current_O ^  flipped;
                ++board.n_nodes;
                
#ifdef USE_ENHANCED_STABLILITY
                    
                int diff_discs = 60 - 2*std::popcount(next_P);
                    
                if ((alpha <= -stability_threshold[4]) || (alpha <= 0 && (diff_discs >= (alpha + 6)))){
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound >= beta ) {
                        return stability_bound;
                    }
                }
                    
#endif

                RXSquareList *empties = board.position_to_empties[pos];
                empties->previous->next = empties->next;
                bestscore = -board.final_score_4(next_P, next_O, -beta, -alpha);
                empties->previous->next = empties;

                
            } else {
                
                // Lambda declaration (zero overhead if inlined)
                auto move_final_score_5 = [&] RX_LAMBDA_INLINE (RXSquareList* empties, int pos, unsigned long long bit) -> bool {
                    
                    const unsigned long long flipped = board.do_flips(pos, current_P, current_O);
                    
                    const unsigned long long next_O = current_P | (flipped | bit);
                    const unsigned long long next_P = current_O ^  flipped;
                    ++board.n_nodes;
                    
#ifdef USE_ENHANCED_STABLILITY
                    
                    int diff_discs = 60 - 2*std::popcount(next_P);
                    
                    if ((alpha <= -stability_threshold[4]) || (alpha <= 0 && (diff_discs >= (alpha + 6)))){
                        score = 2 * board.get_stability(next_O, next_P) - 64;
                        if ( score >= beta ) {
                            return true;
                        }
                    }
                    
#endif
                    
                    empties->previous->next = empties->next;
                    score = -board.final_score_4(next_P, next_O, -beta, -alpha);
                    empties->previous->next = empties;
                    
                    if (score >= beta)
                        return true; // Beta cut-off
                    
                    if (score > bestscore) {
                        bestscore = score;
                        if (bestscore > alpha)
                            alpha = bestscore;
                    }
                    
                    return false;
                };
                
                // scanning empty squares once and deferring even-parity ones looks appealing
                RXSquareList* deferred[4]; // deferred max 4 (1-4)
                int n_deferred = 0;
                
                // Precompute the inverted parity-quadrant mask once here
                const unsigned long long parity_movesBB = ~RXBitBoard::QUADRANT_MASK[board.parity];
                
                // First loop: odd parity case
                for (RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;
                    
                    if (~legal_movesBB & bit) continue;
                    legal_movesBB ^= bit;
                    
                    // Parity check for deferral
                    if (parity_movesBB & bit) {
                        deferred[n_deferred++] = empties;
                        continue;
                    }
                    
                    if (move_final_score_5(empties, pos, bit))
                        return score;
                    
                }
                
                // second loop: even parity case
                for (int i = 0; i < n_deferred; i++) {
                    RXSquareList* empties = deferred[i];
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;
                    
                    if (move_final_score_5(empties, pos, bit))
                        return score;
                }
           }

        } else {
            
            RXMove& move = threads[threadID]._move[board.n_empty][1];

            if(board.parity == 0 || board.parity == 0xF) {
                
                // Enforce JWC sort order—this is the only ordering.
                RXSquareList* empties = board.empties_list;
                do {
                    empties = empties->next;
                    
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if (legal_movesBB & bit) {
                        legal_movesBB ^= bit;
                        
                        board.generate_flips(pos, move);
                        
                        board.do_move(move);
                        score = -EG_alphabeta_parity(threadID, board, -beta, -alpha, false);
                        board.undo_move(move);
                        
                        if (score >= beta)
                            return score;
                        
                        if (score > bestscore) {
                            bestscore = score;
                            if (bestscore > alpha) {
                                alpha = bestscore;
                            }
                        }
                    }
                } while(legal_movesBB);
                
            } else {
                
                unsigned long long parity_movesBB = RXBitBoard::QUADRANT_MASK[board.parity];
                
                for (int parity = 1; parity >= 0; parity--) {
                    unsigned long long movesBB = legal_movesBB & parity_movesBB;
                    
                    // Enforce JWC sort order—this is the only ordering.
                    for(RXSquareList* empties = board.empties_list->next; movesBB; empties = empties->next) {
                        const int pos = empties->position;
                        const unsigned long long bit = 0x1ULL << pos;

                        if (movesBB & bit) {
                            movesBB ^= bit;
                            
                            board.generate_flips(pos, move);
                            
                            board.do_move(move);
                            score = -EG_alphabeta_parity(threadID, board, -beta, -alpha, false);
                            board.undo_move(move);
                            
                            if (score >= beta)
                                return score;
                            
                            if (score > bestscore) {
                                bestscore = score;
                                if (bestscore > alpha) {
                                    alpha = bestscore;
                                }
                            }
                        }
                    }
                    
                    parity_movesBB = ~parity_movesBB;
                }
            }
        }
        
    } else {
        
        if(passed) {
            bestscore = board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_alphabeta_parity(threadID, board, -beta, -alpha, true);
            board.do_pass();
        }
    }
    
    return bestscore;
}


//depth == 6 empty
//Non recursive, called only one
int RXEngine::EG_alphabeta_LTT(const unsigned int threadID, RXBitBoard& board, const bool pv, const int alpha, const int beta, const bool passed) {


    int lower = alpha;
    int upper = beta;
    
#ifdef USE_STABILITY
    if ( !passed && upper >= stability_threshold[board.n_empty] ) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= lower )
            return stability_bound;
        
        if ( stability_bound < upper )
            upper = stability_bound;
        
    }
#endif
    
    const unsigned long long  hash_code = board.hashcode();

    // TT local (no lock)
    auto ltt = LocalTT::probe(board, hash_code, lower, upper);
    if (!pv && ltt.cutoff) return ltt.score;

    int bestscore = UNDEF_SCORE;
//    unsigned int bestmove = NOMOVE;

    //for all empty square
    unsigned long long legal_movesBB = board.get_legal_moves();
    
    if(legal_movesBB) {
        
        RXMove* list = threads[threadID]._move[board.n_empty];
        RXMove* move = list + 1;
        
        if(std::popcount(legal_movesBB) == 1) { //only 1 move
            
            const int pos = std::countr_zero(legal_movesBB);  // Get the index of the lowest set bit

            board.generate_flips(pos, *move);
//            bestmove = move->position;
            
            board.do_move(*move);
            bestscore = -EG_alphabeta_parity(threadID, board, -upper, -lower, false);
            board.undo_move(*move);

            
        } else { //multiple moves : prepare for sort
            
            RXMove* previous = list;

            const unsigned long long current_P = board.discs[board.player];
            const unsigned long long current_O = board.discs[board.player^1];
            
            const int etc_depth = board.n_empty - 1;
            
            // Skipping JWC order is faster, but JWC tie-breaking for equal scores is lost.
            do {
                
                const int pos = std::countr_zero(legal_movesBB); // Get the index of the lowest set bit
                legal_movesBB &= legal_movesBB - 1;             // Clear the lowest set bit

                board.generate_flips(pos, *move);
                
                const unsigned long long next_O = current_P | (move->flipped | move->square);
                const unsigned long long next_P = current_O ^ move->flipped;
                ++board.n_nodes;
                
#ifdef USE_ENHANCED_STABLILITY
                if (lower <= -stability_threshold[etc_depth]) {
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound >= upper ) {
                        LocalTT::store(ltt.entry, hash_code, board, stability_bound, alpha, upper);
                        return stability_bound;
                    }
                }
#endif

                move->score =
                  (RXBitBoard::count_potential_moves(next_P, next_O)<<4)
                - (RXBitBoard::get_edge_stability(next_O, next_P)<<2)
                - (((board.parity & RXBitBoard::QUADRANT_ID[pos])>>RXBitBoard::QUADRANT_SHIFT[pos])<<3)
                + ((move->square & 0x8100000000000081) != 0);
                
                previous = previous->next = move++;
                
            } while (legal_movesBB);
            
            previous->next = nullptr;

            do {
                
                move = list->next;
                if(move->next != nullptr)
                    move = list->pick_next_promising_move();
                
                board.do_move(*move);
                int score = -EG_alphabeta_parity(threadID, board, -upper, -lower, false);
                board.undo_move(*move);
                
                if (score > bestscore) {
//                    bestmove = move->position;
                    bestscore = score;
                    if (bestscore > lower)
                        lower = bestscore;
                }
                
                list = list->next;
                
            } while(lower < upper && list->next != nullptr);

        }
        
        
    } else  {
        
        if (passed) {
            return board.final_score();
         } else {
//            bestmove = PASS;
            board.do_pass();
            bestscore = -EG_alphabeta_LTT(threadID, board, pv, -upper, -lower, true);
            board.do_pass();
        }
    }
        
    LocalTT::store(ltt.entry, hash_code, board, bestscore, alpha, upper);

    return bestscore;
}

/*!
 * \brief Evaluate a position with a deep Principal Variation Search algorithm.
 *
 * This function is used when there are still many empty squares on the board. Move
 * ordering, hash table cutoff, etc. are used in
 * order to diminish the size of the tree to analyse, but at the expense of a
 * slower speed.
 *
 * \param board      board.
 * \param alpha      lower bound.
 * \param beta       upper bound.
 * \param passed     a flag indicating if previous move was a pass.
 * \return the final score, as a disc difference.
 */

//n_empty =< 13 empty
int RXEngine::EG_PVS_ETC_LTT(const unsigned int threadID, RXBitBoard& board, const bool pv, const int alpha, const int beta, const bool passed)
{
    
    if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
        return EG_alphabeta_LTT(threadID, board, pv, alpha, beta, passed);

    
    int lower = alpha;
    int upper = beta;
    
        
#ifdef USE_STABILITY
    /*
     calculated stability is less than or equal to the real stability
     stability_bound is overestimated.
     score_max <= Stability_bound <= alpha ==> cutoff
     &
     score_max<=stability_bound < beta  ==> adjustment search window
     */
    
    
    if ( !passed && upper >= stability_threshold[board.n_empty] ) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= lower )
            return stability_bound;
        
        if ( stability_bound < upper )
            upper = stability_bound;
        
    }
#endif
    
    const unsigned long long  hash_code = board.hashcode();

    // TT local (no lock)
    auto ltt = LocalTT::probe(board, hash_code, lower, upper);
    if (!pv && ltt.cutoff) return ltt.score;

    int bestscore = UNDEF_SCORE;

    //for all empty square
    unsigned long long legal_movesBB = board.get_legal_moves();
    
    if(legal_movesBB) {
                
        RXMove* list = threads[threadID]._move[board.n_empty];
        RXMove* move = list + 1;
        
        if(std::popcount(legal_movesBB) == 1) { //only 1 move
            
            const int pos = std::countr_zero(legal_movesBB);  // Get the index of the lowest set bit

            board.generate_flips(pos, *move);
            
            board.do_move(*move);
            bestscore = -EG_PVS_ETC_LTT(threadID, board, pv, -upper, -lower, false);
            board.undo_move(*move);

            
        } else { //multiple moves : prepare for sort
            
            
            RXMove* previous = list;
            
            const unsigned long long current_P = board.discs[board.player];
            const unsigned long long current_O = board.discs[board.player^1];
            const int etc_depth = board.n_empty-1;
            
            // Skipping JWC order is faster, but JWC tie-breaking for equal scores is lost.
            do {
                const int pos = std::countr_zero(legal_movesBB);  // Get the index of the lowest set bit
                legal_movesBB &= legal_movesBB - 1;              // Clear the lowest set bit
                
                board.generate_flips(pos, *move);
                
                //ETC LTT
                const unsigned long long next_O = current_P | (move->flipped | move->square);
                const unsigned long long next_P = current_O ^ move->flipped;
                ++board.n_nodes;
                
                
#ifdef USE_ENHANCED_STABLILITY
                if (lower <= -stability_threshold[etc_depth]) {
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound >= upper ) {
                        LocalTT::store(ltt.entry, hash_code, board, stability_bound, alpha, upper);
                        return stability_bound;
                    }
                }
#endif
                                                
#ifdef USE_ETC
                if(!pv) { //NWS
                    
                    const unsigned long long next_hashcode = board.hashcode(next_P, next_O);
                    
                    auto etc_ltt = LocalTT::probe(next_P, next_O, etc_depth, next_hashcode, lower, upper);
                    if (etc_ltt.entry) {
//                        if(-etc_ltt.beta>lower) {
                            if(-etc_ltt.beta>=upper) {
                                LocalTT::store(ltt.entry, hash_code, board, -etc_ltt.beta, alpha, upper);
                                return -etc_ltt.beta;
                            }
//                            lower = -etc_ltt.beta;
//                            bestscore = -etc_ltt.beta;
//                        }
                        //remove from list
                        else if(-etc_ltt.alpha <= lower) {
                            if(bestscore < -etc_ltt.alpha)
                                bestscore = -etc_ltt.alpha;
                            continue;
                        }
                    }
                }
#endif
                
                previous = previous->next = move++;
                
            } while (legal_movesBB);
            
            
            previous->next = nullptr;
            
            // Tous les coups éliminés par ETC
            if(previous == list && bestscore != UNDEF_SCORE) {
                LocalTT::store(ltt.entry, hash_code, board, bestscore, alpha, upper);
                return bestscore;
            }
            
            move = list->next;

            if(move->next != nullptr) { //nb moves > 1
                
                //sort list by mobility
                for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
                    
                    ++board.n_nodes;
                    
                    const unsigned long long next_O = current_P | (iter->flipped | iter->square);
                    const unsigned long long next_P = current_O ^ iter->flipped;
                    
                    iter->score =
                    (RXBitBoard::get_mobility(next_P, next_O)<<5)
                    + (RXBitBoard::count_potential_moves(next_P, next_O))
                    - (RXBitBoard::get_edge_stability(next_O, next_P)<<2)
                    - ((board.parity & RXBitBoard::QUADRANT_ID[iter->position])>>RXBitBoard::QUADRANT_SHIFT[iter->position]);
                    
                }
                
                move = list->pick_next_promising_move();
            }
            
                        
            board.do_move(*move);
            int score = -EG_PVS_ETC_LTT(threadID, board, pv, -upper, -lower, false);
            board.undo_move(*move);
            
            if (score > bestscore) {
                bestscore = score;
                if (bestscore > lower)
                    lower = bestscore;
            }

            // next move
            list = list->next;
            
            
            
            // other moves : try to refute the first/best one
            for(; lower < upper && list->next != nullptr; list = list->next) {
                
                move = list->next;
                if(move->next != nullptr)
                    move = list->pick_next_promising_move();
                
                board.do_move(*move);
                
                score = -EG_PVS_ETC_LTT(threadID, board, false, -lower-1, -lower, false);
                if (lower < score && score < upper)
                    score = -EG_PVS_ETC_LTT(threadID, board, pv, -upper, -score, false);
                
                board.undo_move(*move);
                
                if (score > bestscore) {
                    bestscore = score;
                    if (bestscore > lower)
                        lower = bestscore;
                }
            }
        }
        
    } else {
        
        if (passed) {
            return board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_PVS_ETC_LTT(threadID, board, pv, -upper, -lower, true);
            board.do_pass();
        }
    }


    LocalTT::store(ltt.entry, hash_code, board, bestscore, alpha, upper);

    return bestscore;
}

/*!
 * \brief Evaluate a position with a deep Principal Variation Search algorithm.
 *
 * This function is used when there are still many empty squares on the board. Move
 * ordering, hash table cutoff, enhanced transposition cutoff, etc. are used in
 * order to diminish the size of the tree to analyse, but at the expense of a
 * slower speed.
 *
 * \param board     othellier
 * \param alpha      lower bound.
 * \param beta       upper bound.
 * \param passed     a flag indicating if previous move was a pass.
 * \return the final score, as a disc difference.
 */

// Implementing a skip move on TT in ETC is more complicated without any real benefit.
int RXEngine::EG_PVS_ETC_mobility(const unsigned int threadID, RXBitBoard& board, const bool pv, const int alpha, const int beta, const bool passed)
{
    

    if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
        return EG_PVS_ETC_LTT(threadID, board, pv, alpha, beta, passed);

     
    if(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    const unsigned long long  hash_code = board.hashcode();

    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
    
    RXHashValue entry;
    if(hTable->get(hash_code, board, type_hashtable, entry)) {
        
        if(!pv && entry.selectivity == NO_SELECT && entry.depth >= board.n_empty) {
            
            if (entry.lower > lower) {
                lower = entry.lower;
                if (lower >= upper)
                    return lower;
            }
            if (entry.upper < upper) {
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            
        }
        
        bestmove = entry.move;

    }
    
#ifdef USE_STABILITY
    
    if(!passed && bestmove == NOMOVE) {
        
        /*
         calculated stability is less than or equal to the real stability
         stability_bound is overestimated.
         score_max <= Stability_bound <= alpha ==> cutoff
         &
         score_max<=stability_bound < beta  ==> adjustment search window
         */
        if (  upper >= stability_threshold[board.n_empty] ) {
            
            int stability_bound = 64- 2 * board.get_stability(board.player^1);
            if ( stability_bound <= lower )
                return stability_bound;
            
            if ( stability_bound < upper )
                upper = stability_bound;
            
        }
        
    }
    
#endif
    
    int bestscore = UNDEF_SCORE;
    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;
    
    const unsigned long long current_P = board.discs[board.player];
    const unsigned long long current_O = board.discs[board.player^1];

    
    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;
        
        
        const int etc_depth = board.n_empty-1;
        
        
#ifdef USE_ETC
        unsigned long long  next_hashcode;
#endif
        
        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);
            ++board.n_nodes;
            
            const unsigned long long next_P = current_O ^ move->flipped;
            const unsigned long long next_O = current_P | (move->flipped | move->square);
            
            
            
#ifdef USE_ENHANCED_STABLILITY
            if (lower <= -stability_threshold[etc_depth]) {
                int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                if ( stability_bound >= upper ) {
//                    hTable->update(hash_code, board, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, stability_bound, bestmove);
                    return stability_bound;
                }
//                if (stability_bound > lower) {
//                    lower = stability_bound;
//                }
            }
#endif
                        
#ifdef USE_ETC
            
            
            if(!pv && board.n_empty != EG_MEDIUM_HI_TO_LOW) {
                
                next_hashcode = board.hashcode(next_P, next_O);
                
                //synchronized acces
                if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity == NO_SELECT && entry.depth>=etc_depth) {
                    
                    if(-entry.upper >= upper) {
//                        hTable->update(hash_code, board, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, -entry.upper, bestmove);
                        return -entry.upper;
                    }
                    
                }
            }
            
#endif
            
            previous = previous->next = move++;
        }
        
        //for all empty square
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove != NOMOVE) {
            legal_movesBB ^= 0x1ULL<<bestmove;
        }
        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;
            
            if(legal_movesBB & bit){
                legal_movesBB ^= bit;
                
                board.generate_flips(pos, *move);
                ++board.n_nodes;
                
                const unsigned long long next_P = current_O ^ move->flipped;
                const unsigned long long next_O = current_P | (move->flipped | move->square);
                
                
#ifdef USE_ENHANCED_STABLILITY
                if (lower <= -stability_threshold[etc_depth]) {
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound >= upper ) {
//                        hTable->update(hash_code, board, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, stability_bound, pos);
                        return stability_bound;
                    }
//                    if (stability_bound > lower) {
//                        lower = stability_bound;
//                    }
                }
#endif
                
                move->score = 0;
                                
#ifdef USE_ETC
                if(board.n_empty != EG_MEDIUM_HI_TO_LOW) {
                    
                    next_hashcode = board.hashcode(next_P, next_O);
                    
                    //synchronized acces
                    if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.depth>=etc_depth) {

                        if(-entry.upper >= upper) {
                            if(!pv && entry.selectivity == NO_SELECT) {
//                                hTable->update(hash_code, board, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, -entry.upper, pos);
                                return -entry.upper;
                            }
                            move->score = -(2<<4);
                        }
                        
                    }
                }
#endif
                
                previous = previous->next = move++;
                
            }
        }
        
        previous->next = nullptr;
    }

    
    
    if (list->next == nullptr) {
        if (passed) {
            return board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_PVS_ETC_mobility(threadID, board, pv, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    } else {
        
        
        if(bestmove != NOMOVE) {
            /* first move */
            list = list->next;
            
            board.do_move(*list);
            bestscore = -EG_PVS_ETC_mobility(threadID, board, pv, -upper, -lower, false);
            board.undo_move(*list);
            
            //bestmove = list->position;
            if (bestscore > lower)
                lower = bestscore;
            
        }
        
        
        if(lower < upper && list->next != nullptr) {
            
            if((list->next)->next != nullptr) {
                
                //calc answer move->score
                for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
                    
                    ++board.n_nodes;
                    
                    const unsigned long long next_O = current_P | (iter->flipped | iter->square);
                    const unsigned long long next_P = current_O ^ iter->flipped;
                    
                    //score for try :
                    iter->score +=
                      (RXBitBoard::get_mobility(next_P, next_O)<<4)
                    - (RXBitBoard::get_edge_stability(next_O, next_P)<<2)
                    - ((board.parity & RXBitBoard::QUADRANT_ID[iter->position])>>RXBitBoard::QUADRANT_SHIFT[iter->position]);
                    
                }
                
            }
            
            if(bestmove == NOMOVE) {
                
                //first best moves 
                RXMove* move = list->pick_next_promising_move();
                
                board.do_move(*move);
                bestscore = -EG_PVS_ETC_mobility(threadID, board, pv, -upper, -lower, false);
                board.undo_move(*move);
                
                bestmove = move->position;
                if (bestscore > lower)
                    lower = bestscore;
                
                // next move
                list = list->next;
                
            }
            
            
            // other moves : try to refute the first/best one
            int score;
            for(;lower < upper && list->next != nullptr; list = list->next) {
                
                //find moves with worst answer
                RXMove* move = list->next;
                if(move->next != nullptr)
                    move = list->pick_next_promising_move();

                board.do_move(*move);
                score = -EG_PVS_ETC_mobility(threadID, board, false, -lower-1, -lower, false);
                if (lower < score && score < upper)
                    score = -EG_PVS_ETC_mobility(threadID, board, pv, -upper, -score, false);
                board.undo_move(*move);
                
                if (score > bestscore) {
                    bestscore = score;
                    bestmove = move->position;
                    if (bestscore > lower)
                        lower = bestscore;
                }
            }
        }
    }
    
    //interrupt search
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(hash_code, board, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    
    
    return bestscore;
}





int RXEngine::EG_PVS_deep(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int selectivity, const int alpha, const int beta, const bool passed) {
    

    if (sBoard.board.n_empty < EG_DEEP_TO_MEDIUM)
        return EG_PVS_ETC_mobility(threadID, sBoard.board, pv, alpha, beta, passed);
    
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    //time gestion
    if(threadID == 0 && dependent_time && get_current_dependentTime() > time_limit()) {
        abort.store(true);
        return INTERRUPT_SEARCH;
    }

    
    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
    
    RXBitBoard& board = sBoard.board;
    
    //synchronized acces
    RXHashValue entry;
    const unsigned long long hash_code = board.hashcode();
    if(hTable->get(hash_code, board, type_hashtable, entry)) {
        
        if(!pv && entry.selectivity >= selectivity && entry.depth >= board.n_empty) {
            
            if (entry.lower > lower) {
                lower = entry.lower;
                if (lower >= upper)
                    return lower;
            }
            if (entry.upper < upper) {
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            
        }

        bestmove = entry.move;

    }
    
#ifdef USE_STABILITY
    
    if(!passed && bestmove == NOMOVE) {
        
        /*
         calculated stability is less than or equal to the real stability
         stability_bound is overestimated.
         score_max <= Stability_bound <= alpha ==> cutoff
         &
         score_max<=stability_bound < beta  ==> adjustment search window
         */
        if (upper >= stability_threshold[board.n_empty] ) {
            
            int stability_bound = 64- 2 * board.get_stability(board.player^1);
            if ( stability_bound <= lower )
                return stability_bound;
            
            if ( stability_bound < upper )
                upper = stability_bound;
            
        }
        
    }
    
#endif
    
    //    IID & IIS
    if(pv && bestmove == NOMOVE) {
        
        if(selectivity > EG_HIGH_SELECT)
            EG_PVS_deep(threadID, sBoard, pv, selectivity-1, -MAX_SCORE, MAX_SCORE, passed); //lower, upper,
        else
            MG_PVS_deep(threadID, sBoard, pv, MG_SELECT, board.n_empty-(USE_PV_EXTENSION? 10 : 8), -MAX_SCORE, MAX_SCORE, passed); //lower, upper,
        
        if(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))
            return INTERRUPT_SEARCH;
        
        if(hTable->get(hash_code, board, type_hashtable, entry)) {
                bestmove = entry.move;
        }

    }
    
    
    int score, bestscore = UNDEF_SCORE;
    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;
    
    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;
        
        const unsigned long long current_P = board.discs[board.player];
        const unsigned long long current_O = board.discs[board.player^1];
        
        const int etc_depth = board.n_empty-1;
        
        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);
            ++board.n_nodes;
            
            const unsigned long long next_P = current_O ^ move->flipped;
            const unsigned long long next_O = current_P | (move->flipped | move->square);

            
#ifdef USE_ENHANCED_STABLILITY
            
            if (lower <= -stability_threshold[etc_depth]) {
                int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                if ( stability_bound >= upper ) {
                    return stability_bound;
                }
//                if (stability_bound > lower) {
//                    lower = stability_bound;
//                }
            }
#endif
            
#ifdef USE_ETC
            if(!pv) {
                const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);
                
                //synchronized acces
                if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth>=etc_depth) {
                    
                    if(-entry.upper >= upper) {
//                        hTable->update(hash_code, board, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, -entry.upper, bestmove);
                        return -entry.upper ;
                    }
                }
            }
#endif
            
            previous = previous->next = move++;
            
        }
        
        
        //for other move
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove !=NOMOVE)
            legal_movesBB ^= 0x1ULL<<bestmove;
        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;

            if(legal_movesBB & bit){
                legal_movesBB ^= bit;
                
                board.generate_flips(pos, *move);
                ++board.n_nodes;
                
                const unsigned long long next_P = current_O ^ move->flipped;
                const unsigned long long next_O = current_P | (move->flipped | move->square);

#ifdef USE_ENHANCED_STABLILITY
                
                if (lower <= -stability_threshold[etc_depth]) {
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound >= upper ) {
                        return stability_bound;
                    }
//                    if (stability_bound > lower) {
//                        lower = stability_bound;
//                    }
                }
#endif
                
                move->score = 0;    //not in hash
                
#ifdef USE_ETC
                const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);
                
                //synchronized acces
                if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.depth>=etc_depth) {
                    
                    move->score = -3;    //in hash
                    
                    if ( -entry.upper >= upper) {
                        if(!pv && entry.selectivity >= selectivity) {
//                            hTable->update(hash_code, board, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, -entry.upper, pos);
                            return -entry.upper;
                        }
                        move->score = -16;
                    }
                }
#endif
                
                previous = previous->next = move++;
                
            }
        }
        
        
        
        previous->next = nullptr;
        
        
    }
    
    
    if (list->next == nullptr) {
        if (passed) {
            return board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_PVS_deep(threadID, sBoard, pv, selectivity, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
                        
        }
    } else {
        
        
        if(bestmove != NOMOVE) {
            
            /* first move */
            list = list->next;
            
            sBoard.patterns_update(*list);
            
            sBoard.do_move(*list);
            bestscore = -EG_PVS_deep(threadID, sBoard, pv, selectivity , -upper, -lower, false);
            sBoard.undo_move(*list);
                

            //bestmove = list->position;
            if (bestscore > lower)
                lower = bestscore;
                        
        }
        
        if(lower < upper && list->next != nullptr) {
            
            if((list->next)->next != nullptr) {
                // Evaluate moves for a future sort
                
                const int p = board.player;
                const int o = p^1;

                if(board.n_empty > EG_DEEP_TO_MEDIUM && lower >= -48) {

                    
                    int lower_probcut = -MAX_SCORE;
                    int upper_probcut =  MAX_SCORE;
                    probcut_bounds(board, std::max(EG_HIGH_SELECT, std::min(selectivity-1, NO_SELECT-2)), board.n_empty, (8+(board.n_empty & 1)), 0, lower, upper, lower_probcut, upper_probcut);

                    for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                        
                        sBoard.patterns_update(*iter);
                        
                        sBoard.do_move(*iter);
                        
                        int eval_move;
                        if((board.n_empty & 1) == 0) {
                            if(board.n_empty >= 30) {
                                eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_6, -MAX_SCORE, -lower_probcut, false);
                            } else if(board.n_empty >= 26) {
                                eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_4, -MAX_SCORE, -lower_probcut, false);
                            } else {
                                eval_move = -alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -MAX_SCORE, -lower_probcut, false);
                            }
                        } else {
                            if(board.n_empty >= 29) {
                                eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_5, -MAX_SCORE, -lower_probcut, false);
                            } else if(board.n_empty >= 25) {
                                eval_move = -alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, true, -MAX_SCORE, -lower_probcut, false);
                            } else {
                                
                                int bestscore_1 = UNDEF_SCORE; //masquage
                                
                                unsigned long long legal_movesBB = RXBitBoard::get_legal_moves(board.discs[o], board.discs[p]);
                                if(legal_movesBB) {
                                    RXMove& lastMove = threads[threadID]._move[board.n_empty][1];
                                    RXSquareList* empties = board.empties_list;
                                    do {
                                        empties = empties->next;
                                        
                                        const int pos = empties->position;
                                        const unsigned long long bit = 0x1ULL << pos;
                                        
                                        if (legal_movesBB & bit){
                                            legal_movesBB ^= bit;
                                            
                                            board.generate_flips(pos, lastMove);
                                            sBoard.patterns_update(lastMove);
                                            ++board.n_nodes;
                                            
                                            int score_1 = -sBoard.get_score<WITHOUT_FM>(lastMove);
                                            if (score_1>bestscore_1)
                                                bestscore_1 = score_1;
                                            
                                            
                                        }
                                    } while(bestscore_1 < -lower_probcut && legal_movesBB);
                                    
                                    
                                } else {
                                    //PASS
                                    sBoard.board.do_pass();
                                    bestscore_1 = -sBoard.get_score<WITHOUT_FM>();
                                    sBoard.board.do_pass();
                                    
                                }
                                
                                eval_move = -bestscore_1;
                                
                            }
                            
                        }
                        
                        
                        
                        if(eval_move > upper_probcut) {
                            
                            eval_move /= 2;
                            eval_move += 12;
                            
                        } else if(lower_probcut > eval_move ) {
                            
                            eval_move /= 2;
                            eval_move -= 12;
                            
                        }
                    
                    int mobility = RXBitBoard::get_mobility(board.discs[o], board.discs[p]);
                    int edge_stability = RXBitBoard::get_edge_stability(board.discs[p], board.discs[o]);
                    if(board.n_empty < 27 && 22 < board.n_empty) {
                        mobility = 4*mobility/3 - edge_stability/6;
                    } else if(board.n_empty <= 22) {
                        mobility = 3*mobility/2 - edge_stability/4;
                    }
                    
                    iter->score += mobility - eval_move;
                    
                    sBoard.undo_move(*iter);
                }
                                        
                } else {
                    
                    //empties<=EG_DEEP_TO_MEDIUM;
                    const unsigned long long current_P = board.discs[p];
                    const unsigned long long current_O = board.discs[o];

                    for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                        sBoard.patterns_update(*iter);
                        
                        ++board.n_nodes;
                        const unsigned long long next_O = current_P | (iter->flipped | iter->square);
                        const unsigned long long next_P = current_O ^ iter->flipped;

                        iter->score +=
                          sBoard.get_score<WITHOUT_FM>(*iter)
                        + 6*(RXBitBoard::get_mobility(next_P, next_O))
                        - RXBitBoard::get_edge_stability(next_O, next_P);
                    }
                    
                }
                
                
            } else {
                sBoard.patterns_update(*(list->next));
            }
            
            
            
            if(bestmove == NOMOVE) {
                
                RXMove* move = list->pick_next_promising_move();

                
                sBoard.do_move(*move);
                bestscore = -EG_PVS_deep(threadID, sBoard, pv, selectivity, -upper, -lower, false);
                sBoard.undo_move(*move);
                
                bestmove = move->position;
                if (bestscore > lower)
                    lower = bestscore;
                
                
                // next move
                list = list->next;
                
            }
            
            
            /* other moves : try to refute the first/best one */
            for(;lower < upper && list->next != nullptr; list = list->next) {
                
                RXMove* move = list->next;
                if (move->next != nullptr) {
                    
                    // Split?
                    if(activeThreads > 1 && board.n_empty > 19 && !abort.load(std::memory_order_relaxed) && !thread_should_stop(threadID) && idle_thread_exists(threadID)
                       && split(sBoard, pv, 1, board.n_empty, selectivity, lower, upper, bestscore, bestmove, list, threadID, RXSplitPoint::END_PVS)) {
                        
                        break;
                    }
                     
                    move = list->pick_next_promising_move();

                }
                
                
                sBoard.do_move(*move);
                if(selectivity != NO_SELECT) {
                    
                    score = -EG_NWS_XEndCut(threadID, sBoard, 1, selectivity, -lower-1, false); //pvDev == 1
                    if (lower < score && score < upper)
                        score = -EG_PVS_deep(threadID, sBoard, pv, selectivity, -upper, -lower, false); // (score == lower+1) ? -lower : -score
                    
                } else {
                    
                    score = -EG_PVS_deep(threadID, sBoard, false, NO_SELECT, -lower-1, -lower, false);
                    if (lower < score && score < upper)
                        score = -EG_PVS_deep(threadID, sBoard, pv, NO_SELECT, -upper, -score, false);
                    
                }
                sBoard.undo_move(*move);
                
                if (score > bestscore) {
                    bestscore = score;
                    bestmove = move->position;
                    if (bestscore > lower)
                        lower = bestscore;
                }
                                
            }
            
        }
    }
    
    //interrupt search
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(   hash_code, board, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    hTable_PV->update(hash_code, board, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
            
    return bestscore;
}


// EG_SP_search_DEEP() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_PVS_deep() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in EG_SP_search_DEEP().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::EG_SP_search_DEEP(RXSplitPoint* sp, const unsigned int threadID) {
    
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    
    
    //here sp->beta is const
    while(sp->alpha.load(std::memory_order_relaxed)< sp->beta && !abort.load(std::memory_order_relaxed) && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if (sp->list->next == nullptr) break;  // unlock auto via destructeur, même sur le break
            move = sp->list->next;
            sp->list = move;
        }
        
        int score;
        const int alpha = sp->alpha.load(std::memory_order_relaxed); //local copy
        
        
        sBoard.do_move(*move);
        
        if(sp->selectivity != NO_SELECT) {
            
            score = -EG_NWS_XEndCut(threadID, sBoard, sp->pvDev, sp->selectivity, -alpha-1, false);
            
            if (alpha < score && score < sp->beta)
                score = -EG_PVS_deep(threadID, sBoard, sp->pv, sp->selectivity, -sp->beta, -sp->alpha.load(std::memory_order_relaxed), false);
            
        } else {
            
            score = -EG_PVS_deep(threadID, sBoard, false, NO_SELECT, -alpha-1, -alpha, false);
            
            if (alpha < score && score < sp->beta)
                score = -EG_PVS_deep(threadID, sBoard, sp->pv, NO_SELECT, -sp->beta, -score, false);
            
        }
        
        sBoard.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore.load(std::memory_order_relaxed)) {
            
            //update
            std::lock_guard<std::mutex> lk(sp->lock);

            if(!sp->explored.load(std::memory_order_relaxed)) {
                
                
                // New best move?
                if(score > sp->bestscore.load(std::memory_order_relaxed)) {
                    sp->bestscore.store(score, std::memory_order_relaxed);
                    sp->bestmove = move->position;
                    if(score > sp->alpha.load(std::memory_order_relaxed)) {
                        
                        if(score >= sp->beta) {
                            sp->explored.store(true, std::memory_order_release);
                        } else {
                            sp->alpha.store(score, std::memory_order_relaxed);
                        }
                    }
                }
            }
            
        }
        
    }
    
    std::lock_guard<std::mutex> lk(sp->lock);

    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
}


/*
 NULL Window Search  + XProbCut
 Keep sorting (more efficient) before switching to EG_PVS_ETC_mobility method
 */
int RXEngine::EG_NWS_XEndCut(const unsigned int threadID, RXBBPatterns& sBoard, const int pvDev, const int selectivity, const int alpha, const bool passed) {
    

    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    //time gestion
    if(threadID == 0 && dependent_time && get_current_dependentTime() > time_limit()) {
        abort.store(true);
        return INTERRUPT_SEARCH;
    }


    RXBitBoard& board = sBoard.board;
    
    unsigned int bestmove = NOMOVE;
    
    
    //synchronized acces
    RXHashValue entry;
    const unsigned long long hash_code = board.hashcode();
    if(hTable->get(hash_code, board, type_hashtable, entry)) {
        
        if(entry.selectivity >= selectivity && entry.depth>=board.n_empty) {
            
            if(entry.lower > alpha)
                 return entry.lower;
            
            if(entry.upper <= alpha)
                return  entry.upper;
            
            bestmove = entry.move;

        }

        
    }
    
#ifdef USE_STABILITY
    if(!passed && bestmove == NOMOVE) {
        if ( alpha+1 >= stability_threshold[board.n_empty] ) {
            
            int stability_bound = 64- 2 * board.get_stability(board.player^1);
            if ( stability_bound <= alpha )
                return stability_bound;
        }
    }
#endif
    
/* *******************************************************************************
    Disable Not efficient : zero cutoff on Fforum-01-79
   ******************************************************************************* */
//    //param mpc
//    int lower_probcut, upper_probcut;
//    int depth_probcut = (board.n_empty/4)*2 + (board.n_empty & 0x1UL);
//    probcut_bounds(board, selectivity, board.n_empty, depth_probcut, pvDev, alpha, alpha+1, lower_probcut, upper_probcut);
//
//    if(bestmove != NOMOVE && entry.selectivity >= selectivity && entry.depth>=depth_probcut) {
//        
//        if(entry.lower >= upper_probcut) {
//            return alpha+2 -(alpha & 1); //05/05/2026
//        }
//#ifdef USE_PROBCUT_ALPHA
//        if(entry.upper <= lower_probcut) {
//            alpha -(alpha & 1);
//        }
//#endif
//        
//    }

    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;

    int bestscore = UNDEF_SCORE;
    
    const bool use_endcut  = board.n_empty >= MIN_DEPTH_USE_ENDCUT;
    const int  hash_select = use_endcut ? selectivity : NO_SELECT;
    const int  hash_depth  = board.n_empty + (use_endcut ? 0 : DEPTH_BOOSTER);


    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;

        const unsigned long long current_P = board.discs[board.player];
        const unsigned long long current_O = board.discs[board.player^1];
        
        const int etc_depth = board.n_empty-1;

#ifdef USE_ETC
        unsigned long long  next_hashcode;
#endif

        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);
            ++board.n_nodes;
            
            const unsigned long long next_P = current_O ^ move->flipped;
            const unsigned long long next_O = current_P | (move->flipped | move->square);

#ifdef USE_ENHANCED_STABLILITY
            if (alpha <= -stability_threshold[etc_depth]) {
                int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                if ( stability_bound > alpha ) {
//                    hTable->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, stability_bound, bestmove);
                    return stability_bound; // alpha
                }
            }
#endif
            
            //synchronized acces
#ifdef USE_ETC
            next_hashcode = RXBitBoard::hashcode(next_P, next_O);
            
            if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth >= (etc_depth)) {
                
                if(-entry.upper > alpha) {
//                    hTable->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, -entry.upper, bestmove);
                    return -entry.upper;
                }

            }

#endif
            
                previous = previous->next = move++;
            
        }
        
        //for all empty square
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove != NOMOVE) {
            legal_movesBB ^= 0x1ULL<<bestmove;
        }

        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;
            
            if(legal_movesBB & bit) {
                legal_movesBB ^= bit;
                
                board.generate_flips(pos, *move);
                ++board.n_nodes;
                
                const unsigned long long next_P = current_O ^ move->flipped;
                const unsigned long long next_O = current_P | (move->flipped | move->square);

                
#ifdef USE_ENHANCED_STABLILITY
                if (alpha <= -stability_threshold[etc_depth]) {
                    int stability_bound = 2 * board.get_stability(next_O, next_P) - 64;
                    if ( stability_bound > alpha ) {
//                        hTable->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, stability_bound, pos);
                        return stability_bound; // alpha
                    }
                }
#endif
                
                move->score = 0;
                
                //synchronized acces
#ifdef USE_ETC
                
                next_hashcode = RXBitBoard::hashcode(next_P, next_O);

                if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.depth>=etc_depth) {
                    
                    move->score = -3;
                    
                    if(-entry.upper > alpha) {
                        
                        if(entry.selectivity >= selectivity ) {
//                            hTable->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, -entry.upper, pos);
                            return -entry.upper;
                        }
                        
                        move->score = -16; //Bonus
                        
                    }
                    
                    // add malus?
                    else if(-entry.lower <= alpha) {
                        move->score = 2; // Malus
                        if(entry.selectivity >= selectivity)
                            move->score = 4; // super Malus

                    }

                    
                }
#endif
                
                previous = previous->next = move++;
                
            }
        }
        
        previous->next = nullptr;
        
    }
    
    
    if(list->next == nullptr) {
        //PASS
        if(passed) {
            return board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_NWS_XEndCut(threadID, sBoard, pvDev, selectivity, -alpha-1, true);
            board.do_pass();
            bestmove = PASS;
            
        }
        
    } else {

        //param mpc
        int lower_probcut, upper_probcut;
        const int depth_probcut = (board.n_empty/4)*2 + (board.n_empty & 0x1UL);
        probcut_bounds(board, selectivity, board.n_empty, depth_probcut, pvDev, alpha, alpha+1, lower_probcut, upper_probcut);

        //XProbcut
        int type_probcut = probcut(threadID, sBoard, selectivity, alpha, board.n_empty, depth_probcut, lower_probcut, upper_probcut, list, bestmove != NOMOVE);
        if( type_probcut == BETA_CUT) {
            return alpha+2 -(alpha & 1); //5/05/2026
        }
#ifdef USE_PROBCUT_ALPHA
        if(type_probcut == ALPHA_CUT) {
            return alpha -(alpha & 1);
        }
#endif
        
        //interrupt search
        if(type_probcut == INTERRUPT_SEARCH)
            return INTERRUPT_SEARCH;

        //fisrt move
        RXMove* move = list->next;
        
        if(board.n_empty<MIN_DEPTH_USE_ENDCUT) {
            board.do_move(*move);
            bestscore = -EG_PVS_ETC_mobility(threadID, board, false, -alpha-1, -alpha, false);
            board.undo_move(*move);
        } else {
            sBoard.do_move(*move);
            bestscore = -EG_NWS_XEndCut(threadID, sBoard, pvDev, selectivity, -alpha-1, false);
            sBoard.undo_move(*move);
        }
        
        bestmove = move->position;
        list = list->next;
        
        
        
        int score;
        for(RXMove* iter = list->next; bestscore<=alpha && iter != nullptr; iter = iter->next, list = list->next) {
            
            if(activeThreads > 1 && board.n_empty>=MIN_DEPTH_USE_ENDCUT && iter->next != nullptr
               && !abort.load(std::memory_order_relaxed) && !thread_should_stop(threadID) &&  idle_thread_exists(threadID)
               && split(sBoard, false, pvDev+1, board.n_empty, selectivity,
                        alpha, (alpha + 1), bestscore, bestmove, list, threadID, RXSplitPoint::END_XPROBCUT)) {
             
                break;
            }
            
            
            if(board.n_empty < MIN_DEPTH_USE_ENDCUT) {
                board.do_move(*iter);
                score = -EG_PVS_ETC_mobility(threadID, board, false, -alpha-1, -alpha, false);
                board.undo_move(*iter);
            } else {
                sBoard.do_move(*iter);
                score = -EG_NWS_XEndCut(threadID, sBoard, pvDev+1, selectivity, -alpha-1, false);
                sBoard.undo_move(*iter);
            }
             
            if (score>bestscore) {
                bestscore = score;
                bestmove = iter->position;
            }
                        
        }
    }
    
    
    //interrupt search
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, bestscore, bestmove);
    if(pvDev < 4)
        hTable_PV->update(hash_code, board, type_hashtable, hash_select, hash_depth, alpha, bestscore, bestmove);
    
    return bestscore;
    
    
}

// EG_SP_search_XEndcut() is used to search from a split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_NWS_XEndCut() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in EG_SP_search_XEndcut().  We also don't
// need to store anything to the hash table here: This is taken care of
// after we return from the split point.

void RXEngine::EG_SP_search_XEndcut(RXSplitPoint* sp, const unsigned int threadID) {
    
    //    assert(threadID >= 0 && threadID < activeThreads);
    //    assert(activeThreads > 1);
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    
    //here sp->alpha is const
    while(sp->bestscore.load(std::memory_order_relaxed) <= sp->alpha.load(std::memory_order_relaxed) && !abort.load(std::memory_order_relaxed)  && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if (sp->list->next == nullptr) break;  // unlock auto via destructeur, même sur le break
            move = sp->list->next;
            sp->list = move;
        }
        
        const int alpha = sp->alpha.load(std::memory_order_relaxed); //local copy
        
        sBoard.do_move(*move);
        int score = -EG_NWS_XEndCut(threadID, sBoard, sp->pvDev, sp->selectivity, -alpha-1, false);
        sBoard.undo_move(*move);
        
        
        //first without mutex
        if(score > sp->bestscore.load(std::memory_order_relaxed)) {
            
            std::lock_guard<std::mutex> lk(sp->lock);

            //update SplitPoint
            if(!sp->explored.load(std::memory_order_relaxed)) {
                
                
                // New best move?
                if(score > sp->bestscore.load(std::memory_order_relaxed)) {
                    sp->bestscore.store(score, std::memory_order_relaxed);
                    sp->bestmove = move->position;
                    if(score > sp->alpha.load(std::memory_order_relaxed)) {
                        sp->explored.store(true, std::memory_order_release);
                    }
                }
            }
            
            
        }
    }
    
    std::lock_guard<std::mutex> lk(sp->lock);

    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
}

/*!
 * \brief Principal Variation Search algorithm at the root of the tree.
 *
 * This function solves the position provided within the limits set by the alpha
 * and beta bounds. The list parameter is updated so that the bestmove is the
 * first of the list when the search ended.
 *
 * \param sBoard      	sBoard.
 * \param selectivity 	selectivity.
 * \param alpha      	lower bound.
 * \param beta       	upper bound.
 * \param list   		List of legal moves (should actually contain moves !).
 */
void RXEngine::EG_PVS_root(RXBBPatterns& sBoard, const int selectivity, const int alpha, const int beta, RXMove* list)
{
    
    //	assert(alpha>=-64 && beta<=64);
    
    
    /*
     *log << "[" << alpha << " ; " << beta << "]" << std::endl;
     */
    
    //*log << "                  [EndGame @" << CONFIDENCE[selectivity] << " alpha : " << alpha << " beta : " << beta << "]" << std::endl;
    
    RXMove* iter = list->next;
    
    unsigned int bestmove = iter->position;
    
    RXBitBoard& board = sBoard.board;
    
    int lower = alpha;
    const int upper = beta;
    
    
    /* first move */
    first_move.store(true);
        
    sBoard.do_move(*iter);
            
    int bestscore;
    if (board.n_empty == 0) {
        bestscore = -board.final_score();
    } else if (board.n_empty == 1) {
        bestscore = -board.final_score_1();
    } else if (board.n_empty == 2) {
        bestscore = -board.final_score_2(-upper, -lower);
    } else if (board.n_empty == 3) {
        bestscore = -board.final_score_3(-upper, -lower);
    } else if (board.n_empty == 4) {
        bestscore = -board.final_score_4(-upper, -lower);
    } else if (board.n_empty < EG_MEDIUM_TO_SHALLOW) {
        bestscore = -EG_alphabeta_parity(0, board, -upper, -lower, false);
    } else if (board.n_empty < EG_MEDIUM_HI_TO_LOW) {
        bestscore = -EG_PVS_ETC_LTT(0, board, true, -upper, -lower, false);
    } else  if (board.n_empty < EG_DEEP_TO_MEDIUM) {
        bestscore = -EG_PVS_ETC_mobility(0, board, true, -upper, -lower, false);
    } else {
        bestscore = -EG_PVS_deep(0, sBoard, true, selectivity, -upper, -lower, false);
    }

    sBoard.undo_move(*iter);
    
    if(!abort.load(std::memory_order_relaxed) && std::abs(bestscore) != INTERRUPT_SEARCH) {
        
        if(search_client == RXSearch::kGGSMode) {	// GGS mode
            
            if(dependent_time && board.n_empty>19)
                manager->sendMsg(showBestmove(board.n_empty, selectivity, lower, upper, bestscore, bestmove));
        }
        
        if (bestscore > lower)
            lower = bestscore;
        else {
            extra_time = 1;
            //            *log << "                  [extra time <=:" << extra_time << "]" << std::endl;
        }
        
        /* other moves : try to refute the first/best one */
        int score;
        first_move.store(false);
        
        for (iter = iter->next; !abort.load(std::memory_order_relaxed)  && lower < upper && iter != nullptr; iter = iter->next) {
            
#ifdef USE_SPLIT_AT_ROOT
            
            if(activeThreads > 1 && iter->next != nullptr && board.n_empty >= EG_DEEP_TO_MEDIUM
               && !abort.load(std::memory_order_relaxed) && idle_thread_exists(0) && !thread_should_stop(0)
               && split(sBoard, true, 0, board.n_empty, selectivity,
                        lower, upper, bestscore, bestmove, iter, 0, RXSplitPoint::END_ROOT)) {
                
                break;
            }
#endif
            
            sBoard.do_move(*iter);

            if (board.n_empty == 1) {
                score = -board.final_score_1();
            } else if (board.n_empty == 2) {
                score = -board.final_score_2(-upper, -lower);
            } else if (board.n_empty == 3) {
                score = -board.final_score_3(-upper, -lower);
            } else if (board.n_empty == 4) {
                score = -board.final_score_4(-upper, -lower);
            } else {
                
                //simple_pv PV == false
                if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
                    score = -EG_alphabeta_parity(0, board, -lower-1, -lower, false);
                else if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
                    score = -EG_PVS_ETC_LTT(0, board, false, -lower-1, -lower, false);
                else if (board.n_empty < EG_DEEP_TO_MEDIUM)
                    score = -EG_PVS_ETC_mobility(0, board, false, -lower-1, -lower, false); //simple-PV pv == false ????
                else
                    score = -EG_PVS_deep(0, sBoard, false, selectivity, -lower-1, -lower, false); //simple-PV pv == false ????
                
                
                if (!abort.load(std::memory_order_relaxed) && board.n_empty > 4 && lower < score && score < upper) {
                    
                    if(search_client == RXSearch::kGGSMode) {	// GGS mode
                        
                        if(dependent_time && board.n_empty>19)
                            manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " maybe better? ");
                        
                    }
                    
                    if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
                        score = -EG_alphabeta_parity(0, board, -upper, -score, false);
                    else if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
                        score = -EG_PVS_ETC_LTT(0, board, true, -upper, -score, false);
                    else if (board.n_empty < EG_DEEP_TO_MEDIUM)
                        score = -EG_PVS_ETC_mobility(0, board, true, -upper, -score, false);
                    else {
                        ++extra_time;
                        //                        *log << "                  [extra time > :" << extra_time << "]" << std::endl;
                        
                        score = -EG_PVS_deep(0, sBoard, true, selectivity, -upper, -score, false);
                        
                        if(search_client == RXSearch::kGGSMode && !abort.load(std::memory_order_relaxed)) {    // GGS mode
                            if(dependent_time && board.n_empty>19 && score <= bestscore)
                                manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " refuted ");
                        }
                        
                        
                        --extra_time;
                        //                        *log << "                  [extra time end :" << extra_time << "]" << std::endl;
                        
                    }
                    
                    
                }
                
                
                
            }
            
            

            sBoard.undo_move(*iter);
            
            
            if (!abort.load(std::memory_order_relaxed)  && std::abs(score) != INTERRUPT_SEARCH) {
                
                if(score > bestscore) {
                    
                    bestmove = iter->position;
                    bestscore = score;
                    
                    if(search_client == RXSearch::kGGSMode)    // GGS mode
                        if(dependent_time && board.n_empty>19)
                            manager->sendMsg(showBestmove(board.n_empty, selectivity, lower, upper, bestscore, bestmove));
                    
                    if (bestscore > lower) {
                        lower = bestscore;
                        extra_time = 0;
                    }
                    
                    
                }
            }
            
        }
        
        extra_time = 0;
                
        list->sort_bestmove(bestmove);
        list->next->score = bestscore;
        
        //        *log << "                  [score " << bestscore << " ]" << std::endl;
        
        hTable->update(board.hashcode(), board, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
        
        
    }
    
}

// EG_SP_search_root() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_PVS_root() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in EG_SP_search_DEEP().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::EG_SP_search_root(RXSplitPoint* sp, const unsigned int threadID) {
    
    //    assert(threadID >= 0 && threadID < activeThreads);
    //    assert(activeThreads > 1);
    
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    
    //here sp->beta is const
    while(sp->alpha.load(std::memory_order_relaxed)< sp->beta && !abort.load(std::memory_order_relaxed) && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if(sp->list == nullptr) break;
            move = sp->list;
            sp->list = move->next;
        }

        const int alpha = sp->alpha.load(std::memory_order_relaxed); //local copy
        
        sBoard.do_move(*move);
        
        
        int score = -EG_PVS_deep(threadID, sBoard, false, sp->selectivity, -alpha-1, -alpha, false);
        
        if (!(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID)) && alpha < score && score < sp->beta) {
            
            ++extra_time;
            
            if(dependent_time && board.n_empty>19)
                manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " maybe better? ");
            
            if(sp->selectivity != NO_SELECT)
                score = -EG_PVS_deep(threadID, sBoard, true, sp->selectivity, -sp->beta, -sp->alpha.load(std::memory_order_relaxed), false);
            else
                score = -EG_PVS_deep(threadID, sBoard, true, sp->selectivity, -sp->beta, -score, false);
            
            if(search_client == RXSearch::kGGSMode && !(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))) {    // GGS mode
                if(dependent_time && board.n_empty>19 && score <= sp->bestscore.load(std::memory_order_relaxed))
                    manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " refuted ");
            }
            
            
            --extra_time; //atomic = thread-safe
            
        }
        
        sBoard.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore.load(std::memory_order_relaxed)) {
            //update
            std::lock_guard<std::mutex> lk(sp->lock);

            if(!sp->explored.load(std::memory_order_relaxed)) {
                
                // New best move?
                if(score > sp->bestscore.load(std::memory_order_relaxed)) {
                    sp->bestscore.store(score, std::memory_order_relaxed);
                    sp->bestmove = move->position;
                    
                    if(dependent_time && board.n_empty>19)
                        manager->sendMsg(showBestmove(board.n_empty, sp->selectivity, sp->alpha.load(std::memory_order_relaxed), sp->beta, sp->bestscore.load(std::memory_order_relaxed), sp->bestmove));
                    
                    if(score > sp->alpha.load(std::memory_order_relaxed)) {
                        
                        if(score >= sp->beta) {
                            sp->explored.store(true, std::memory_order_release);
                        } else {
                            sp->alpha.store(score, std::memory_order_relaxed);
                        }
                        
                    }
                }
            }
            
        }
        
    }
    
    std::lock_guard<std::mutex> lk(sp->lock);

    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
}

/*!
 * \brief Driver for solver.
 *
 * This function reduced and moved the search window
 *
 * \param sBoard      	sBoard.
 * \param selectivity 	selectivity.
 * \param list   		List of legal moves (should actually contain moves !).
 */

void RXEngine::EG_driver(RXBBPatterns& sBoard, int selectivity, int end_selectivity, RXMove* list) {
        
    set_type_search(ENDGAME);
    
    //utiliser directement "list->next->score" provoque un bug d'affichage 25/03/2025
    int score = list->next->score;
    score -= score % 2;
    
    score = std::min(64, std::max(-64, score));
    
    
    int s_alpha = search_alpha;
    if(s_alpha<0) {
        s_alpha += s_alpha%2;
    } else {
        s_alpha -= s_alpha%2;
    }
    
    int s_beta  = search_beta;
    if(s_beta<0) {
        s_beta  -= s_beta%2;
    } else {
        s_beta  += s_beta%2;
    }
    
    s_alpha = std::max(-64, s_alpha);
    s_beta  = std::min( 64, s_beta);
    
#ifdef SOLVER_DRIVER
    for(selectivity = 2; !abort.load(std::memory_order_relaxed)  && selectivity <= end_selectivity; selectivity+=(selectivity == NO_SELECT? 1:(NO_SELECT-selectivity)))
#else
    for(; !abort.load(std::memory_order_relaxed)  && selectivity <= end_selectivity; selectivity++)
#endif
    {
        
        selectivity = std::max(std::min(NO_SELECT, std::max(EG_HIGH_SELECT, 28-sBoard.board.n_empty)), std::max(EG_HIGH_SELECT, selectivity));

        set_select_search(selectivity);
        
        if(dependent_time)
            determine_move_time(sBoard.board);
        
        new_search = false;
        
        int eTime_start_level = get_current_time();
        
        time_startLevel = eTime_start_level;
        
        
        sBoard.board.n_nodes = 0;
        
        
        int alpha = std::max(s_alpha, std::min(s_beta-1, score - 1));
        int beta  = std::min(s_beta, std::max(s_alpha+1, score + 1));
        
        //always even window
        if(alpha<0) {
            alpha += alpha%2 ;
        } else {
            alpha -= alpha%2;
        }
        
        if(beta<0) {
            beta  -= beta%2;
        } else {
            beta  += beta%2;
        }
        
        //high score >= 48 skip selectivity stage
        if (selectivity == EG_HIGH_SELECT && abs(alpha) > 48) {
            selectivity = 2;
        } else  if (selectivity != EG_HIGH_SELECT && abs(alpha) > 51) {
            ++selectivity;
            if (abs(alpha) > 59)
                selectivity = NO_SELECT;
            selectivity = std::min(selectivity, end_selectivity);
        }
        

        EG_PVS_root(sBoard, selectivity, alpha, beta, list);
        score = list->next->score;
        
        
        int left = 2;
        int right = 2;
        
        while (!abort.load(std::memory_order_relaxed) && !(alpha < score && score < beta)) {
            
            //std::cout << "                  EG research : [" << alpha << " < " << list->next->score << " < " << beta << "]" <<std::endl;
            
            if (score <= alpha) {
                
                if(alpha <= s_alpha)
                    break;
                
                left *=2;
                
                alpha = score - left;
                
            } else if (score >= beta) {
                
                if(beta >= s_beta)
                    break;
                
                right *=2;
                
                beta  = score + right;
            }
            
            
            alpha = std::max(s_alpha, std::min(s_beta-1, alpha));
            beta  = std::min(s_beta, std::max(s_alpha+1, beta ));
            
            //search window has always peers limits
            if(alpha<0) {
                alpha += alpha%2;
            } else {
                alpha -= alpha%2;
            }
            
            if(beta<0) {
                beta  -= beta%2;
            } else {
                beta  += beta%2;
            }
            

            EG_PVS_root(sBoard, selectivity, alpha, beta, list);
            score = list->next->score;

        };
        
        
        extra_time = 0;
        
        
        best_answer.position = list->next->position;
        best_answer.score = list->next->score;
        best_answer.depth = sBoard.board.n_empty;
        best_answer.selectivity = CONFIDENCE[selectivity];
        
        best_answer.nodes += sBoard.board.n_nodes;
        
        
        int eTime = get_current_time();
        
        
        //unsynchronized acces
        RXHashValue entry;
        if(hTable->get(sBoard.board, type_hashtable, entry) && entry.depth >= sBoard.board.n_empty && entry.selectivity>=selectivity) {
            int type = EXACT;
            
            score = list->next->score;
            if(entry.lower == entry.upper) {
                if(abort.load(std::memory_order_relaxed))
                    type = INTERRUPT;
            } else if(list->next->score == entry.lower || entry.upper ==  MAX_SCORE) {
                type = SUPERIOR;
                score = entry.lower;
            } else if(list->next->score == entry.upper || entry.lower == -MAX_SCORE){
                type = INFERIOR;
                score = entry.upper;
            }
            
            
            *log << display(sBoard.board, type, selectivity, score, eTime, eTime - eTime_start_level) << std::endl;
            
        }
        
        //check PV at 100%
#ifdef EG_CHECK_PV
        if(!abort.load(std::memory_order_relaxed) && sBoard.board.n_empty-6 > 0 && selectivity == NO_SELECT && s_alpha <= list->next->score && list->next->score <= s_beta) {
            RXSearch::t_client save_client = search_client;
            search_client = RXSearch::kPrivate;
            EG_check_PV(search_sBoard, list->next->score);
            search_client = save_client;
        }
#endif

        
        if(abort.load(std::memory_order_relaxed) )
            break;
        
        time_nextLevel = pTime_next_level(sBoard.board, eTime - eTime_start_level);
        
        if(probable_timeout(time_nextLevel)) {
            abort.store(true);
            *log << "                  EG Driver : likely timeout" << std::endl;
            
            break;
        }
        
    }
    
}
    
    
/*!
 * \brief Extension of research until the solution.
 *
 * This function prepare parameters for solver
 *
 * \param sBoard      	sBoard.
 */


void RXEngine::EG_check_PV(RXBBPatterns& sBoard, const int score) {
        
    //collect PV
    std::vector<unsigned char> pv;
    hTable->mainVariation(pv, sBoard.board, type_hashtable, sBoard.board.n_empty-6);
    
    
    //print PV
    std::ostringstream buffer;
    bool player = false;
    for(auto it : pv) {

        std::string coord = RXMove::index_to_coord(it);
        if(player)
            std::transform(coord.begin(), coord.end(), coord.begin(), ::tolower);
        buffer << coord << ' ';

        player = !player;
        
    }
    std:: cout << buffer.str() << std::endl;;

    // check PV
    if(!pv.empty())
       EG_check_PV(pv, sBoard, score);

}

bool RXEngine::EG_check_PV(std::vector<unsigned char>& pv, RXBBPatterns& sBoard, int score) {

    bool good_pv = true;
    RXBitBoard& board = sBoard.board;

    const int pos = pv.front();
    if(pos != NOMOVE) {
        
        RXMove& move = threads[0]._move[board.n_empty][1];
        if(pos == PASS) {
            board.do_pass();
        } else {
            board.generate_flips(pos, move);
            sBoard.patterns_update(move);
            sBoard.do_move(move);
        }
        
        
        pv.erase(pv.begin());
        
        if(!pv.empty() && pv.front() != NOMOVE) {
            
            if(pv.front() == PASS) {
                good_pv = EG_check_PV(pv, sBoard, -score);
            } else {
                RXMove* list = threads[0]._move[board.n_empty];
                board.moves_producing(list);
                list->sort_bestmove(pv.front());
                
                for(RXMove* iter = list->next; iter != nullptr; iter = iter->next)
                    sBoard.patterns_update(*iter);
 
                //version monogame [provoque bug affichage]
                hTable->reset();
                EG_PVS_root(sBoard, NO_SELECT, -64, 64, list);
                
                int real = -list->next->score;
                
                if(real == score)
                    good_pv = EG_check_PV(pv, sBoard, -score);
                else {
                    
                    std::cout << "RED ALERT : wrong PV" << std::endl;
                    std::cout << sBoard.board.string_rawdata() << std::endl;
                    std::cout << "at depth : " << sBoard.board.n_empty << std::endl;
                    std::cout << "bad move : " << RXMove::index_to_coord(pos) << std::endl;
                    std::cout << "score PV = " << score << " real = " << real << std::endl;

                    good_pv = false;
                }
                
            }
            
        }
        
        
        pv.insert(pv.begin(), pos);
        
        
        if(pos == PASS) {
            board.do_pass();
        } else {
            sBoard.undo_move(move);
        }
    }
    

    return good_pv;
    
}

