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

#include <iomanip>
#include <fstream>
#include <sstream>
#include <sys/time.h>

#include "RXEngine.hpp"
#include "RXRoxane.hpp"


const int RXEngine::CONFIDENCE[]   = {  60,    72,    84,    91,    95,    98,   100};
const float RXEngine::PERCENTILE[] = {1.00f, 1.15f, 1.40f, 1.80f, 2.45f, 3.20f};

const int RXEngine::EG_HIGH_SELECT = 0;
const int RXEngine::MG_SELECT = 1; //72%
const int RXEngine::NO_SELECT = std::size(RXEngine::PERCENTILE);

//const int RXEngine::CONFIDENCE[]   = {   72,    84,    91,    95,    98,   100};
//const float RXEngine::PERCENTILE[] = {1.15f, 1.40f, 1.80f, 2.45f, 3.20f};
//
//const int RXEngine::EG_HIGH_SELECT = 0;
//const int RXEngine::MG_SELECT = 0; //72%
//const int RXEngine::NO_SELECT = std::size(RXEngine::PERCENTILE);


const int RXEngine::DEPTH_BOOSTER = DEPTH_4;

const unsigned int RXEngine::LMR_NO_REDUCTION = 0;


extern "C"
void* init_threadHelper(void* pt) {
    RXEngine::ThreadLaunchArgs* args = (RXEngine::ThreadLaunchArgs*)(pt);
    RXEngine*    engine   = args->engine;
    unsigned int threadID = args->threadID;
    delete args;  // libère la mémoire allouée dans init_threads
    engine->idle_loop(threadID, nullptr);
    return nullptr;
}

extern "C"
void* init_pthreadMain(void* pt)  {
    
    RXEngine* engine = (RXEngine*)(pt);
    
    engine->run();
    
    return nullptr;
    
}


RXEngine::RXEngine(RXRoxane* _manager, std::string filename, int maxThread):
manager(_manager), THREAD_MAX(maxThread),
select_search(0), allThreadsShouldExit(false), allThreadsShouldSleep(true) {
    
    // Construction en place pour éviter toute copie (pthread_mutex_t non copiable)
    threads.reserve(maxThread);
    for(int i = 0; i < maxThread; i++)
        threads.emplace_back(maxThread, ACTIVE_SPLITPOINT_MAX);
    
    pthread_mutex_init(&MP_sync, nullptr);
    pthread_mutex_init(&mutex, nullptr);
    
    init_threads();
    
    hTable_shallow = new RXHashShallow(19);
    
    logfile = new std::ofstream(filename.c_str());
    
#ifdef LOGGING_ON
    log = logfile; // login actif
#else
    log = &nullStream; // logging désactivé
#endif
    
}


RXEngine::~RXEngine() {
    
    //order inverted
#ifdef LOGGING_ON
    // if log is an ofstream and it's open, close it
    if (logfile) {
        if (logfile->is_open())
            logfile->close();
        delete logfile;
    }
#endif

    delete hTable_shallow;
    
    stop_threads();
    
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&MP_sync);
    
}


unsigned int RXEngine::confidence_to_selectivity(int c) {
    if(1<=c && c<=99)
        for(int i = 0; i<NO_SELECT; i++)
            if(CONFIDENCE[i] >= c)
                return i;
    
    return NO_SELECT;
}

unsigned int RXEngine::selectivity_to_confidence(int s) {
    if(0<=s && s<=NO_SELECT)
        return CONFIDENCE[s];
    
    return CONFIDENCE[NO_SELECT];
}


void RXEngine::sort_moves(const unsigned int threadID, const bool endgame, RXBBPatterns& sBoard, const int depth, const int selectivity, const int alpha, const int beta, RXMove* list, const bool hashMove) {
    
    RXBitBoard& board = sBoard.board;
    
    //sort moves list
    if(list->next != nullptr) {
        
        RXMove* iter = list->next;
        
        if(iter->next != nullptr) {
            
            const int p = board.player;
            const int o = p^1;
            
            if(depth>DEPTH_4) {
                
                int lower_probcut = -MAX_SCORE;
                int upper_probcut =  MAX_SCORE;
                
                int eval_error;
                
                if(endgame)
                    eval_error = probcut_bounds(board, std::max(EG_HIGH_SELECT, selectivity-1), board.n_empty, (8+(board.n_empty & 1)), 0, alpha, beta, lower_probcut, upper_probcut);
                else
                    eval_error = probcut_bounds(board, MG_SELECT, depth, std::min(depth-2, (6 - (depth & 1))), 0, alpha, beta, lower_probcut, upper_probcut);
                
                
                lower_probcut = std::max(static_cast<int>(-MAX_SCORE), alpha-2*eval_error);
                upper_probcut = std::min(static_cast<int>( MAX_SCORE), beta +2*eval_error);
                
                
                for(; iter != nullptr; iter = iter->next) {
                    sBoard.patterns_update(*iter);
                    
                    sBoard.do_move(*iter);
                                        
                    int eval_move;
                    //Idea: if there is a hashmove and it didn't cause a cutoff, it's probably an all-node.
                    if(!hashMove && depth>(endgame? 18:20)) { //from endgame 36 / midgame 40
                        
                        if((board.n_empty & 1) == 0)
                            eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_6, -upper_probcut, -lower_probcut, false);
                        else
                            eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_5, -upper_probcut, -lower_probcut, false);
                        
                    } else if(depth>(endgame? 12:14)) { //from endgame 24 / midgame 28
                        
                        if((board.n_empty & 1) == 0)
                            eval_move = -PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_4, -upper_probcut, -lower_probcut, false);
                        else
                            eval_move = -alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, true, -upper_probcut, -lower_probcut, false);
                        
                    } else if(depth>8) { //from midgame 16
                        
                        if((board.n_empty & 1) == 0)
                            eval_move = -alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -upper_probcut, -lower_probcut, false);
                        else
                            eval_move = -alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, true, -upper_probcut, -lower_probcut, false);
                        
                    } else { //from midgame 10
                        
                        if((board.n_empty & 1) == 0) {
                            
                            eval_move = -alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -upper_probcut, -lower_probcut, false);
                            
                        } else {
                            
                            
                            int bestscore = UNDEF_SCORE; //masquage
                            
                            unsigned long long legal_movesBB = RXBitBoard::get_legal_moves(board.discs[o], board.discs[p]);
                            if(legal_movesBB) {
                                
                                RXMove& lastMove = threads[threadID]._move[board.n_empty][1];
                                RXSquareList* empties = board.empties_list;
                                do {
                                    empties = empties->next;

                                    const int pos = empties->position;
                                    const unsigned long long bit = 0x1ULL << pos;

                                    if(legal_movesBB & bit) {
                                        legal_movesBB ^= bit;
                                        board.generate_flips(pos, lastMove);
                                        sBoard.patterns_update(lastMove);
                                        ++board.n_nodes;
                                        
                                        int score = -sBoard.get_score<WITHOUT_FM>(lastMove);
                                        if (score>bestscore) {
                                            bestscore = score;
                                        }
                                        
                                    }
                                    
                                } while(bestscore < -lower_probcut && legal_movesBB);
                                
                                
                            } else {
                                //PASS
                                sBoard.board.do_pass();
                                bestscore = -sBoard.get_score<WITHOUT_FM>();
                                sBoard.board.do_pass();
                            }
                            
                            eval_move = -bestscore;
                            
                        }
                        
                    }
                    
                    
                    
                    iter->score -= eval_move;
                    
                    if(iter->score <= -upper_probcut) {
                        //good move    "probable beta cut"    study in first
                        iter->score -= 12;
                    }
                    
                    
                    if(endgame) {
                        
                        int mobility = RXBitBoard::get_mobility(board.discs[o], board.discs[p]);
                        
                        if(depth <= 17 && 11 < depth) {
                            mobility = (5*mobility)/4;
                        } else if(depth <= 11 /*&& 5 < depth*/) {
                            mobility = (3*mobility)/2;
                        }
                        
                        iter->score += mobility;
                    }

                    
                    sBoard.undo_move(*iter);
                    
                }
                
            } else {
                
                for(; iter != nullptr; iter = iter->next) {
                    ++board.n_nodes;
                    
                    sBoard.patterns_update(*iter);
                    iter->score += sBoard.get_score<WITHOUT_FM>(*iter);
                    
                }
            }
            
            list->sort_by_score();
            
        } else {
            sBoard.patterns_update(*iter);
        }
        
    }
    
}

