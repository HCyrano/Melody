/*
 *  RXEndGame.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 11/06/05.
 *  Copyleft 2005-2025 personnel.
 *
 */
#include <iostream>
#include <iomanip>


#include "RXEngine.hpp"
#include "RXRoxane.hpp"
#include "RXBitBoard.hpp"
#include "RXSetting.hpp"

const int RXEngine::stability_threshold[] =
{   65, 65, 65, 65,  6,  8, 10, 14,
    16, 18, 22, 24, 26, 28, 30, 32,
    34, 36, 38, 40, 40, 42, 42, 44,
    44, 46, 46, 48, 48, 50, 52, 52,
    54, 54, 56, 56, 58, 58, 60, 60,
    62, 62, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65,
    65, 65, 65, 65, 65, 65, 65, 65};

#ifdef SOLVER_SETTING
/* for record fforum-40-59 setting*/
const int RXEngine::EG_DEEP_TO_MEDIUM = 16;
const int RXEngine::EG_MEDIUM_HI_TO_LOW = 13;
const int RXEngine::EG_MEDIUM_TO_SHALLOW = 7;
const int RXEngine::MIN_DEPTH_USE_ENDCUT = 16;
#else
/* standart setting */
const int RXEngine::EG_DEEP_TO_MEDIUM = 17;
const int RXEngine::EG_MEDIUM_HI_TO_LOW = 14;
const int RXEngine::EG_MEDIUM_TO_SHALLOW = 8;
const int RXEngine::MIN_DEPTH_USE_ENDCUT = 16;
#endif



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
    if ( beta >= stability_threshold[board.n_empty]) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= alpha )
            return stability_bound;
        if ( stability_bound < beta )
            beta = stability_bound;
    }