int RXEngine::probcut(const unsigned int threadID, RXBBPatterns& sBoard, const int selectivity, const int alpha, const int depth, const int depth_probcut, const int lower_probcut, const int upper_probcut, RXMove* list, const bool hashMove) {
        
    RXBitBoard& board = sBoard.board;
    
    int bestscore = UNDEF_SCORE;
    
    RXMove* list1 = list;
    
    
    const int beta = alpha+1;
    const int eval_error_0 = static_cast<int>(std::round(PERCENTILE[selectivity] * sigma(board.n_empty, depth, 0)));

    const int eval_0 = sBoard.get_score<WITHOUT_FM>();
    
    if(hashMove) {

        list1= list->next ;
        
        sBoard.patterns_update(*list1);
        
        if (upper_probcut < 64 && eval_0 >= (beta - eval_error_0) && sBoard.get_score<WITHOUT_FM>(*list1) <= (eval_error_0-alpha)) {

            sBoard.do_move(*list1);

            //Keep search depth in parity with the number of empty cells.

            if(depth_probcut == DEPTH_2) {
                
                int bestscore_1 = UNDEF_SCORE;
                
                unsigned long long legal_movesBB = board.get_legal_moves();
                if(legal_movesBB) {
                    
                    RXMove& lastMove = threads[threadID]._move[board.n_empty][1];
                    RXSquareList* empties = board.empties_list;
                    do {
                        empties = empties->next;
                        
                        const int pos = empties->position;
                        const unsigned long long bit = 0x1ULL << pos;
                        
                        if(legal_movesBB & bit) {
                            legal_movesBB ^= bit;
                            
                            board.generate_flips(pos, lastMove);
                            sBoard.patterns_update(lastMove);
                            ++board.n_nodes;
                            
                            int score= -sBoard.get_score(lastMove);
                            if (score>bestscore_1)
                                bestscore_1 = score;
                            
                        }
                    } while(bestscore_1 <= -upper_probcut && legal_movesBB);
                    
                } else {
                    //PASS
                    sBoard.board.do_pass();
                    bestscore_1 = -sBoard.get_score();
                    sBoard.board.do_pass();
                }
                
                bestscore = -bestscore_1;
                
            } else if(depth_probcut == DEPTH_3) {
                bestscore = -alphabeta_last_two_ply(threadID, sBoard, -upper_probcut, -upper_probcut+1, false);
            } else if(depth_probcut == DEPTH_4) {
                bestscore = -alphabeta_last_three_ply(threadID, sBoard, false, -upper_probcut, -upper_probcut+1, false);
            } else if(depth_probcut <= DEPTH_5) {
                bestscore = -PVS_last_ply(threadID, sBoard, false, depth_probcut-1, -upper_probcut, -upper_probcut+1, false);
            } else {
                bestscore = -MG_NWS_XProbCut(threadID, sBoard, 0, selectivity, depth_probcut-1, -upper_probcut, false); // reset pvDev = 0
            }
            
            sBoard.undo_move(*list1);
            
            //interrupt search
            if(abort.load() || thread_should_stop(threadID))
                return INTERRUPT_SEARCH;
            
            if(bestscore >= upper_probcut) { //beta cut
                
                hTable->update(board.hashcode(), board, type_hashtable, (depth_probcut>DEPTH_5? selectivity:NO_SELECT), depth_probcut, upper_probcut-1, bestscore, list1->position);
                return BETA_CUT;
            }
        }
    }
    
    sort_moves(threadID, board.n_empty == depth, sBoard, depth_probcut, selectivity, lower_probcut, upper_probcut, list1, hashMove);

    if(eval_0 >= (beta - eval_error_0) && upper_probcut < 64) {

            //beta prob cut
        for(RXMove* iter = list1->next; iter != nullptr; iter = iter->next) {
            
            if (sBoard.get_score<WITHOUT_FM>(*iter) <= (eval_error_0-alpha)) {

                sBoard.do_move(*iter);

                //Keep search depth in parity with the number of empty cells.

                if(depth_probcut == DEPTH_2) {

                    int bestscore_1 = UNDEF_SCORE;
                    
                    unsigned long long legal_movesBB = board.get_legal_moves();
                    if(legal_movesBB) {
                        
                        RXMove& lastMove = threads[threadID]._move[board.n_empty][1];
                        RXSquareList* empties = board.empties_list;
                        do {
                            empties = empties->next;

                            const int pos = empties->position;
                            const unsigned long long bit = 0x1ULL << pos;

                            if(legal_movesBB & bit) {
                                legal_movesBB ^= bit;
                                
                                board.generate_flips(pos, lastMove);
                                sBoard.patterns_update(lastMove);
                                ++board.n_nodes;
                                
                                int score= -sBoard.get_score(lastMove);
                                if (score> bestscore_1) {
                                    bestscore_1 = score;
                                }
                            }
                        } while(bestscore_1 <= -upper_probcut && legal_movesBB);
                        
                    } else {
                        //PASS
                        sBoard.board.do_pass();
                        bestscore_1 = -sBoard.get_score();
                        sBoard.board.do_pass();
                    }
                    
                    bestscore = -bestscore_1;
                    
                } else if(depth_probcut == DEPTH_3) {
                    bestscore = -alphabeta_last_two_ply(threadID, sBoard, -upper_probcut, -upper_probcut+1, false);
                } else if(depth_probcut == DEPTH_4) {
                    bestscore = -alphabeta_last_three_ply(threadID, sBoard, false, -upper_probcut, -upper_probcut+1, false);
                } else if(depth_probcut <= DEPTH_5) {
                    bestscore = -PVS_last_ply(threadID, sBoard, false, depth_probcut-1, -upper_probcut, -upper_probcut+1, false);
                } else {
                    bestscore = -MG_NWS_XProbCut(threadID, sBoard, 0, selectivity, depth_probcut-1, -upper_probcut, false); // reset pvDev = 0
                }
                
                sBoard.undo_move(*iter);
                
                //interrupt search
                if(abort.load() || thread_should_stop(threadID))
                    return INTERRUPT_SEARCH;
                
                if(bestscore >= upper_probcut) { //beta cut
                    
                    hTable->update(board.hashcode(), board, type_hashtable, (depth_probcut>DEPTH_5? selectivity:NO_SELECT), depth_probcut, upper_probcut-1, bestscore, iter->position);
                    return BETA_CUT;
                }
            }
        }
    }
        
#ifdef USE_PROBCUT_ALPHA
    
    if(eval_0 < (alpha + eval_error_0) && lower_probcut > -64) {
                
        unsigned int bestmove = list->next->position;
        bestscore = UNDEF_SCORE;
        
        for(RXMove* iter = list->next; iter != nullptr; iter = iter->next) {
            
            if(sBoard.get_score(*iter) > (eval_error_0-beta))
               continue;
                
                sBoard.do_move(*iter);
                
                if(depth_probcut == DEPTH_2) {
                    
                    int bestscore_1 = UNDEF_SCORE;
                    
                    unsigned long long legal_movesBB = board.get_legal_moves();
                    if(legal_movesBB) {
                        
                        RXMove& lastMove = threads[threadID]._move[board.n_empty][1];
                        RXSquareList* empties = board.empties_list;
                        do {
                            empties = empties->next;
                            
                            const int pos = empties->position;
                            const unsigned long long bit = 0x1ULL << pos;

                            if(legal_movesBB & bit) {
                                legal_movesBB ^= bit;
                                
                                board.generate_flips(pos, lastMove);
                                sBoard.patterns_update(lastMove);
                                ++board.n_nodes;
                                
                                
                                int score= -sBoard.get_score(lastMove);
                                if (score>bestscore_1) {
                                    bestscore_1 = score;
                                }
                                
                                
                            }
                        } while(bestscore_1 < -lower_probcut && legal_movesBB);
                        
                    } else  {
                        //PASS
                        sBoard.board.do_pass();
                        bestscore_1 = -sBoard.get_score();
                        sBoard.board.do_pass();
                    }
                    
                    iter->score = -bestscore_1;
                    
                } else if(depth_probcut == DEPTH_3) {
                    iter->score = -alphabeta_last_two_ply(threadID, sBoard, -lower_probcut-1, -lower_probcut, false);
                } else if(depth_probcut == DEPTH_4) {
                    iter->score = -alphabeta_last_three_ply(threadID, sBoard, false, -lower_probcut-1, -lower_probcut, false);
                } else if(depth_probcut <= DEPTH_5) {
                    iter->score = -PVS_last_ply(threadID, sBoard, false, depth_probcut-1, -lower_probcut-1, -lower_probcut, false);
                } else {
                    iter->score = -MG_NWS_XProbCut(threadID, sBoard, 0, selectivity, depth_probcut-1, -lower_probcut-1, false); // reset pvDev = 0
                }
                
                sBoard.undo_move(*iter);
                
                //interrupt search
                if(abort.load() || thread_should_stop(threadID))
                    return INTERRUPT_SEARCH;
                
                if (iter->score > bestscore) {
                    
                    bestmove = iter->position;
                    bestscore = iter->score;
                    
                    if(bestscore > lower_probcut) { //no cut
                        hTable->update(board.hashcode(), board, type_hashtable, (depth_probcut>DEPTH_5? selectivity:NO_SELECT), depth_probcut, lower_probcut, bestscore, bestmove);
                        return NO_CUT;
                    }
                }
        }
        
        //bestscore <= lower_probcut
        
        if(bestscore != UNDEF_SCORE)
            hTable->update(board.hashcode(), board, type_hashtable, (depth_probcut>DEPTH_5? selectivity:NO_SELECT), depth_probcut, lower_probcut, bestscore, bestmove);
        return ALPHA_CUT;
        
        
    }
    
#endif
    
    return NO_CUT;
    
}

//  PVS:  3 < depth <= 6
template <const bool UseFM>
int RXEngine::PVS_last_ply(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, int depth, int alpha, const int beta, const bool passed) {
        
    if(depth == DEPTH_3)
        return alphabeta_last_three_ply<UseFM>(threadID, sBoard, pv, alpha, beta, passed);

    RXBitBoard& board = sBoard.board;
    const unsigned long long  hash_code = board.hashcode();
    
    unsigned int bestmove = NOMOVE;
    
    int upper = beta;
    int lower = alpha;
    
    
    //synchronized access
    RXHashValue entry;
    if(hTable_shallow->get(hash_code, board, entry)) {
        if(entry.depth >= depth) {
            
            if (upper > entry.upper) {
                upper = entry.upper;
                if (upper <= lower)
                    return upper;
            }
            
            if (!pv && entry.lower >= upper) {
                return entry.lower;
            }
            
//            if(board.isValid_square(entry.move))
            bestmove = entry.move;

        }

    }
    
    int bestscore = UNDEF_SCORE;
    
    if(bestmove != PASS) {
        
        RXMove* list = threads[threadID]._move[board.n_empty];
        
        if(bestmove != NOMOVE) {
            
            RXMove& move = list[1];
            board.generate_flips(bestmove, move);
            sBoard.patterns_update(move);
            
            //first move
            sBoard.do_move(move);
            bestscore = -PVS_last_ply<UseFM>(threadID, sBoard, pv, depth-1, -upper, -lower, false);
            sBoard.undo_move(move);
            
            
            if(bestscore>lower)
                lower = bestscore;
        }
        
        
        if(lower < upper ) {
            
            
            if(bestmove != NOMOVE) {
                board.moves_producing(list, 0x1ULL << bestmove);
            } else {
                board.moves_producing(list);
            }
            
            if(list->next != nullptr) {
                
                RXMove* iter = list->next;
                
                if(iter->next != nullptr) { // more than 1 move
                    // Evaluate moves for a future sort
                                        
                    for(iter = list->next; iter != nullptr; iter = iter->next) {
                        sBoard.patterns_update(*iter);
                        
                        sBoard.do_move(*iter);
                        
                        if (depth == DEPTH_6) {
                            
                            // [endgame n_empty >= 30 -> n_empty/4 * 2]
                            
                            int bestscore1 = UNDEF_SCORE;
                            
                            unsigned long long legal_movesBB = board.get_legal_moves();
                            if(legal_movesBB) {
                                
                                RXMove& lastMove = threads[threadID]._move[board.n_empty][1];

                                // Skipping JWC order is faster, but JWC tie-breaking for equal scores is lost.
                                do {
                                    const int pos = __builtin_ctzll(legal_movesBB);  // index du bit le plus bas
                                    legal_movesBB &= legal_movesBB - 1;              // retire ce bit

                                    board.generate_flips(pos, lastMove);
                                    sBoard.patterns_update(lastMove);
                                    ++board.n_nodes;

                                    int score = -sBoard.get_score<WITHOUT_FM>(lastMove);
                                    if (score > bestscore1) {
                                        bestscore1 = score;
                                    }
                                } while (legal_movesBB);
                                
                            } else {
                                //PASS
                                sBoard.board.do_pass();
                                bestscore1 = -sBoard.get_score<WITHOUT_FM>();
                                sBoard.board.do_pass();
                                
                            }
                            
                            iter->score = bestscore1;

                        } else if (depth == DEPTH_5) {
                            
                            iter->score = sBoard.get_score<WITHOUT_FM>();
                            
                        } else { //DEPTH_4
                            
                            iter->score = RXBitBoard::get_mobility(board.discs[board.player], board.discs[board.player^1]);
                        }
                        
                        
                        sBoard.undo_move(*iter);
                    }
                    
                } else {
                    sBoard.patterns_update(*iter);
                }
                
                
                if(bestmove == NOMOVE) {
                    
                    RXMove* move = list->pick_next_promising_move();
                    
                    sBoard.do_move(*move);
                    bestscore = -PVS_last_ply<UseFM>(threadID, sBoard, pv, depth-1, -upper, -lower, false);
                    sBoard.undo_move(*move);
                    
                    bestmove = move->position;
                    if(bestscore>lower)
                        lower = bestscore;
                    
                    // next move
                    list = list->next;
                }
                
                //other moves
                int score = UNDEF_SCORE;
                for(; lower < upper && list->next != nullptr; list = list->next) {
                    
                    RXMove* move = list->next;
                    if(move->next != nullptr)
                        move = list->pick_next_promising_move();

                    sBoard.do_move(*move);
                    
                    score = -PVS_last_ply<UseFM>(threadID, sBoard, false, depth-1, -lower-1, -lower, false); //change
                    if(lower < score && score < upper)
                        score = -PVS_last_ply<UseFM>(threadID, sBoard, pv, depth-1, -upper, -score, false);
                    
                    sBoard.undo_move(*move);
                    
                    if(score>bestscore) {
                        bestscore = score;
                        bestmove = move->position;
                        if(bestscore>lower)
                            lower = bestscore;
                    }
                }
            }
        }
    }
    
    if(bestscore == UNDEF_SCORE) {
        
        if(passed) {
            return sBoard.final_score();
        } else {
            board.do_pass();
            bestscore = -PVS_last_ply<UseFM>(threadID, sBoard, pv, depth-1, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    }
    
    if constexpr (UseFM)
        hTable_shallow->update(hash_code, board, depth, alpha, upper, bestscore, bestmove);
    
    return bestscore;
    
}



template <const bool UseFM>
int RXEngine::alphabeta_last_three_ply(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, int alpha, const int beta, const bool passed) {

    RXBitBoard& board = sBoard.board;
    const unsigned long long  hash_code = board.hashcode();
    
    unsigned int bestmove = NOMOVE;
    
    int upper = beta;
    int lower = alpha;
    
    
    //synchronized access
    RXHashValue entry;
    if (hTable_shallow->get(hash_code, board, entry)) {
        //if(entry.depth >= 3) { //always true
        
        if (upper > entry.upper) {
            upper = entry.upper;
            if (upper <= lower)
                return upper;
        }
        
        if (!pv && entry.lower >= upper) {
            return entry.lower;
        }

        //}
        
//        if(board.isValid_square(entry.move))
            bestmove = entry.move;
    }
    
    int bestscore = UNDEF_SCORE;
    
    if (bestmove != PASS) {
        
        RXMove& move = threads[threadID]._move[board.n_empty][1];
        
        //fisrt move
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, move);
            sBoard.patterns_update(move);
            
            sBoard.do_move(move);
            bestscore = -alphabeta_last_two_ply<UseFM>(threadID, sBoard, -upper, -lower, false);
            sBoard.undo_move(move);
            
            if(bestscore>lower)
                lower = bestscore;
            
        }
        
        if(lower<upper) {
            
            unsigned long long legal_movesBB = board.get_legal_moves();
            if(bestmove != NOMOVE)
                legal_movesBB ^= 0x1ULL<<bestmove;
                        
            int score;
            for(RXSquareList* empties = board.empties_list->next; lower < upper && legal_movesBB; empties = empties->next) {
                /* order JWC */
                const int pos = empties->position;
                const unsigned long long bit = 0x1ULL << pos;
                if(legal_movesBB & bit) {
                    legal_movesBB ^= bit;
                    board.generate_flips(pos, move);
                    sBoard.patterns_update(move);
                    
                    sBoard.do_move(move);
                    score = -alphabeta_last_two_ply<UseFM>(threadID, sBoard, -upper, -lower, false);
                    sBoard.undo_move(move);
                    
                    if(score>bestscore) {
                        bestscore = score;
                        bestmove = move.position;
                        if(bestscore>lower)
                            lower = bestscore;
                    }
                    
                }
            }
        }
        
    }
    
    if(bestscore == UNDEF_SCORE) {
        
        if(passed) {
            return sBoard.final_score();
        } else {
            board.do_pass();
            bestscore = -alphabeta_last_two_ply<UseFM>(threadID, sBoard, -upper, -lower, true);
            board.do_pass();
            bestmove = PASS;
        }
    }
    
    if constexpr (UseFM)
        hTable_shallow->update(hash_code, board, DEPTH_3, alpha, upper, bestscore, bestmove);
    
    return bestscore;
    
}


template <const bool UseFM>
int RXEngine::alphabeta_last_two_ply(const unsigned int threadID, RXBBPatterns& sBoard, int alpha, const int beta, const bool passed) {
    
    RXBitBoard& board = sBoard.board;
    int bestscore = UNDEF_SCORE;
    
    RXMove& move     = threads[threadID]._move[board.n_empty][1];
    RXMove& lastMove = threads[threadID]._move[board.n_empty - 1][1];

    // Factorisation du "last ply"
    auto last_ply = [&sBoard, &lastMove, &board](const int beta_inner, const bool passed) __attribute__((always_inline)) -> int {
        int bestscore_1 = UNDEF_SCORE;
        unsigned long long legal_movesBB_1 = board.get_legal_moves();

        if (legal_movesBB_1) {
            RXSquareList* empties_1 = board.empties_list;
            do { /* order JWC */
                empties_1 = empties_1->next;
                const int pos_1              = empties_1->position;
                const unsigned long long bit_1 = 0x1ULL << pos_1;

                if (legal_movesBB_1 & bit_1) {
                    legal_movesBB_1 ^= bit_1;
                    board.generate_flips(pos_1, lastMove);
                    sBoard.patterns_update(lastMove);
                    ++board.n_nodes;

                    int score = -sBoard.get_score<UseFM>(lastMove);
                    if (score > bestscore_1)
                        bestscore_1 = score;
                }
            } while (bestscore_1 < beta_inner && legal_movesBB_1);

        } else {
            // PASS
            if(passed) {
                return sBoard.final_score();
            } else {
                sBoard.board.do_pass();
                bestscore_1 = -sBoard.get_score<UseFM>();
                sBoard.board.do_pass();
            }
        }

        return bestscore_1;
    };

    // -------------------------------------------------------------------------

    unsigned long long legal_movesBB = board.get_legal_moves();
    if (legal_movesBB) {

        RXSquareList* empties = board.empties_list;
        do { /* order JWC */
            empties = empties->next;
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;

            if (legal_movesBB & bit) {
                legal_movesBB ^= bit;

                board.generate_flips(pos, move);
                sBoard.patterns_update(move);

                board.discs[board.player] |= (move.flipped | move.square);
                board.player ^= 1;
                board.discs[board.player] ^= move.flipped;
                --board.n_empty;
                ++board.n_nodes;
                empties->previous->next = empties->next;

                move.undo_pattern = sBoard.pattern;
                sBoard.pattern    = move.pattern;

                int score = -last_ply(-alpha, false);

                sBoard.pattern = move.undo_pattern;
                
                empties->previous->next = empties;
                ++board.n_empty;
                board.discs[board.player] |= move.flipped;
                board.player ^= 1;
                board.discs[board.player] ^= (move.flipped | move.square);

                if (score >= beta)
                    return score;

                if (score > bestscore) {
                    bestscore = score;
                    if (bestscore > alpha)
                        alpha = bestscore;
                }
            }
        } while (legal_movesBB);

    } else {    // PASS

        if (passed) {
            bestscore = sBoard.final_score();
        } else {
            board.do_pass();
            bestscore = -last_ply(-alpha, true);
            board.do_pass();
        }
    }

    return bestscore;
}