#endif
    
    
    unsigned long long legal_movesBB = board.get_legal_moves();
    
    if(legal_movesBB) {
        
        RXMove& move = threads[threadID]._move[board.n_empty][1];
        
        if(board.n_empty == 5) {
            
            unsigned long long parity_movesBB = RXBitBoard::QUADRANT_MASK[board.parity];
            
            for (int parity = 1; parity >= 0; parity--) {
                unsigned long long movesBB = legal_movesBB & parity_movesBB;
                
                for(RXSquareList* empties = board.empties_list->next; movesBB; empties = empties->next) {
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if (movesBB & bit) {
                        movesBB ^= bit;
                        
                        ((board).*(board.generate_flips[pos]))(move);
                        
                        // board.do_move(move);
                        // optimized 5 empties
                        board.discs[board.player] |= (move.flipped | move.square);
                        board.player ^= 1;
                        board.discs[board.player] ^= move.flipped;
                        
                        parity ^= board.QUADRANT_ID[move.position];

                        empties->previous->next = empties->next;
                        
                        ++board.n_nodes;
                        
                        score = -board.final_score_4(-beta, -alpha, false);
 
                        // board.undo_move(move);
                        // optimized 5 empties
                        empties->previous->next = empties;
                        
                        parity ^= board.QUADRANT_ID[move.position];
                        
                        board.discs[board.player] |= move.flipped;
                        board.player ^= 1;
                        board.discs[board.player] ^= (move.flipped | move.square);

                        
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
            
            
        } else {
            
            if(board.parity == 0 || board.parity == 0xF) {
                
                RXSquareList* empties = board.empties_list;
                do {
                    empties = empties->next;
                    
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if (legal_movesBB & bit) {
                        legal_movesBB ^= bit;
                        
                        ((board).*(board.generate_flips[pos]))(move);
                        
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
                    
                    for(RXSquareList* empties = board.empties_list->next; movesBB; empties = empties->next) {
                        const int pos = empties->position;
                        const unsigned long long bit = 0x1ULL << pos;

                        if (movesBB & bit) {
                            movesBB ^= bit;
                            
                            ((board).*(board.generate_flips[pos]))(move);
                            
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


//7 empty
int RXEngine::EG_alphabeta_hash_mobility(const unsigned int threadID, RXBitBoard& board, const bool pv, int alpha, const int beta, const bool passed) {
    
    int bestscore = UNDEF_SCORE;
    int lower = alpha;
    int upper = beta;
    
    unsigned int bestmove = NOMOVE;
    
    const unsigned long long  hash_code = board.hashcode();
    
    
    hTable->entry_prefetch(hash_code, type_hashtable);
    
#ifdef USE_STABILITY
    
    if ( upper >= stability_threshold[board.n_empty] ) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= lower )
            return stability_bound;
        
        if ( stability_bound < upper )
            upper = stability_bound;
        
    }
#endif
    
    
    RXHashValue entry;
    if(!pv && hTable->get(hash_code, type_hashtable, entry)) {
        
        if(entry.selectivity == NO_SELECT && entry.depth >= board.n_empty) { //
            
            if (upper > entry.upper) {
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            if (lower < entry.lower) {
                lower = entry.lower;
                if (lower >= upper)
                    return lower;
            }
            
            //04/02/2025
            if(board.isValid_square(entry.move))
                bestmove = entry.move;

        }
    }
    
    
    if(bestmove != PASS) {
        
        RXMove* list = threads[threadID]._move[board.n_empty];
        RXMove* move = list + 1;
        
        
        if(bestmove != NOMOVE) {
            
            ((board).*(board.generate_flips[bestmove]))(*move);
            
            // first move
            board.do_move(*move);
            bestscore = -EG_alphabeta_parity(threadID, board, -upper, -lower, false);
            board.undo_move(*move);
            
            if (bestscore > lower)
                lower = bestscore;
            
        }
        
        if(lower < upper) {
            
            //for all empty square
            unsigned long long legal_movesBB = board.get_legal_moves();
            if(bestmove !=NOMOVE)
                legal_movesBB ^=  0x1ULL<<bestmove;
            
            if(legal_movesBB) {
                
                RXMove* previous = list;
                RXSquareList* empties = board.empties_list;
                do {
                    empties = empties->next;
                    
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if(legal_movesBB & bit) {
                        legal_movesBB ^= bit;
                        
                        ((board).*(board.generate_flips[pos]))(*move);
                        
                        previous = previous->next = move++;
                    }
                    
                } while(legal_movesBB);
                
                previous->next = nullptr;
                
                if((list->next)->next != nullptr) { //nb moves > 1
                    
                    const int p = board.player;
                    const int o = p^1;
                    
                    //sort list by mobility
                    for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
                        
                        ++board.n_nodes;
                        
                        const unsigned long long p_discs = board.discs[p] | (iter->flipped | iter->square);
                        const unsigned long long o_discs = board.discs[o] ^ iter->flipped;
                        
                        iter->score = (RXBitBoard::count_potential_moves(o_discs, p_discs)<<4)
                        - (RXBitBoard::get_corner_stability(p_discs)<<2)
                        - (((board.parity & RXBitBoard::QUADRANT_ID[iter->position])>>RXBitBoard::QUADRANT_SHIFT[iter->position])<<4);
                        
                    }
                    
                    
                }
                
                
                int score;
                do {
                    
                    move = list->next;
                    
                    if(move->next != nullptr)
                        move = list->pick_next_promising_move();
                    
                    
                    board.do_move(*move);
                    score = -EG_alphabeta_parity(threadID, board, -upper, -lower, false);
                    board.undo_move(*move);
                    
                    if (score > bestscore) {
                        bestscore = score;
                        bestmove  = move->position;
                        if (bestscore > lower)
                            lower = bestscore;
                    }
                    
                    list = list->next;
                    
                } while(lower < upper && list->next != nullptr);
            }
        }
    }
    
    
    //if PASS
    if (bestscore == UNDEF_SCORE) {
        if (passed) {
            return board.final_score();
         } else {
            board.do_pass();
            bestscore = -EG_alphabeta_hash_mobility(threadID, board, pv, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    }
    
    //en test 21/01/2025 suspision bug (bestscore >= upper mais stocker comme < beta)
    hTable->update(hash_code, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    
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
int RXEngine::EG_PVS_hash_mobility(const unsigned int threadID, RXBitBoard& board, const bool pv, int alpha, const int beta, const bool passed)
{
    

    if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
        return EG_alphabeta_hash_mobility(threadID, board, pv, alpha, beta, passed);

    
    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
    const unsigned long long  hash_code = board.hashcode();
    
    
    hTable->entry_prefetch(hash_code, type_hashtable);
    
#ifdef USE_STABILITY
    
    /*
     calculated stability is less than or equal to the real stability
     stability_bound is overestimated.
     score_max <= Stability_bound <= alpha ==> cutoff
     &
     score_max<=stability_bound < beta  ==> adjustment search window
     */
    
    
    if ( upper >= stability_threshold[board.n_empty] ) {
        
        int stability_bound = 64- 2 * board.get_stability(board.player^1);
        if ( stability_bound <= lower )
            return stability_bound;
        
        if ( stability_bound < upper )
            upper = stability_bound;
        
    }
    
#endif
    
    
    RXHashValue entry;
    if(!pv && hTable->get(hash_code, type_hashtable, entry)) {
        
        if(entry.selectivity == NO_SELECT && entry.depth >= board.n_empty) { //
            
            
            if (upper > entry.upper) {
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            if (lower < entry.lower) {
                lower = entry.lower;
                if (lower >= upper)
                    return lower;
            }
            
            //04/02/2025
            if(board.isValid_square(entry.move))
                bestmove = entry.move;

        }
    }
    
    int bestscore = UNDEF_SCORE;
    
    if(bestmove != PASS) {
        
        RXMove* list = threads[threadID]._move[board.n_empty];
        RXMove* move = list + 1;
        
        if(bestmove != NOMOVE) {
            
            ((board).*(board.generate_flips[bestmove]))(*move);
            
            /* first move */
            board.do_move(*move);
            bestscore = -EG_PVS_hash_mobility(threadID, board, pv, -upper, -lower, false);
            board.undo_move(*move);
            
            
            //bestmove = move->position;
            if (bestscore > lower)
                lower = bestscore;
            
        }
        
        if(lower < upper) {
            
            //for all empty square
            unsigned long long legal_movesBB = board.get_legal_moves();
            if(bestmove !=NOMOVE)
                legal_movesBB ^= 0x1ULL<<bestmove;
            
            if(legal_movesBB) {
                
                RXMove* previous = list;
                
                RXSquareList* empties = board.empties_list;
                do {
                    empties = empties->next;
                    
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if(legal_movesBB & bit) {
                        legal_movesBB ^= bit;
                        
                        ((board).*(board.generate_flips[empties->position]))(*move);
                        previous = previous->next = move++;
                    }
                } while(legal_movesBB);
                
                previous->next = nullptr;
                
                
                if((list->next)->next != nullptr) { //nb moves > 1
                    
                    const int p = board.player;
                    const int o = p^1;
                    
                    //sort list by mobility
                    for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
                        
                        ++board.n_nodes;
                        
                        const unsigned long long p_discs = board.discs[p] | (iter->flipped | iter->square);
                        const unsigned long long o_discs = board.discs[o] ^ iter->flipped;
                        
                        iter->score = (RXBitBoard::get_mobility(o_discs, p_discs)<<5)
                        - (RXBitBoard::get_corner_stability(p_discs)<<2)
                        + (RXBitBoard::count_potential_moves(o_discs, p_discs))
                        - ((board.parity & RXBitBoard::QUADRANT_ID[iter->position])>>RXBitBoard::QUADRANT_SHIFT[iter->position]);
                        
                    }
                    
                    
                }
                
                
                if(bestmove == NOMOVE) {
                    
                    move = list->pick_next_promising_move();
                    
                    board.do_move(*move);
                    bestscore = -EG_PVS_hash_mobility(threadID, board, pv, -upper, -lower, false);
                    board.undo_move(*move);
                    
                    bestmove = move->position;
                    if (bestscore > lower)
                        lower = bestscore;
                    
                    // next move
                    list = list->next;
                    
                }
                
                
                // other moves : try to refute the first/best one
                int score;
                for(; lower < upper && list->next != nullptr; list = list->next) {
                    
                    move = list->pick_next_promising_move();

                    board.do_move(*move);
                    
                    score = -EG_PVS_hash_mobility(threadID, board, false, -lower-1, -lower, false);
                    if (lower < score && score < upper)
                        score = -EG_PVS_hash_mobility(threadID, board, pv, -upper, -score, false);
                    
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
    }
    
    
    if (bestscore == UNDEF_SCORE) {
        if (passed) {
            return board.final_score();
        } else {
            board.do_pass();
            bestscore = -EG_PVS_hash_mobility(threadID, board, pv, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    }
    
    //en test 21/01/2025 suspision bug (bestscore >= upper mais stocker comme < beta)
    hTable->update(hash_code, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    
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
 * \param sBoard      sBoard : just for conformity with split method.
 * \param alpha      lower bound.
 * \param beta       upper bound.
 * \param passed     a flag indicating if previous move was a pass.
 * \return the final score, as a disc difference.
 */
int RXEngine::EG_PVS_ETC_mobility(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, int alpha, const int beta, const bool passed)
{
    
    if (sBoard.board.n_empty < EG_MEDIUM_HI_TO_LOW)
        return EG_PVS_hash_mobility(threadID, sBoard.board, pv, alpha, beta, passed);

     
    if(abort.load() || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    RXBitBoard& board = sBoard.board;
    
    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
    const unsigned long long  hash_code = board.hashcode();
    //hTable->entry_prefetch(hash_code, type_hashtable);
    
    RXHashValue entry;
    if(hTable->get(hash_code, type_hashtable, entry)) {
        
        if(!pv && entry.selectivity == NO_SELECT && entry.depth >= board.n_empty) {
            
            if (upper > entry.upper) {
                upper = entry.upper;
                if (upper <= lower) {
                    return upper;
                }
            }
            if (lower < entry.lower) {
                lower = entry.lower;
                if (lower >= upper) {
                    return lower;
                }
            }
            
        }
        
        if(board.isValid_square(entry.move))
            bestmove = entry.move;

    }
    
#ifdef USE_STABILITY
    
    if(bestmove == NOMOVE) {
        
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
    
    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;
        
#ifdef USE_ETC
        unsigned long long  hashcode_after_move;
#endif
        
        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            ((board).*(board.generate_flips[bestmove]))(*move);
            ++board.n_nodes;
            
#ifdef USE_ETC
            hashcode_after_move = board.hashcode_after_move(move);
            hTable->entry_prefetch(hashcode_after_move, type_hashtable);
#endif
            
#ifdef USE_ENHANCED_STABLILITY
            if (lower <= -stability_threshold[board.n_empty-1]  ) {
                const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                if ( stability_bound >= upper ) {
                    return stability_bound;
                }
            }
#endif
            
#ifdef USE_ETC
            //synchronized acces
            if(!pv && hTable->get(hashcode_after_move, type_hashtable, entry) && entry.selectivity == NO_SELECT && entry.depth>=(board.n_empty-1)) {
                
                if(-entry.upper >= upper) {
                    return -entry.upper ;
                }
                
            }
#endif
            
            
            previous = previous->next = move++;
        }
        
        //for all empty square
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove !=NOMOVE)
            legal_movesBB ^= 0x1ULL<<bestmove;
        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;

            if(legal_movesBB & bit){
                legal_movesBB ^= bit;
                
                ((board).*(board.generate_flips[pos]))(*move);
                ++board.n_nodes;
                
#ifdef USE_ETC
                hashcode_after_move = board.hashcode_after_move(move);
                hTable->entry_prefetch(hashcode_after_move, type_hashtable);
#endif
                
#ifdef USE_ENHANCED_STABLILITY
                if (lower <= -stability_threshold[board.n_empty-1]  ) {
                    const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                    int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                    if ( stability_bound >= upper ) {
                        return stability_bound;
                    }
                }
#endif
                
                move->score = 0; //not in Hash
                
#ifdef USE_ETC
                //synchronized acces
                if(hTable->get(hashcode_after_move, type_hashtable, entry) && entry.depth>=(board.n_empty-1)) {
                    
                    if (!pv && entry.selectivity == NO_SELECT && -entry.upper >= upper )
                        return -entry.upper ;
                    
                    move->score = -2;  //in hash
                    
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
            bestscore = -EG_PVS_ETC_mobility(threadID, sBoard, pv, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    } else {
        
        
        if(bestmove != NOMOVE) {
            /* first move */
            list = list->next;
            
            board.do_move(*list);
            bestscore = -EG_PVS_ETC_mobility(threadID, sBoard, pv, -upper, -lower, false);
            board.undo_move(*list);
            
            //bestmove = list->position;
            if (bestscore > lower)
                lower = bestscore;
            
        }
        
        
        if(lower < upper && list->next != nullptr) {
            
            if((list->next)->next != nullptr) {
                
                const int p = board.player;
                const int o = p^1;
                
                //calc answer move->score
                for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
                    
                    ++board.n_nodes;
                    
                    const unsigned long long p_discs = board.discs[p] | (iter->flipped | iter->square);
                    const unsigned long long o_discs = board.discs[o] ^ iter->flipped;
                    
                    //score for try : mobility - corner_stability * 8
                    iter->score += (2*RXBitBoard::get_mobility(o_discs, p_discs) - RXBitBoard::get_corner_stability(p_discs));
                    /*+ (RXBitBoard::count_potential_moves(o_discs, p_discs)>>2); */
                    /*- (((board.parity & RXBitBoard::QUADRANT_ID[iter->position])>>RXBitBoard::QUADRANT_SHIFT[iter->position])>>2); */
                    
                    
                    
                }
                
            }
            
            if(bestmove == NOMOVE) {
                
                //find moves with worst answer
                
                RXMove* move = list->pick_next_promising_move();
                
                board.do_move(*move);
                bestscore = -EG_PVS_ETC_mobility(threadID, sBoard, pv, -upper, -lower, false);
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
                if(move->next != nullptr) {
                    
#ifdef USE_SPLIT_AT_ETC_MOBILITY
                    
                    // Split? not efficient : unused
                    if(activeThreads > 1  && !abort.load() && board.n_empty>=EG_MEDIUM_HI_TO_LOW
                       && !thread_should_stop(threadID) && idle_thread_exists(threadID)
                       && split(sBoard, pv, 0, board.n_empty, NO_SELECT, lower, upper, bestscore, bestmove, list, threadID, RXSplitPoint::END_ETC_MOBILITY)) {
                        
                        break;
                    }
                    
#endif
                    move = list->pick_next_promising_move();

                }
                
                board.do_move(*move);
                score = -EG_PVS_ETC_mobility(threadID, sBoard, false, -lower-1, -lower, false);
                if (lower < score && score < upper)
                    score = -EG_PVS_ETC_mobility(threadID, sBoard, pv, -upper, -score, false);
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
    if(abort.load()  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    //en test 21/01/2025 suspision bug (bestscore >= upper mais stocker comme < beta)
    hTable->update(hash_code, type_hashtable, NO_SELECT, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    
    
    return bestscore;
}

// EG_SP_search_ETC_Mobility() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_PVS_ETC_Mobility() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in EG_SP_search_ETC_Mobility().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::EG_SP_search_ETC_Mobility(RXSplitPoint* sp, const unsigned int threadID) {
    
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    board.n_nodes = 0;
    
    
    //here sp->beta is const
    while(sp->alpha < sp->beta && !abort.load()  && !thread_should_stop(threadID)) {
        
        pthread_mutex_lock(&(sp->lock));
        
        if(sp->list->next == nullptr) {
            pthread_mutex_unlock(&(sp->lock));
            break;
        }
        
        RXMove* move = sp->list->next;
        sp->list = move;
        
        pthread_mutex_unlock(&(sp->lock));
        
        
        const int alpha = sp->alpha; //local copy

        board.do_move(*move);

        int score = -EG_PVS_ETC_mobility(threadID, sBoard, false, -alpha-1, -alpha, false);
        if (alpha < score && score < sp->beta)
            score = -EG_PVS_ETC_mobility(threadID, sBoard, sp->pv, -sp->beta, -score, false);
        
        board.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore) {
            
            //update
            pthread_mutex_lock(&(sp->lock));
            
            if(sp->explored == false) {
                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    if(score > sp->alpha) {
                        
                        if(score >= sp->beta) {
                            sp->explored =true;
                        } else {
                            sp->alpha = score;
                        }
                    }
                }
            }
                
            
            pthread_mutex_unlock(&(sp->lock));
        }
        
    }
    
    pthread_mutex_lock(&(sp->lock));
    
    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
    pthread_mutex_unlock(&(sp->lock));
}




int RXEngine::EG_PVS_deep(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int selectivity, int alpha, const int beta, const bool passed) {
    

    if (sBoard.board.n_empty < EG_DEEP_TO_MEDIUM)
        return EG_PVS_ETC_mobility(threadID, sBoard, pv, alpha, beta, passed);
    
    //time gestion
    if(dependent_time && get_current_dependentTime() > time_limit())
        abort.store(true);

    if(abort.load() || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    
    
    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
    
    RXBitBoard& board = sBoard.board;
    
    //synchronized acces
    RXHashValue entry;
    const unsigned long long hash_code = board.hashcode();
    if(hTable->get(hash_code, type_hashtable, entry)) {
        
        
        if(!pv &&  entry.selectivity >= selectivity && entry.depth >= board.n_empty) {
            
            if (upper > entry.upper) {
                
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            
            if (lower < entry.lower) {
                
                lower = entry.lower;
                if (lower >= upper)
                    return lower;
            }
            
        }
        
        if(board.isValid_square(entry.move))
            bestmove = entry.move;

    }
    
#ifdef USE_STABILITY
    
    if(bestmove == NOMOVE) {
        
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
    if(pv && bestmove == NOMOVE && board.n_empty >= EG_DEEP_TO_MEDIUM) {
        
        if(selectivity > EG_HIGH_SELECT)
            EG_PVS_deep(threadID, sBoard, pv, selectivity-1, -MAX_SCORE, MAX_SCORE, passed); //lower, upper,
        else
            MG_PVS_deep(threadID, sBoard, pv, MG_SELECT, board.n_empty-(USE_PV_EXTENSION? 10 : 8), -MAX_SCORE, MAX_SCORE, passed); //lower, upper,
        
        if(abort.load() || thread_should_stop(threadID))
            return INTERRUPT_SEARCH;
        
        if(hTable->get(hash_code, type_hashtable, entry)) {
            if(board.isValid_square(entry.move))
                bestmove = entry.move;
        }

    }
    
    
    int score, bestscore = UNDEF_SCORE;
    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;
    
    if(bestmove != PASS) {
        
        
        RXMove* move = list + 1;
        RXMove* previous = list;
        
        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            ((board).*(board.generate_flips[bestmove]))(*move);
            ++board.n_nodes;
            
#ifdef USE_ENHANCED_STABLILITY
            
            if (lower <= -stability_threshold[board.n_empty-1]  ) {
                const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                if ( stability_bound >= upper ) {
                    return stability_bound;
                }
            }
#endif
            
#ifdef USE_ETC
            //synchronized acces
            if(!pv && hTable->get(board.hashcode_after_move(move), type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth>=(board.n_empty-1)) {
                
                if(-entry.upper >= upper) {
                    return -entry.upper ;
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
                
                ((board).*(board.generate_flips[pos]))(*move);
                ++board.n_nodes;
                
#ifdef USE_ENHANCED_STABLILITY
                
                if (lower <= -stability_threshold[board.n_empty-1]  ) {
                    const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                    int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                    if ( stability_bound >= upper ) {
                        return stability_bound;
                    }
                }
#endif
                
                move->score = 0;    //not in hash
                
#ifdef USE_ETC
                //synchronized acces
                if(hTable->get(board.hashcode_after_move(move), type_hashtable, entry) && entry.depth>=(board.n_empty-1)) {
                    
                    
                    if(!pv && entry.selectivity >= selectivity && -entry.upper >= upper) {
                        return -entry.upper ;
                    }
                    
                    move->score = -3;    //in hash
                    
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
            
            
            ((sBoard).*(sBoard.update_patterns[list->position][board.player]))(*list);
            
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
                
                if(board.n_empty > EG_DEEP_TO_MEDIUM) {
                    
                    int lower_probcut = -MAX_SCORE;
                    int upper_probcut =  MAX_SCORE;
                    probcut_bounds(board, std::max(EG_HIGH_SELECT, std::min(selectivity-1, NO_SELECT-2)), board.n_empty, (8+(board.n_empty & 1)), 0, lower, upper, lower_probcut, upper_probcut);
                    
                     for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                        
                        ((sBoard).*(sBoard.update_patterns[iter->position][board.player]))(*iter);
                        
                        sBoard.do_move(*iter);
                        
                         
                        int eval_move;
                        if((board.n_empty & 1) == 0) {
                            if(board.n_empty >= 28) {
                                // Near the root: Use Factorization Machine to improve ranking accuracy without sacrificing inference speed.
                                eval_move = -PVS_last_ply(threadID, sBoard, DEPTH_6, -MAX_SCORE, -lower_probcut, false);
                            } else if(board.n_empty >= 24) {
                                eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, DEPTH_4, -MAX_SCORE, -lower_probcut, false);
                            } else {
                                eval_move = -alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -MAX_SCORE, -lower_probcut, false);
                            }
                        } else {
                            if(board.n_empty >= 27) {
                                // Near the root: Use Factorization Machine to improve ranking accuracy without sacrificing inference speed.
                                eval_move = -PVS_last_ply(threadID, sBoard, DEPTH_5, -MAX_SCORE, -lower_probcut, false);
                            } else if(board.n_empty >= 23) {
                                eval_move = -alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, -MAX_SCORE, -lower_probcut, false);
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
                                            
                                            ((board).*(board.generate_flips[pos]))(lastMove);
                                            ((sBoard).*(sBoard.update_patterns[pos][o]))(lastMove);
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
                        int corner_stability = RXBitBoard::get_corner_stability(board.discs[p]);
                        if(board.n_empty < 27 && 22 < board.n_empty) {
                            mobility = 4*mobility/3 - corner_stability/5;
                        } else if(board.n_empty <= 22) {
                            mobility = 3*mobility/2 - corner_stability/3;
                        }
                        
                        iter->score += mobility - eval_move;
                        
                        sBoard.undo_move(*iter);
                        
                    }
                    
                } else {
                    
                    //empties<=EG_DEEP_TO_MEDIUM;
                    
                    for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                        ((sBoard).*(sBoard.update_patterns[iter->position][board.player]))(*iter);
                        
                        ++board.n_nodes;
                        const unsigned long long p_discs = board.discs[p] | (iter->flipped | iter->square);
                        
                        //test 1 : score + 2*mobility_adv + corner_stability/4
                        iter->score += sBoard.get_score<WITHOUT_FM>(*iter) + (8*(RXBitBoard::get_mobility(board.discs[o] ^ iter->flipped, p_discs)) - RXBitBoard::get_corner_stability(p_discs))/3;
                    }
                    
                }
                
                
            } else {
                ((sBoard).*(sBoard.update_patterns[list->next->position][board.player]))(*(list->next));
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
            
            
            //************************************************************************************************
            //dans la methode search sequentielle dans la boucle des autres moves
            //************************************************************************************************
            
            
            /* other moves : try to refute the first/best one */
            for(;!abort.load()  && lower < upper && list->next != nullptr; list = list->next) {
                
                RXMove* move = list->next;
                if (move->next != nullptr) {
                    
                    // Split?
                    if(activeThreads > 1  && !abort.load() && !thread_should_stop(threadID) && idle_thread_exists(threadID)
                       && split(sBoard, pv, 1, board.n_empty, selectivity, lower, upper, bestscore, bestmove, list, threadID, RXSplitPoint::END_PVS)) {
                        
                        break;
                    }
                     
                    move = list->pick_next_promising_move();

                }
                
                
                sBoard.do_move(*move);
                if(selectivity != NO_SELECT) {
                    
                    score = -EG_NWS_XEndCut(threadID, sBoard, 1, selectivity, -lower-1, false); //pvDev == 1
                    if (lower < score && score < upper)
                        score = -EG_PVS_deep(threadID, sBoard, pv, selectivity, -upper, -lower, false); //-score
                    
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
    if(abort  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(   hash_code, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
    hTable_PV->update(hash_code, type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
            
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
    board.n_nodes = 0;
    
    
    //here sp->beta is const
    while(sp->alpha < sp->beta && !abort.load() && !thread_should_stop(threadID)) {
        
        pthread_mutex_lock(&(sp->lock));
        
        if(sp->list->next == nullptr) {
            pthread_mutex_unlock(&(sp->lock));
            break;
        }
        
        RXMove* move = sp->list->next;
        sp->list = move;
        
        pthread_mutex_unlock(&(sp->lock));
        
        
        int score;
        const int alpha = sp->alpha; //local copy
        
        
        sBoard.do_move(*move);
        
        if(sp->selectivity != NO_SELECT) {
            
            score = -EG_NWS_XEndCut(threadID, sBoard, sp->pvDev, sp->selectivity, -alpha-1, false);
            
            if (alpha < score && score < sp->beta)
                score = -EG_PVS_deep(threadID, sBoard, sp->pv, sp->selectivity, -sp->beta, -sp->alpha, false);
            
        } else {
            
            score = -EG_PVS_deep(threadID, sBoard, false, NO_SELECT, -alpha-1, -alpha, false);
            
            if (alpha < score && score < sp->beta)
                score = -EG_PVS_deep(threadID, sBoard, sp->pv, NO_SELECT, -sp->beta, -score, false);
            
        }
        
        sBoard.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore) {
            
            //update
            pthread_mutex_lock(&(sp->lock));
            
            if(sp->explored == false) {
                
                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    if(score > sp->alpha) {
                        
                        if(score >= sp->beta) {
                            sp->explored = true;
                        } else {
                            sp->alpha = score;
                        }
                    }
                }
            }
            
            pthread_mutex_unlock(&(sp->lock));
        }
        
    }
    
    pthread_mutex_lock(&(sp->lock));
    
    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
    pthread_mutex_unlock(&(sp->lock));
}


/*
 NULL Window Search  + XProbCut
 Keep sorting (more efficient) before switching to EG_PVS_ETC_mobility method
 */
int RXEngine::EG_NWS_XEndCut(const unsigned int threadID, RXBBPatterns& sBoard, const int pvDev, const int selectivity, const int alpha, const bool passed) {
    
    //time gestion
    if(dependent_time && get_current_dependentTime() > time_limit())
        abort.store(true);

    if(abort.load()  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;

    RXBitBoard& board = sBoard.board;
    
    unsigned int bestmove = NOMOVE;
    
    
    //synchronized acces
    RXHashValue entry;
    const unsigned long long hash_code = board.hashcode();
    if(hTable->get(hash_code, type_hashtable, entry)) {
        
        if(entry.selectivity >= selectivity && entry.depth>=board.n_empty) {
            
            if(entry.lower > alpha)
                 return entry.lower;
            
            if(entry.upper <= alpha)
                return  entry.upper;
            
            if(board.isValid_square(entry.move))
                bestmove = entry.move;

        }
        
        
    }
    
#ifdef USE_STABILITY
    if(bestmove == NOMOVE) {
        if ( alpha+1 >= stability_threshold[board.n_empty] ) {
            
            int stability_bound = 64- 2 * board.get_stability(board.player^1);
            if ( stability_bound <= alpha )
                return stability_bound;
        }
    }
#endif
    
    //param mpc
    int lower_probcut, upper_probcut;
    int depth_probcut = (board.n_empty/4)*2 + (board.n_empty & 0x1UL);
    probcut_bounds(board, selectivity, board.n_empty, depth_probcut, pvDev, alpha, alpha+1, lower_probcut, upper_probcut);

    if(bestmove != NOMOVE && entry.selectivity >= selectivity && entry.depth>=depth_probcut) {
        
        if(entry.lower >= upper_probcut) {
            return alpha + 1; //9/02/2025
        }
#ifdef USE_PROBCUT_ALPHA
        if(entry.upper <= lower_probcut) {
            return alpha;
        }
#endif
        
    }

    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;
    
    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;
        
#ifdef USE_ETC
        unsigned long long  hashcode_after_move;
#endif
        
        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            ((board).*(board.generate_flips[bestmove]))(*move);
            ++board.n_nodes;
            
#ifdef USE_ETC
            hashcode_after_move = board.hashcode_after_move(move);
            hTable->entry_prefetch(hashcode_after_move, type_hashtable);
#endif
            
#ifdef USE_ENHANCED_STABLILITY
            if (alpha <= -stability_threshold[board.n_empty-1]  ) {
                const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                if ( stability_bound > alpha ) {
                    return stability_bound; // alpha
                }
            }
#endif
            
            //synchronized acces
#ifdef USE_ETC
            
            if(hTable->get(hashcode_after_move, type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth >= (board.n_empty-1)) {
                
                if(-entry.upper > alpha) {
                    return -entry.upper;
                }
                
            }
#endif
            
            
            previous = previous->next = move++;
            
        }
        
        //for all empty square
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove !=NOMOVE)
            legal_movesBB ^= 0x1ULL<<bestmove;
        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;
            
            if(legal_movesBB & bit) {
                legal_movesBB ^= bit;
                
                ((board).*(board.generate_flips[pos]))(*move);
                ++board.n_nodes;
                
#ifdef USE_ETC
                hashcode_after_move = board.hashcode_after_move(move);
                hTable->entry_prefetch(hashcode_after_move, type_hashtable);
#endif
                
#ifdef USE_ENHANCED_STABLILITY
                if (alpha <= -stability_threshold[board.n_empty-1]  ) {
                    const unsigned long long d_player = board.discs[board.player] | (move->flipped | move->square);
                    int stability_bound = 2 * board.get_stability(d_player, board.discs[board.player^1] ^ move->flipped) - 64;
                    if ( stability_bound > alpha ) {
                        return stability_bound; // alpha
                    }
                }
#endif
                
                move->score = 0;
                
                //synchronized acces
#ifdef USE_ETC
                
                if(hTable->get(hashcode_after_move, type_hashtable, entry) && entry.depth>=(board.n_empty-1)) {
                    
                    move->score = -3;
                    
                    if(-entry.upper > alpha) {
                        
                        if(entry.selectivity >= selectivity ) {
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
    
    int bestscore = UNDEF_SCORE;
    
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
        
        //XProbcut
        int type_probcut = probcut(threadID, sBoard, selectivity, alpha, board.n_empty, depth_probcut, lower_probcut, upper_probcut, list, bestmove != NOMOVE);
        if( type_probcut == BETA_CUT) {
            return alpha + 1; //9/02/2025
        }
#ifdef USE_PROBCUT_ALPHA
        if(type_probcut == ALPHA_CUT) {
            return alpha;
        }
#endif
        
        //interrupt search
        if(type_probcut == INTERRUPT_SEARCH)
            return INTERRUPT_SEARCH;

        //fisrt move
        RXMove* move = list->next;
        
        if(board.n_empty<MIN_DEPTH_USE_ENDCUT) {
            board.do_move(*move);
            bestscore = -EG_PVS_ETC_mobility(threadID, sBoard, false, -alpha-1, -alpha, false);
            board.undo_move(*move);
        } else {
            sBoard.do_move(*move);
            bestscore = -EG_NWS_XEndCut(threadID, sBoard, pvDev, selectivity, -alpha-1, false);
            sBoard.undo_move(*move);
        }
        
        bestmove = move->position;
        list = list->next;
        
        
        
        int score;
        for(RXMove* iter = list->next; !abort.load()  && bestscore<=alpha && iter != nullptr; iter = iter->next, list = list->next) {
            
            if(activeThreads > 1 && board.n_empty>=MIN_DEPTH_USE_ENDCUT && (list->next)->next != nullptr
               && !abort.load() && !thread_should_stop(threadID) &&  idle_thread_exists(threadID)
               && split(sBoard, false, pvDev+1, board.n_empty, selectivity,
                        alpha, (alpha + 1), bestscore, bestmove, list, threadID, RXSplitPoint::END_XPROBCUT)) {
             
                break;
            }
            
            
            if(board.n_empty<MIN_DEPTH_USE_ENDCUT) {
                board.do_move(*iter);
                score = -EG_PVS_ETC_mobility(threadID, sBoard, false, -alpha-1, -alpha, false);
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
    if(abort.load()  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(hash_code, type_hashtable, (board.n_empty<MIN_DEPTH_USE_ENDCUT ? NO_SELECT: selectivity), DEPTH_BOOSTER+board.n_empty, alpha, bestscore, bestmove);
    if(pvDev < 4)
        hTable_PV->update(hash_code, type_hashtable, (board.n_empty<MIN_DEPTH_USE_ENDCUT ? NO_SELECT: selectivity), DEPTH_BOOSTER+board.n_empty, alpha, bestscore, bestmove);
    
    return bestscore;
    
    
}

// EG_SP_search_XEndcut() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_NWS_XEndCut() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in EG_SP_search_XEndcut().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::EG_SP_search_XEndcut(RXSplitPoint* sp, const unsigned int threadID) {
    
    //    assert(threadID >= 0 && threadID < activeThreads);
    //    assert(activeThreads > 1);
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    board.n_nodes = 0;
    
    //here sp->alpha is const
    while(sp->bestscore <= sp->alpha && !abort.load()  && !thread_should_stop(threadID)) {
        
        pthread_mutex_lock(&(sp->lock));
        
        if(sp->list->next == nullptr) {
            pthread_mutex_unlock(&(sp->lock));
            break;
        }
        
        RXMove* move = sp->list->next;
        sp->list = move;
        
        
        pthread_mutex_unlock(&(sp->lock));
        
        const int alpha = sp->alpha; //local copy
        
        sBoard.do_move(*move);
        int score = -EG_NWS_XEndCut(threadID, sBoard, sp->pvDev, sp->selectivity, -alpha-1, false);
        sBoard.undo_move(*move);
        
        
        //first without mutex
        if(score > sp->bestscore) {
            
            pthread_mutex_lock(&(sp->lock));
            
            //update SplitPoint
            if(sp->explored == false) {
                
                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    if(score > sp->alpha) {
                        sp->explored = true;
                    }
                }
            }
            
            
            pthread_mutex_unlock(&(sp->lock));
        }
    }
    
    pthread_mutex_lock(&(sp->lock));
    
    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
    pthread_mutex_unlock(&(sp->lock));
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
void RXEngine::EG_PVS_root(RXBBPatterns& sBoard, const int selectivity, int alpha, const int beta, RXMove* list)
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
        bestscore = -board.final_score_4(-upper, -lower, false);
    } else if (board.n_empty < EG_MEDIUM_TO_SHALLOW) {
        bestscore = -EG_alphabeta_parity(0, board, -upper, -lower, false);
    } else if (board.n_empty < EG_MEDIUM_HI_TO_LOW) {
        bestscore = -EG_PVS_hash_mobility(0, board, true, -upper, -lower, false);
    } else  if (board.n_empty < EG_DEEP_TO_MEDIUM) {
        bestscore = -EG_PVS_ETC_mobility(0, sBoard, true, -upper, -lower, false);
    } else {
        bestscore = -EG_PVS_deep(0, sBoard, true, selectivity, -upper, -lower, false);
    }
    
    sBoard.undo_move(*iter);
    
    if(!abort.load() && std::abs(bestscore) != INTERRUPT_SEARCH) {
        
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
        
        for (iter = iter->next; !abort.load()  && lower < upper && iter != nullptr; iter = iter->next) {
            
#ifdef USE_SPLIT_AT_ROOT
            
            if(activeThreads > 1 && iter->next != nullptr && board.n_empty >= EG_DEEP_TO_MEDIUM
               && !abort.load() && idle_thread_exists(0) && !thread_should_stop(0)
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
                score = -board.final_score_4(-upper, -lower, false);
            } else {
                
                //simple_pv PV == false
                if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
                    score = -EG_alphabeta_parity(0, board, -lower-1, -lower, false);
                else if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
                    score = -EG_PVS_hash_mobility(0, board, false, -lower-1, -lower, false);
                else if (board.n_empty < EG_DEEP_TO_MEDIUM)
                    score = -EG_PVS_ETC_mobility(0, sBoard, false, -lower-1, -lower, false); //simple-PV pv == false ????
                else
                    score = -EG_PVS_deep(0, sBoard, false, selectivity, -lower-1, -lower, false); //simple-PV pv == false ????
                
                
                if (!abort.load() && board.n_empty > 4 && lower < score && score < upper) {
                    
                    if(search_client == RXSearch::kGGSMode) {	// GGS mode
                        
                        if(dependent_time && board.n_empty>19)
                            manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " maybe better? ");
                        
                    }
                    
                    if (board.n_empty < EG_MEDIUM_TO_SHALLOW)
                        score = -EG_alphabeta_parity(0, board, -upper, -score, false);
                    else if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
                        score = -EG_PVS_hash_mobility(0, board, true, -upper, -score, false);
                    else if (board.n_empty < EG_DEEP_TO_MEDIUM)
                        score = -EG_PVS_ETC_mobility(0, sBoard, true, -upper, -score, false);
                    else {
                        ++extra_time;
                        //                        *log << "                  [extra time > :" << extra_time << "]" << std::endl;
                        
                        score = -EG_PVS_deep(0, sBoard, true, selectivity, -upper, -score, false);
                        
                        if(search_client == RXSearch::kGGSMode && !abort.load()) {    // GGS mode
                            if(dependent_time && board.n_empty>19 && score <= bestscore)
                                manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " refuted ");
                        }
                        
                        
                        --extra_time;
                        //                        *log << "                  [extra time end :" << extra_time << "]" << std::endl;
                        
                    }
                    
                    
                }
                
                
                
            }
            
            
            
            sBoard.undo_move(*iter);
            
            
            if (!abort.load()  && std::abs(score) != INTERRUPT_SEARCH) {
                
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
        list->next->selectivity = selectivity;
        list->next->depth = board.n_empty;
        
        //        *log << "                  [score " << bestscore << " ]" << std::endl;
        
        hTable->update(sBoard.board.hashcode(), type_hashtable, selectivity, DEPTH_BOOSTER+board.n_empty, alpha, upper, bestscore, bestmove);
        
        
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
    board.n_nodes = 0;
    
    //here sp->beta is const
    while(sp->alpha < sp->beta && !abort.load() && !thread_should_stop(threadID)) {
        
        pthread_mutex_lock(&(sp->lock));
        
        if(sp->list == nullptr) {
            pthread_mutex_unlock(&(sp->lock));
            break;
        }
        
        RXMove* move = sp->list;
        sp->list = move->next;
        
        pthread_mutex_unlock(&(sp->lock));
        
        const int alpha = sp->alpha; //local copy
        
        sBoard.do_move(*move);
        
        
        int score = -EG_PVS_deep(threadID, sBoard, false, sp->selectivity, -alpha-1, -alpha, false);
        
        if (!(abort.load() || thread_should_stop(threadID)) && alpha < score && score < sp->beta) {
            
            ++extra_time;
            
            if(dependent_time && board.n_empty>19)
                manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " maybe better? ");
            
            if(sp->selectivity != NO_SELECT)
                score = -EG_PVS_deep(threadID, sBoard, true, sp->selectivity, -sp->beta, -sp->alpha, false);
            else
                score = -EG_PVS_deep(threadID, sBoard, true, sp->selectivity, -sp->beta, -score, false);
            
            if(search_client == RXSearch::kGGSMode && !(abort.load() || thread_should_stop(threadID))) {    // GGS mode
                if(dependent_time && board.n_empty>19 && score <= sp->bestscore)
                    manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " refuted ");
            }
            
            
            --extra_time; //atomic = thread-safe
            
        }
        
        sBoard.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore) {
            //update
            pthread_mutex_lock(&(sp->lock));
            
            if(sp->explored == false) {
                
                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    
                    if(dependent_time && board.n_empty>19)
                        manager->sendMsg(showBestmove(board.n_empty, sp->selectivity, sp->alpha, sp->beta, sp->bestscore, sp->bestmove));
                    
                    if(score > sp->alpha) {
                        
                        if(score >= sp->beta) {
                            sp->explored = true;
                        } else {
                            sp->alpha = score;
                        }
                        
                    }
                }
            }
            
            pthread_mutex_unlock(&(sp->lock));
        }
        
    }
    
    pthread_mutex_lock(&(sp->lock));
    
    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
    pthread_mutex_unlock(&(sp->lock));
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
    for(selectivity = 2; !abort.load()  && selectivity <= end_selectivity; selectivity+=(selectivity == NO_SELECT? 1:(NO_SELECT-selectivity)))
#else
    for(; !abort.load()  && selectivity <= end_selectivity; selectivity++)
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
        
        while (!abort.load() && !(alpha < score && score < beta)) {
            
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
        best_answer.depth = list->next->depth;
        best_answer.selectivity = CONFIDENCE[list->next->selectivity];
        
        best_answer.nodes += sBoard.board.n_nodes;
        
        
        int eTime = get_current_time();
        
        
        //unsynchronized acces
        RXHashValue entry;
        if(hTable->get(sBoard.board, type_hashtable, entry) && entry.depth >= sBoard.board.n_empty && entry.selectivity>=selectivity) {
            int type = EXACT;
            
            score = list->next->score;
            if(entry.lower == entry.upper) {
                if(abort.load())
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
        if(!abort.load() && sBoard.board.n_empty-6 > 0 && selectivity == NO_SELECT && s_alpha <= list->next->score && list->next->score <= s_beta) {
            RXSearch::t_client save_client = search_client;
            search_client = RXSearch::kPrivate;
            EG_check_PV(search_sBoard, list->next->score);
            search_client = save_client;
        }
#endif

        
        if(abort.load() )
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
       EG_check_PV(pv, sBoard, -score);

}

bool RXEngine::EG_check_PV(std::vector<unsigned char>& pv, RXBBPatterns& sBoard, const int score) {

    bool good_pv = true;
    RXBitBoard& board = sBoard.board;

    const int pos = pv.front();
    if(pos != NOMOVE) {
        
        
        RXMove& move = threads[0]._move[board.n_empty][1];
        if(pos == PASS) {
            board.do_pass();
        } else {
            ((board).*(board.generate_flips[pos]))(move);
            ((sBoard).*(sBoard.update_patterns[pos][board.player]))(move);
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
                    ((sBoard).*(sBoard.update_patterns[iter->position][board.player]))(*iter);
 
                //version monogame [provoque bug affichage]
                hTable->reset();
                EG_PVS_root(sBoard, NO_SELECT, score-1, score+1, list);
                
                int result = list->next->score;
                
                if(result == score)
                    good_pv = EG_check_PV(pv, sBoard, -score);
                else {
                    
                    std::cout << "RED ALERT : wrong PV" << std::endl;
                    std::cout << "bad move : " << RXMove::index_to_coord(pos) << std::endl;
                    std::cout << "score = " << -score << " result = " << result << std::endl;

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
    
    if(!good_pv) {
        std::cout << "hashcode : " << board.hashcode() << std::endl;
        std::cout << board << std::endl;
    }


    return good_pv;
    
}