std::string RXEngine::variationPrincipal(RXBitBoard& board, int depthLine) const {
    return hTable->line2String(board, depthLine, type_hashtable);
}

std::string RXEngine::showPV(RXBitBoard& board, int depthLine) const {
    std::ostringstream buffer;
    
    // unsynchronized acces
    RXHashValue entry;
    if(hTable->get(board, type_hashtable, entry)) {
        
        int depth = static_cast<int>(entry.depth);
        if(depth>board.n_empty)
            depth = board.n_empty;
        
        buffer << " " << depth << "@" << CONFIDENCE[entry.selectivity];
        
        if(entry.selectivity != NO_SELECT)
            buffer << " ";
        
        buffer << "  " << RXMove::index_to_coord(entry.move);
        
        int score;
        if(entry.lower == entry.upper) {
            buffer << " == ";
            score = entry.lower;
        } else if(entry.lower == -MAX_SCORE) {
            buffer << " <= ";
            score = entry.upper;
        } else {
            buffer << " >= ";
            score = entry.lower;
        }
        
        buffer << std::fixed << std::showpos << std::setprecision(0) << (static_cast<float>(score));
        
        buffer << "  Pv : " << hTable->line2String(board, depthLine, type_hashtable);
    }
    
    return buffer.str();
    
}

std::string RXEngine::showHashmove(const RXBitBoard& board, RXHashValue& entry) const {
    
    std::ostringstream buffer;
    
    int depth = static_cast<int>(entry.depth);
    if(depth>board.n_empty)
        depth = board.n_empty;
    
    buffer << "[" << depth << "@" << CONFIDENCE[entry.selectivity];
    
    if(entry.selectivity != NO_SELECT)
        buffer << " ";
    
    buffer << "] " << RXMove::index_to_coord(entry.move);
    
    int score;
    if(entry.lower == entry.upper) {
        buffer << " == ";
        score = entry.lower;
    } else if(entry.lower == -MAX_SCORE) {
        buffer << " <= ";
        score = entry.upper;
    } else {
        buffer << " >= ";
        score = entry.lower;
    }
    
    buffer << std::fixed << std::showpos << std::setprecision(0) << (static_cast<float>(score));
    
    
    return buffer.str();
    
}

std::string RXEngine::showBestmove(const int depth, const int selectivity, const int alpha, const int beta, const int score, const unsigned int bestmove) const {
    
    std::ostringstream buffer;
    
    buffer << " " << depth << "@" << CONFIDENCE[selectivity];
    
    if(selectivity != NO_SELECT)
        buffer << " ";
    
    buffer << "  " << RXMove::index_to_coord(bestmove);
    
    if(score<=alpha)
        buffer << " <= ";
    else if (beta <= score)
        buffer << " >= ";
    else
        buffer << " == ";
    
    buffer << std::fixed << std::showpos << std::setprecision(0) << (static_cast<float>(score));
    
    
    return buffer.str();
    
}

std::string RXEngine::display(RXBitBoard& board, const int type, const int allowed_display, int score, const int time, const int time_level) {
    
    std::ostringstream buffer;
    std::locale loc(std::locale(),new My_punct);
    buffer.imbue(loc);
    
    
    //unsynchronized acces
    RXHashValue entry;
    if( hTable->get(board, type_hashtable, entry) && ((entry.depth >= board.n_empty && entry.selectivity >= allowed_display) || (entry.depth < board.n_empty && entry.depth >= allowed_display))) {
        
        int depth = (entry.depth >= board.n_empty? board.n_empty : entry.depth);
        
        buffer << (type == HASHTABLE? "[":" ");
        buffer <<std::fixed << std::setw(2) << depth ;
        if(entry.selectivity != NO_SELECT) {
            buffer << "@" << std::setw(2) << CONFIDENCE[entry.selectivity];
            
        } else {
            buffer << "   ";
        }
        
        buffer << (type == HASHTABLE? "]|":" |");
        
        //track bug HASH 24/01/2025
        if(entry.depth < board.n_empty) { //normalize midGame final_score()
            if(score >= (MAX_SCORE-1)-64)
                score -= (MAX_SCORE-1)-64;
            else if (score <= (-MAX_SCORE+1)+64)
                score -= (-MAX_SCORE+1)+64;
        }
        
        int _type = type;
        
        if(type == HASHTABLE || type == GGS_MSG) {
            if(entry.upper == entry.lower) {
                score = entry.upper;
                _type = EXACT;
                
                if(score >= 64){
                    score = 64;
                    _type = SUPERIOR;
                } else if(score <= -64){
                    score = -64;
                    _type = INFERIOR;
                }
                
            } else if(entry.upper == MAX_SCORE) {
                score = entry.lower;
                if(score >= 64)
                    score = 64;
                _type = SUPERIOR;
            } else {
                score = entry.upper;
                if(score <= -64)
                    score = -64;
                _type = INFERIOR;
            }
        }
        
        
        
        switch(_type) {
            case INFERIOR:
                buffer << "<=";
                break;
            case SUPERIOR:
                buffer << ">=";
                break;
            case INTERRUPT:
                buffer << " #";
                break;
            default:
                buffer << "  ";
                break;
        }
        
        buffer << std::showpos << std::setprecision(0) << std::setw(3) << static_cast<float>(score) << "  | ";
        
        buffer << std::noshowpos << variationPrincipal(board, 12) << "| ";
        
        if(type != GGS_MSG) {
            if(type == HASHTABLE) {
                buffer << "00:00:00.00 |                 |           |";
            } else {
                buffer << toHMS(time/1000.0) << " | ";
                
                buffer << std::noshowpos << std::setprecision(0) << std::setw(15) << board.n_nodes << " | ";
                
                unsigned long long speed = 0;
                if(time_level>0)
                    speed = board.n_nodes/time_level;
                if (board.n_nodes > 300000) {
                    buffer << std::setw(9) << (time_level == 0 ? ' ': speed) << " |";
                } else {
                    buffer << std::setw(9) << " N/A" << " |";
                }
            }
        }
        
    }
    
    return buffer.str();
    
}



void RXEngine::stop(std::string msg) {
    
    abort.store(true);
    
    
    //wait end main thread
    if(pthreadMain[0] != nullptr) {
        pthread_join(pthreadMain[0], nullptr);
        pthreadMain[0] = nullptr;
    }
    
    hash_code_search = 0;
        
}


void RXEngine::resume() {
    
    resume_flag.store(true);
    
    stop("resume");
    
    hTable_shallow->reset();
    
    
}



/* synchronized method */
void RXEngine::get_move(RXSearch& s) {
    
    pthread_mutex_lock(&mutex);
    
    resume_flag.store(false);
    
    dependentTime_start = get_system_time();
    dependent_time = s.dependent_time;
    
    
    
    RXBBPatterns& sBoard = s.sBoard;
    RXBitBoard& board = sBoard.board;
    
    activeThreads = std::min<unsigned int>(std::max<unsigned int>(1, s.nThreads), THREAD_MAX);
    
    time_remaining = s.tRemaining;
    
    
    if(board.hashcode() == hash_code_search) {
        // search continue
        new_search = false;
        
        determine_move_time(board);
        
        //interrupt search if time limit < probable time search
        
        *log << "                  time limit : " << time_limit() << std::endl;
        *log << "                  time next level : " << time_nextLevel << std::endl;
        *log << "                  time start level : " << (get_current_time() - time_startLevel) << std::endl;
        
        
        if(time_limit() < (time_nextLevel - (get_current_time() - time_startLevel))) {
            *log  << "                  interrupt search: likely timeout\n" <<  std::endl;
            abort.store(true);
        }
        
    } else {
        
        //new position : new search
        
        
        //kill current search
        stop("new position");
        
        new_search = s.search_on_opponent_time;
        
        //restart
        time_start = dependentTime_start;
        
        //copy
        search_sBoard = sBoard; //utility?
        
        
        search_client      = s.clientMode;
        search_alpha       = std::max(-MAX_SCORE, std::min( MAX_SCORE-1, s.alpha));
        search_beta        = std::min(+MAX_SCORE, std::max(-MAX_SCORE+1, s.beta));
        search_selectivity = std::max(0, std::min(NO_SELECT, s.selectivity));
        search_depth       = std::max(2, std::min(board.n_empty, s.depth));
        
        
        hTable = s.htable;
        main_PV = s.main_PV;
        expected_PV = s.expected_PV;
        
        if(hTable->is_shared()) {
            type_hashtable = RXHashTable::HASH_SHARED;
        } else if(board.player == BLACK) {
            type_hashtable = RXHashTable::HASH_BLACK;
        } else { //color white
            type_hashtable = RXHashTable::HASH_WHITE;
        }
        
        
        hTable_PV = main_PV;
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        
        if(!resume_flag.load() && pthread_create(pthreadMain, &attr, init_pthreadMain, (void*)(this)) != 0)
            std::cout << "Echec: Thread main engine" << std::endl;
        
        pthread_attr_destroy(&attr);
        
    }
    
    
    //wait end search
    if(pthreadMain[0] != nullptr) {
        pthread_join(pthreadMain[0], nullptr);
        pthreadMain[0] = nullptr;
    }
    
    //affectation answer
    s.bestMove = best_answer;
    s.bestMove.tElapsed = get_current_dependentTime()/1000.0;
    
#ifndef GENERATE_RAWDATA
    
    if(search_client != RXSearch::kIOStd && search_sBoard.board.n_empty > 19) {
        int speed = 0;
        if(time_search != 0)
            speed = static_cast<int>(s.bestMove.nodes/time_search) ; //*1000)
        
        std::ostringstream buffer;
        
        std::locale loc(std::locale(),new My_punct);
        buffer.imbue(loc);
        
        
        buffer    << showPV(search_sBoard.board, 6);
        manager->sendMsg(buffer.str());
        
        buffer.str("");
        buffer << "Ts : " << toHMS(time_search/1000.0) << "   Tm : " << toHMS(s.bestMove.tElapsed);
        manager->sendMsg(buffer.str());
        
        buffer.str("");
        buffer << "nodes : " << std::setw(14) << s.bestMove.nodes;
        manager->sendMsg(buffer.str());
        
        buffer.str("");
        buffer << "speed : " << std::setw(14);
        if(s.bestMove.nodes > 300000) {
            buffer << static_cast<int>(speed) << " kN/s";
        } else {
            buffer << "N/A";
            
        }
        manager->sendMsg(buffer.str());
        
    }
#endif
    
    *log << "I play " << RXMove::index_to_coord(s.bestMove.position) << std::endl;
    *log << "evaluation " << (s.bestMove.score) <<  std::endl;
    *log << "time " << toHMS(s.bestMove.tElapsed) <<  std::endl;
    
    hash_code_search = 0;
    
    //************************************************************************************************
    //                                  here one thread only
    //************************************************************************************************
    
    if(!resume_flag.load() && s.search_on_opponent_time == true && board.n_empty>18) {
        
 
        //************************************************************************************************
        //                                  thinking on opponent time
        //************************************************************************************************

        dependent_time = false;
        
        RXMove& move = threads[0]._move[board.n_empty][1];
        
        //play best move
        if(s.bestMove.position == PASS) {
            sBoard.board.do_pass();
        } else {
            
            sBoard.board.generate_flips(s.bestMove.position, move);
            sBoard.patterns_update(move);
            
            sBoard.do_move(move);
            
        }
        
        
        //copy
        search_sBoard = sBoard;
 

        if(!hTable->is_shared()) {
            
            // 17/07/2009
            //pas d'anticipation si le score n'est pas exacte
            
            activeThreads = std::min<unsigned int>(std::max<unsigned int>(1, THREAD_MAX/2), THREAD_MAX);

            //************************************************************************************************
            //                                  anticipation strategy
            //************************************************************************************************

            RXHashValue entry;
            if(hTable->get(search_sBoard.board, type_hashtable, entry) && entry.move != NOMOVE) {
                
                if(entry.upper == entry.lower) {
                    
                    if(entry.move == PASS) {
                        search_sBoard.board.do_pass();
                    } else {
                        RXMove& answer = threads[0]._move[board.n_empty][1];
                        sBoard.board.generate_flips(entry.move, answer);
                        sBoard.patterns_update(answer);
                        
                        sBoard.do_move(answer);
                        search_sBoard = sBoard;
                        sBoard.undo_move(answer);
                        
                    }
                }
                
            }
        }
        
        
        hash_code_search = search_sBoard.board.hashcode();
        
        hTable_PV = expected_PV;
        
        time_start = get_system_time();
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        
        if(!resume_flag.load() && pthread_create(pthreadMain, &attr, init_pthreadMain, (void*)(this)) != 0)
            std::cout << "Echec: Thread main engine" << std::endl;
        
        pthread_attr_destroy(&attr);
        
        
        
        if(s.bestMove.position == PASS) {
            sBoard.board.do_pass();
        } else {
            sBoard.undo_move(move);
        }
        
    }
    
    //***********************************************************************************************
    
    pthread_mutex_unlock(&mutex);
}


void* RXEngine::run() {
        
    time_search = get_system_time();
    
    abort.store(false);
    
    // Wake up sleeping threads:
    wake_sleeping_threads();
    
    
    
    *log    << "---------------------------------------------------------------------------------------------------\n"
    << search_sBoard
    << std::endl;
    
    
    RXBitBoard& board = search_sBoard.board;
    
    best_answer.nodes = 0;
    board.n_nodes = 0;
    time_nextLevel = 0;
    
    RXMove* list = threads[0]._move[board.n_empty];
    board.moves_producing(list);
    
    if(list->next == nullptr) {    //PASS
        
        best_answer.position = PASS;
        best_answer.score = 0;
        
    } else if((list->next)->next == nullptr) { //FORCED
        
        best_answer.position = list->next->position;
        best_answer.score = 0;
        
        
    } else {
        
        *log << " depth | score | principal variation                 | time        |       nodes (N) |      kN/s |" << std::endl;
        
        int depth = 2;
        int selectivity = EG_HIGH_SELECT;
        
        RXMove* list1 = list;
        //        list->next->score = 0;
        
        
        RXHashValue entry_PV;
        const unsigned long long hash_code = board.hashcode();
        if(expected_PV->get(hash_code, board, type_hashtable, entry_PV)) {
            RXHashValue entry;
            if(!hTable->get(hash_code, board, type_hashtable, entry)) {
                *log << "                  in expected_PV" << std::endl;
                hTable->copyPV(expected_PV, type_hashtable, board, type_hashtable);
            }
        }
        
        if(main_PV->get(hash_code, board, type_hashtable, entry_PV)) {
            RXHashValue entry;
            if(!hTable->get(hash_code, board, type_hashtable, entry)) {
                *log << "                  in main_PV" << std::endl;
                hTable->copyPV(main_PV, type_hashtable, board, type_hashtable);
            }
        }
        
        bool endgame_flag = false;
        
        
        RXHashValue entry;
        if(hTable->get(hash_code, board, type_hashtable, entry)) {
            
            best_answer.position = entry.move;
            list->sort_bestmove(entry.move);
            
            list1 = list->next;
            search_sBoard.patterns_update(*list1);
        
            *log << display(board, HASHTABLE) << std::endl;
            
            if(dependent_time && search_sBoard.board.n_empty> 19 && entry.depth>13)
                manager->sendMsg(showHashmove(board, entry));
            
            depth = std::max(2, +entry.depth);
            endgame_flag = (search_sBoard.board.n_empty - depth <= 0 ? true: false);
            
            if(endgame_flag) //endgame
                selectivity = std::min(NO_SELECT, +entry.selectivity);
            
            if(entry.upper == entry.lower) {
                
                list1->score = entry.upper;
                
                if(search_alpha<list1->score && list1->score<search_beta) {
                    if(endgame_flag) //endgame
                        selectivity = std::min(NO_SELECT, selectivity+1);
                    else
                        depth = std::min(search_sBoard.board.n_empty, depth+2);
                }
                
                
            } else {
                
                if(entry.upper == MAX_SCORE) {
                    list1->score = entry.lower;
                    
                    if(list1->score>=search_beta) {
                        if(endgame_flag) //endgame
                            selectivity = std::min(NO_SELECT, selectivity+1);
                        else
                            depth = std::min(search_sBoard.board.n_empty, depth+2);
                    }
                    
                } else if(entry.lower == -MAX_SCORE) {
                    list1->score = entry.upper;
                    
                    if(list1->score<=search_alpha) {
                        if(endgame_flag) //endgame
                            selectivity = std::min(NO_SELECT, selectivity+1);
                        else
                            depth = std::min(search_sBoard.board.n_empty, depth+2);
                    } else {
                        //reset search
                        //                        std::cout << "hash [-inf;score] reset search" << std::endl;
                        list1->score = 0;
                        depth = 2;
                        selectivity = EG_HIGH_SELECT;
                    }
                    
                } else {
                    list1->score = (entry.upper+entry.lower)/2;
                }
                
            }
            
            
        }
                
        sort_moves(0, endgame_flag, search_sBoard, depth, selectivity, -MAX_SCORE, MAX_SCORE, list1);
        
        //if no hashmove
        list1->next->score = 0;
        
        
        best_answer.position = list->next->position;
        best_answer.score = list->next->score;
        
        extra_time = 0;
        
        hTable_shallow->new_search(search_sBoard.board.n_empty);
        
        hTable_PV->new_search(search_sBoard.board.player, search_sBoard.board.n_empty);
        hTable->new_search(search_sBoard.board.player, search_sBoard.board.n_empty);
        
        hTable->protectPV(search_sBoard.board);
        
        
        int max_depth;
        if(search_depth <= search_sBoard.board.n_empty-(USE_PV_EXTENSION ? 10: 6)) {
            depth = std::min(depth, search_depth);
            max_depth = search_depth;
        } else {
            depth = std::min(depth, search_sBoard.board.n_empty);
            max_depth = search_sBoard.board.n_empty-(USE_PV_EXTENSION ? 10: 6);
        }
        
        //normalisation de depth
        if(depth%2 == (search_sBoard.board.n_empty%2 == 1 ? 0 : 1))
            ++depth;
        
        //normalisation de max_depth
        if(max_depth%2 == (search_sBoard.board.n_empty%2 == 1? 0:1))
            ++max_depth;
        
        
        
        
        if(depth<=max_depth) {
            
            int MG_selectivity = MG_SELECT;
            if(search_client == RXSearch::kIOStd && search_depth <= (search_sBoard.board.n_empty-(USE_PV_EXTENSION ? 10: 6))) {
                //only midgame search
                MG_selectivity = search_selectivity;
                depth = std::min(depth, search_depth);
                max_depth = search_depth;
            
            }
                 
            iterative_deepening(search_sBoard, list, MG_selectivity, depth, max_depth);
        }
        
        if (!abort.load() && search_depth > (search_sBoard.board.n_empty-(USE_PV_EXTENSION ? 10: 6))) {
            
            //coherence selectivty et end_selectivity
            int end_selectivity = search_depth < search_sBoard.board.n_empty? EG_HIGH_SELECT:search_selectivity;
                        
            EG_driver(search_sBoard, std::min(selectivity, end_selectivity), end_selectivity, list);
        }
                
    }
    
    
    //sleeping threads
    allThreadsShouldSleep = true;
    
    abort.store(true);
    
    time_search = get_system_time() - time_search;
    
    return nullptr;
    
}


//monothread
bool RXEngine::probable_timeout(double probable_time_next_level) const {
    
    if(dependent_time && extra_time == 0) {
        
        double tElapsed_dependent = get_current_dependentTime() ;
        double time_for_move = time_move;
        
        if ((tElapsed_dependent + probable_time_next_level) > 3*time_for_move)
            return true;
        
        if(tElapsed_dependent > (6*time_for_move/10)) //40%
            if ((tElapsed_dependent + probable_time_next_level) > 2*time_for_move/3)
                return true;
    }
    
    return false;
}

//monothread
int RXEngine::pTime_next_level(RXBitBoard& board, int time_level, int depth, int next_depth) const {
    
    int probable_Time_next_level = 0;
    
    //*log << "                  time level : " << time_level << std::endl;
    
    if(get_type_search() == MIDGAME) { //midgame
        
        if(next_depth > board.n_empty-6) {
            //*log << "                  depth " << depth << " to @60" << std::endl;
            
            //Start ENDGAME
            probable_Time_next_level = time_level * 9;
            
        } else if(depth>13 && time_level != 0) {
            //*log << "                  depth " << depth << " to " << next_depth << std::endl;
            
            
            double width = std::min(pow(board.n_nodes, 1.0/depth), 1.85); //std::max
            double newNodes = pow(width, next_depth);
            
            probable_Time_next_level = static_cast<int>(newNodes/(board.n_nodes/time_level));
            
        }
        
        
    } else {    //endgame
        
        //*log << "                  confidence " << CONFIDENCE[get_select_search()] << " to " << CONFIDENCE[std::min(NO_SELECT, get_select_search()+1)] << std::endl;
        
        switch(CONFIDENCE[std::min(NO_SELECT, get_select_search()+1)]) {
                
            case 72:
            case 84:
                probable_Time_next_level = time_level * 3;
                break;
                
            case 91:
            case 95:
                probable_Time_next_level = time_level * 5;
                break;
                
            case 98:
            case 99:
                probable_Time_next_level = time_level * 7;
                break;
                
            default:
                probable_Time_next_level = time_level * 11;
        }
        
        
        
    }
    
    //*log << "                  time next level : " << probable_Time_next_level << std::endl;
    
    return probable_Time_next_level;
}


//monothread
void RXEngine::determine_move_time(RXBitBoard& board) {
    /*
     // > 3 minutes
     static const int tSafety[] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  1,
     1,  2,  2,  5,  5,  9,  9, 13,
     13, 18, 18, 22, 22, 24, 24, 24,
     24, 24, 24, 24, 24, 24, 24, 24,
     24, 24, 24, 24, 24, 24, 24, 24,
     24, 24, 24, 24, 24, 24, 24, 24, 24 };
     */
    
    //1 minute
    static const int tSafety[] = {
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  1,
        1,  2,  2,  4,  4,  7,  7,  9,
        9, 11, 11, 13, 13, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15,
        15, 15, 15, 15, 15, 15, 15, 15, 15 };
    
    
    int time_Safety = 1000 * tSafety[board.n_empty];
    *log << "                  time safety : " << time_Safety << std::endl;
    
    
    int tElasped = get_current_dependentTime();
    
    int tRemaining = time_remaining - tElasped;
    
    *log << "                  time remaining : " << tRemaining << std::endl;
    
    
    int tMove; //time for move
    
    if(get_type_search() == MIDGAME) {
        
        int n_empty_before_solved = std::max(2, board.n_empty-(24+static_cast<int>(activeThreads)/4)); //M3 Pro solved at 24 empties // 1 minute 26 empties
        
        float n_remaining_moves = std::floor((n_empty_before_solved)/2.0);
        
        //Midgame mode
        tMove = static_cast<int>((1+(n_remaining_moves-1)/n_remaining_moves)*(tRemaining-time_Safety) / n_remaining_moves);
        
        *log << "                  MG time move : " << tMove << std::endl;
        
        if(new_search) {    // new research
            tMove = 3*tMove/2; //*1,50
            
            *log << "                  MG new search time move : " << tMove << std::endl;
            
        }
        
    } else {
        //EndGame mode
        int selectivity = get_select_search();
        double confidence = std::max(67, CONFIDENCE[selectivity])/100.0;
        tMove = static_cast<int>((tRemaining-time_Safety)*(confidence*confidence)*0.75);
        
        *log << "                  EG time move : " << tMove << std::endl;
        
        if(new_search) { // new research
            if(board.n_empty>36) {
                tMove = 4*tMove/3; //*1,33
            } else if(board.n_empty>32) {
                tMove = 7*tMove/4; //*1,75
            } else {
                tMove *= 2; //*2
            }
            
            *log << "                  EG new search time move : " << tMove << std::endl;
            
        }
        
        if(tMove < 10*1000) //min time
            tMove = std::max(tMove, time_remaining/2);
    }
    
    //MIDGAME and ENGDGAME
    if(!new_search && board.n_empty>34) {
        tMove = std::max(3*tMove/4, time_Safety/6); //*0,75
        *log << "                  search continue time move : " << tMove << std::endl;
        
    }
    
    
    //security
    tMove = std::min(90*(tRemaining-time_Safety)/100, tMove);
    tMove = std::max(1000, tMove);

    *log << "                  final time move : " << tMove << std::endl;
    
    
    int tExtra = tMove;
    if(get_type_search() == MIDGAME)
        tExtra = 2*tMove;
    
    tExtra = std::min(tExtra, tRemaining-(tMove+time_Safety));
    
    time_move = tMove + tElasped;
    extratime_move = tExtra;
    
    
    *log        << "\n"
    << "        tr: " << toHMS((tRemaining)/1000.0) << " "
    << "tm: " << toHMS(tMove/1000.0) << " "
    << "xt: " << toHMS(tExtra/1000.0) << "\n"
    << "        stop before       : " << toHMS((tRemaining - tMove)/1000.0) << "   : " << toHMS((tRemaining - (tMove+tExtra))/1000.0) << "\n"
    << std::endl;
    
}


void RXEngine::writeLog(std::string s) {
    *log << s << std::endl;
}


// init_threads() is called during startup.  It launches all helper threads,
// and initializes the split point stack and the global locks and condition
// objects.

void RXEngine::init_threads() {
        
    pthread_t pthread[1]; //pointeur
    
    
    for(unsigned int i = 0; i < THREAD_MAX; i++) {
        
        threads[i].activeSplitPoints = 0;
        threads[i].state = RXThread::INITIALIZING;
        
    }
    
    threads[0].state = RXThread::SEARCHING;
    
    // Launch the helper threads:
    // RXEngine idThread est passé en paramettre avec (void*)(this)
    for(unsigned int i = 1; i < THREAD_MAX; i++) {
        
        ThreadLaunchArgs* args = new ThreadLaunchArgs{this, i};
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        
        if(pthread_create(pthread, &attr, init_threadHelper, (void*)(args)) != 0) {
            std::cout << "Echec: Thread helper engine" << std::endl;
            delete args;
        }
        
        pthread_attr_destroy(&attr);
        
        // le thread lit son threadID depuis args avant de le delete,
        // puis passe à AVAILABLE — on attend ça :
        while(threads[i].state == RXThread::INITIALIZING)
            ;
    }
}


// stop_threads() is called when the program exits.  It makes all the
// helper threads exit cleanly.

void RXEngine::stop_threads() {
    
    activeThreads = THREAD_MAX;     // kill all threads
    
    //important: before wake up
    allThreadsShouldExit = true;
    
    //a etudier
    wake_sleeping_threads();
    
    for(unsigned int i = 1; i < THREAD_MAX; i++) {
        while(threads[i].state != RXThread::TERMINATED)
            ;
    }
    
}




void* RXEngine::idle_loop(unsigned int threadID, RXSplitPoint* waitSp) {
    
    
    while(true) {
        
        if(allThreadsShouldExit) {
            threads[threadID].state = RXThread::TERMINATED;
            break;
        }
        
        // If we are not thinking, wait for a condition to be signaled instead
        // of wasting CPU time polling for work:
        while (  allThreadsShouldSleep
               || threadID >= activeThreads
               || threads[threadID].state == RXThread::INITIALIZING
               || threads[threadID].state == RXThread::AVAILABLE ) {
            
            
            pthread_mutex_lock(&(threads[threadID].lock));
            
            if(threads[threadID].state == RXThread::INITIALIZING)
                threads[threadID].state = RXThread::AVAILABLE;
            
            
            if (allThreadsShouldExit || (waitSp && waitSp->n_Slaves == 0)) {
                pthread_mutex_unlock(&(threads[threadID].lock));
                break;
            }
            
            if(   allThreadsShouldSleep
               || threadID >= activeThreads
               || threads[threadID].state == RXThread::AVAILABLE) {
                
                pthread_cond_wait(&(threads[threadID].cond), &(threads[threadID].lock));
            }
            
            pthread_mutex_unlock(&(threads[threadID].lock));
        }
        
        
        
        if(threads[threadID].state == RXThread::WORKISWAITING) {
            
            pthread_mutex_lock(&(threads[threadID].lock));
            
            threads[threadID].state = RXThread::SEARCHING;
            RXSplitPoint* splitPoint = threads[threadID].splitPoint;
            
            pthread_mutex_unlock(&(threads[threadID].lock));
            
            
            //functions callback
            switch (splitPoint->CBSearch) {
                    
                case RXSplitPoint::MID_ROOT:
                    MG_SP_search_root(splitPoint, threadID);
                    break;
                    
                case RXSplitPoint::MID_PVS:
                    MG_SP_search_deep(splitPoint, threadID);
                    break;
                    
                case RXSplitPoint::MID_XPROBCUT:
                    MG_SP_search_XProbcut(splitPoint, threadID);
                    break;
                    
                case RXSplitPoint::END_ROOT:
                    EG_SP_search_root(splitPoint, threadID);
                    break;
                    
                case RXSplitPoint::END_PVS:
                    EG_SP_search_DEEP(splitPoint, threadID);
                    break;
                    
                case RXSplitPoint::END_XPROBCUT:
                    EG_SP_search_XEndcut(splitPoint, threadID);
                    break;
                                        
            }
            
            pthread_mutex_lock(&(threads[threadID].lock));
            threads[threadID].state = RXThread::AVAILABLE;
            pthread_mutex_unlock(&(threads[threadID].lock));
            
            // je dois proteger l'acces
            pthread_mutex_lock(&threads[splitPoint->master].lock);
            if (threadID != splitPoint->master && threads[splitPoint->master].state == RXThread::AVAILABLE) {
                pthread_cond_signal(&(threads[splitPoint->master].cond));
                //wake_sleeping_thread(splitPoint->master);
            }
            pthread_mutex_unlock(&threads[splitPoint->master].lock);
            
            
        }
        
        
        
        // If this thread is the master of a split point and all threads have
        // finished their work at this split point, return from the idle loop:
                
        //n_Slaves without mutex
        if (waitSp && waitSp->n_Slaves == 0) {
            
            pthread_mutex_lock(&(waitSp->lock));
            threads[threadID].state = RXThread::SEARCHING;
            pthread_mutex_unlock(&(waitSp->lock));
            
            break;
        }
        
        
    }
    
    return nullptr;
}

// wake_sleeping_threads() wakes up all sleeping threads (passive waiting) when it is time
// to start a new search from the root (active waiting).

void RXEngine::wake_sleeping_threads() {
    
    allThreadsShouldSleep = false;
    
    for (unsigned int i = 0; i<activeThreads; i++) {
        pthread_mutex_lock(&(threads[i].lock));
        pthread_cond_signal(&(threads[i].cond));
        
        //       wake_sleeping_thread(i);
        pthread_mutex_unlock(&(threads[i].lock));
    }
}


//void RXEngine::wake_sleeping_thread(unsigned int threadID) {
//
//
//    assert(allThreadsShouldSleep == false);
//
//    pthread_cond_signal(&(threads[threadID].cond));
//
//
//}


// thread_is_available() checks whether the thread with threadID "slave" is
// available to help the thread with threadID "master" at a split point.  An
// obvious requirement is that "slave" must be idle.  With more than two
// threads, this is not by itself sufficient:  If "slave" is the master of
// some active split point, it is only available as a slave to the other
// threads which are busy searching the split point at the top of "slave"'s
// split point stack (the "helpful master concept" in YBWC terminology).


// Si la methode est appeler directement depuis split()
// elle est synchronisée avec MP_Sync
// si la methode est appelé depuis idle_thread_exists()
// la methode est non synchronisée
bool RXEngine::thread_is_available(unsigned int slave, unsigned int master) {
    
    //    assert(slave >= 0 && slave < activeThreads);
    //    assert(master >= 0 && master < activeThreads);
    //    assert(activeThreads > 1);
    
    
    if(slave == master || threads[slave].state != RXThread::AVAILABLE) {
        return false;
    }
    
    //    // the "helpful master concept" in YBWC terminology
    //    // cas particulier 2 threads
    //    if(activeThreads == 2) {
    //        return true;
    //    }
    
    //copy local (argh... bug 25/01/2010)
    const unsigned int localActiveSplitPoints = threads[slave].activeSplitPoints;
    
#ifndef USE_IMPROVE_HELPFUL_MASTER_CONCEPT
    
    //Apply the "helpful master" concept if possible.
    if(localActiveSplitPoints == 0 || threads[slave].splitPointStack[localActiveSplitPoints-1].slaves[master])
        return true;
#else
    /*improve helpful master concept */
    
    
    if(localActiveSplitPoints == 0)
        return true;
    
    const RXSplitPoint& slave_activeSplitPoint = threads[slave].splitPointStack[localActiveSplitPoints-1];
    
    if(slave_activeSplitPoint.slaves[master])
        return true;
    
    //improve helpful master concept
    if(threads[master].splitPoint != nullptr) {
        RXSplitPoint* splitPoint = threads[master].splitPoint->parent;
        
        while (splitPoint != nullptr) {
            if(splitPoint->master == slave && &slave_activeSplitPoint == splitPoint) {
                return true;
            }
            splitPoint = splitPoint->parent;
        };
    }
    
#endif
    
    return false;
}

// split() does the actual work of distributing the work at a node between
// several threads.  If it does not succeed in splitting the node (because no
// idle threads are available, or because we have no unused split point
// objects), the function immediately returns false.  If splitting is
// possible, a SplitPoint object is initialized with all the data that must
// be copied to the helper threads (the current position and,
// alpha, beta, the search depth, etc.), and we tell our helper threads that
// they have been assigned work.  This will cause them to instantly leave
// their idle loops and call sp_search().  When all threads have returned
// from sp_search (or, equivalently, when splitPoint->n_slaves becomes 0),
// split() returns true.



bool RXEngine::split(RXBBPatterns& sBoard, bool pv, int pvDev,
                     int depth, int depth_reduction, int selectivity, int alpha, int beta, int& bestscore, unsigned int& bestmove,
                     RXMove* list, unsigned int master, RXSplitPoint::t_callBackSearch callback) {
    
    
    
    pthread_mutex_lock(&MP_sync);
    
    if (threads[master].activeSplitPoints >= ACTIVE_SPLITPOINT_MAX) {
        pthread_mutex_unlock(&MP_sync);
        return false;
    }
    
    // Pick the next available split point object from the split point stack:
    RXSplitPoint& splitPoint = threads[master].splitPointStack[threads[master].activeSplitPoints];
    splitPoint.n_Slaves = 1;
    
    
    pthread_mutex_unlock(&MP_sync);
    
    // add thread
    for(unsigned int i = 0; i < activeThreads && splitPoint.n_Slaves <= THREAD_PER_SPLITPOINT_MAX ; i++) {
        
        //first without mutex
        if(i != master && threads[i].state == RXThread::AVAILABLE) {
            
            pthread_mutex_lock(&MP_sync);
            
            //second control with mutex
            if(thread_is_available(i, master)) {
                
                
                pthread_mutex_lock(&(threads[i].lock));
                
                threads[i].state = RXThread::RESERVED;
                
                pthread_mutex_unlock(&(threads[i].lock));
                
                splitPoint.slaves[i] = true;
                splitPoint.n_Slaves++;
            }
            
            pthread_mutex_unlock(&MP_sync);
        }
        
    }
    
    //without mutex
    if(splitPoint.n_Slaves == 1) {
        return false;
    }
    
    pthread_mutex_lock(&MP_sync);
    
    threads[master].activeSplitPoints++;
    splitPoint.parent = threads[master].splitPoint;
    
    splitPoint.explored = false;
    
    pthread_mutex_unlock(&MP_sync);
    
    
    // without synchronisation
    // Initialize the split point object:
    
    splitPoint.sBoard = &sBoard; // pointer on sBoard
    
    //free scheduling rather than under mutex
    if (callback == RXSplitPoint::MID_PVS || callback == RXSplitPoint::END_PVS) //
        list->sort_by_score();
    
    splitPoint.list = list;
    
    splitPoint.depth = depth;
    splitPoint.depth_reduction = depth_reduction;

    splitPoint.pv = pv;
    splitPoint.pvDev = pvDev;
    splitPoint.selectivity = selectivity;
    splitPoint.alpha = alpha;
    splitPoint.beta = beta;
    
    splitPoint.bestscore = bestscore;
    splitPoint.bestmove = bestmove;
    
    splitPoint.CBSearch = callback;
    
    splitPoint.master = master;
    
    
    // Tell the threads that they have work to do.  This will make them leave
    // their idle loop.
    
    for(unsigned int i = 0; i < activeThreads; i++)
        if(i == master || splitPoint.slaves[i]) {
            
            pthread_mutex_lock(&threads[i].lock);
            
            threads[i].splitPoint = &splitPoint;
            threads[i].state = RXThread::WORKISWAITING;
            
            if (i != master) {
                pthread_cond_signal(&(threads[i].cond));
                //wake_sleeping_thread(i);
            }
            pthread_mutex_unlock(&threads[i].lock);
            
        }
    
    
    
    // Everything is set up.  The master thread enters the idle loop, from
    // which it will instantly launch a search, because its workIsWaiting
    // slot is 'true'.  We send the split point as a second parameter to the
    // idle loop, which means that the main thread will return from the idle
    // loop when all threads have finished their work at this split point
    // (i.e. when // splitPoint->n_Slaves == 0).
    idle_loop(master, &splitPoint);
    
    
    // We have returned from the idle loop, which means that all threads are
    // finished.  Update bestvalue, and return:
    
    //update return value
    bestscore = splitPoint.bestscore;
    bestmove = splitPoint.bestmove;
    
    
    pthread_mutex_lock(&MP_sync);
    
    threads[master].splitPoint = splitPoint.parent;
    threads[master].activeSplitPoints--;
    
    pthread_mutex_unlock(&MP_sync);
    
    
    return true;
}


#ifdef TUNE_PROBCUT_MID

void RXEngine::probcut_mid_data(RXHashTable* HT, RXHashTable* PV) {
    
    activeThreads = get_THREAD_MAX();
    
    hTable = HT;
    hTable_PV = PV;
    type_hashtable = RXHashTable::HASH_SHARED;
    
    //open ofstream
    std::ofstream ofs("probcut_mid.txt");
    
    RXBBPatterns sBoard;
    RXBitBoard& board = sBoard.board;
    
    unsigned int depth_max = 22;
    int scores[depth_max+1];

    for(int n_data = 0; n_data < 1000; ++n_data) {
        
        if(n_data % 100 == 0)
            std::cout << n_data << std::endl;
        
        //start at 8 discs
        for (int n_discs = 8; n_discs < 64-5-depth_max; ++n_discs){
            int n_moves = 0;
            for(; n_moves < n_discs-4 && board.n_moves()!=0 ; ++n_moves) {
                unsigned long long legal_movesBB = board.get_legal_moves();
                if(legal_movesBB) {
                    
                    unsigned long long legal_move = RXMove::random_pick_bit_in_legalmoves(legal_movesBB);
                    
                    RXMove* move = threads[0]._move[board.n_empty];
                    for(RXSquareList* empties = board.empties_list->next; empties->position != NOMOVE; empties = empties->next) {
                        if((legal_movesBB & 0x1ULL<<empties->position) & legal_move) {
                            
                            board.generate_flips(empties->position, *move);
                            sBoard.patterns_update(*move);
                            
                            break;
                        }
                    }
                    
                    sBoard.do_move(*move);
                }
                
            }
            
            
            if(board.n_moves()!=0) {
                                
                hTable->reset();
                
                for(int depth = board.n_empty & 1; depth <= depth_max; depth+=2){
                    
                    int score;
                    if(depth < DEPTH_4) {
                        score = MG_PVS_shallow(0, sBoard, true, depth, -MAX_SCORE, MAX_SCORE, false);
                    } else {
                        wake_sleeping_threads();
                        score = MG_PVS_deep(0, sBoard, true, NO_SELECT, depth, -MAX_SCORE, MAX_SCORE, false);
                    }
                    
                    scores[depth] = score;
                }
                    
                for(int depth = 2 + (board.n_empty & 1); depth <= depth_max; depth+=2){
                    for(int shallow_depth = depth & 1; shallow_depth <= depth-2; shallow_depth+=2){
                        
                        int diff_score = scores[depth] - scores[shallow_depth];
                        
                        //if(depth == DEPTH_2 && n_moves == 0)
                        //std::cout << n_data  << " :"  << n_discs << " " << shallow_depth << " " << depth << " " << diff_score << std::endl;
                        
                        if(-64 <= diff_score && diff_score <= 64)
                            ofs << n_discs << " " << shallow_depth << " " << depth << " " << diff_score << std::endl;
                    }
                }
                
            }
            
            // reset position
            for(; 0 < n_moves ; --n_moves) {
                RXMove* move = threads[0]._move[board.n_empty+1];
                sBoard.undo_move(*move);
            }
            
            
        }
    }
    
    ofs.close();
}

#endif

#ifdef TUNE_PROBCUT_END

void RXEngine::probcut_end_data(RXHashTable* HT, RXHashTable* PV) {
    
    activeThreads = get_THREAD_MAX();
    
    hTable = HT;
    hTable_PV = PV;
    type_hashtable = RXHashTable::HASH_SHARED;
    
    //open ofstream
    std::ofstream ofs("probcut_end.txt");
    
    RXBBPatterns sBoard;
    RXBitBoard& board = sBoard.board;

    for(int n_data = 0; n_data < 1000; ++n_data) {
        
        if(n_data % 100 == 0)
            std::cout << n_data << std::endl;

        for(int n_empty = 2; n_empty <= 27; ++n_empty) {
            int n_moves = 0;
            for (;n_moves < 64-4-depth && board.n_moves()!=0; ++n_moves){
                //sBoard.reset();
                unsigned long long legal_movesBB = board.get_legal_moves();
                if(legal_movesBB) {
                                        
                    unsigned long long legal_move = RXMove::random_pick_bit_in_legalmoves(legal_movesBB);
                                        
                    RXMove* move = threads[0]._move[board.n_empty];
                    for(RXSquareList* empties = board.empties_list->next; empties->position != NOMOVE; empties = empties->next) {
                        if((legal_movesBB & 0x1ULL<<empties->position) & legal_move) {
                              
                            board.generate_flips(empties->position, *move);
                            sBoard.patterns_update(*move);
                                                
                            break;
                        }
                    }
                                        
                    sBoard.do_move(*move);
                }

            }
            
            
            if(board.n_moves()!=0) {
                                
                int score_at_depth;
                
                //resolution de la position
                if (board.n_empty == 2) {
                    score_at_depth = board.final_score_2(-MAX_SCORE, MAX_SCORE);
                } else if (board.n_empty == 3) {
                    score_at_depth = board.final_score_3(-MAX_SCORE, MAX_SCORE);
                } else if (board.n_empty == 4) {
                    score_at_depth = board.final_score_4(-MAX_SCORE, MAX_SCORE);
                } else if (board.n_empty < EG_MEDIUM_TO_SHALLOW) {
                    score_at_depth = EG_alphabeta_parity(0, board, -MAX_SCORE, MAX_SCORE, false);
                } else if (board.n_empty < EG_MEDIUM_HI_TO_LOW) {
                    score_at_depth = EG_PVS_ETC_LTT(0, board, true, -MAX_SCORE, MAX_SCORE, false);
                } else  if (board.n_empty < EG_DEEP_TO_MEDIUM) {
                    score_at_depth = EG_PVS_ETC_mobility(0, sBoard, true, -MAX_SCORE, MAX_SCORE, false);
                } else {
                    wake_sleeping_threads();
                    score_at_depth = EG_PVS_deep(0, sBoard, true, NO_SELECT, -MAX_SCORE, MAX_SCORE, false);
                }
                
                hTable->reset();

                for(int shallow_depth = board.n_empty & 1; shallow_depth <= board.n_empty-2; shallow_depth+=2){
                    
                    int score_at_shallow_depth;
                    
                    if(shallow_depth < DEPTH_4) {
                        score_at_shallow_depth = MG_PVS_shallow(0, sBoard, true, shallow_depth, -MAX_SCORE, MAX_SCORE, false);
                    } else {
                        wake_sleeping_threads();
                        score_at_shallow_depth = MG_PVS_deep(0, sBoard, true, NO_SELECT, shallow_depth, -MAX_SCORE, MAX_SCORE, false);
                    }
                    
                    int diff_score_depth_score_shallow = (score_at_depth - score_at_shallow_depth);
                    
                    //std::cout << n_data  << " :"  << 64-board.n_empty << " " << shallow_depth << " " << board.n_empty << " " << diff_score_depth_score_shallow << std::endl;
                    
                    if(-64 <= diff_score_depth_score_shallow && diff_score_depth_score_shallow <= 64)
                        ofs << 64-board.n_empty<< " " << shallow_depth << " " << diff_score_depth_score_shallow << std::endl;
                }

            }
            
            for(; 0 < n_moves ; --n_moves) {
                RXMove* move = threads[0]._move[board.n_empty+1];
                sBoard.undo_move(*move);
            }
            
            
        }
    }
    ofs.close();

}

#endif

#ifdef TUNE_PROBCUT_END2

void RXEngine::probcut_end2_data(const std::string& file_name, RXHashTable* HT, RXHashTable* PV) {
        
    activeThreads = get_THREAD_MAX();
    
    hTable = HT;
    hTable_PV = PV;
    type_hashtable = RXHashTable::HASH_SHARED;
    
    //open ofstream
    std::ofstream ofs("probcut_end_ext.txt");
    
    RXBBPatterns sBoard;
    RXBitBoard& board = sBoard.board;

    std::ifstream in(file_name.c_str());
    if(in) {
        
        int n_data = 0;
        std::string line;
        
        while(!resume_flag.load() && std::getline(in, line)) {
            
            ++n_data;
            
            if(n_data % 100 == 0)
                std::cout << n_data << std::endl;

            std::stringstream ss;
            int score_at_depth = UNDEF_SCORE;
            
            ss << line.substr(line.find(":")+1);
            
            ss >> score_at_depth;

            sBoard.build(line);
            int n_empties = board.n_empty;
            
            hTable->reset();
                                        
            for(int shallow_depth = n_empties & 1; shallow_depth <= 17; shallow_depth+=2){
                
                int score_at_shallow_depth;
                
                if(shallow_depth < DEPTH_4) {
                    score_at_shallow_depth = MG_PVS_shallow(0, sBoard, true, shallow_depth, -MAX_SCORE, MAX_SCORE, false);
                } else {
                    wake_sleeping_threads();
                    score_at_shallow_depth = MG_PVS_deep(0, sBoard, true, NO_SELECT, shallow_depth, -MAX_SCORE, MAX_SCORE, false);
                }
                
                int diff_score_depth_score_shallow = (score_at_depth - score_at_shallow_depth);
                
                //std::cout << n_data  << " :"  << 64-n_empties << " " << shallow_depth << " " << n_empties << " " << diff_score_depth_score_shallow << std::endl;
                
                if(-64 <= diff_score_depth_score_shallow && diff_score_depth_score_shallow <= 64)
                    ofs << 64-n_empties<< " " << shallow_depth << " " << diff_score_depth_score_shallow << std::endl;
            }
            
            
        }
    }
    
    in.close();
    ofs.close();

}

#endif


