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


#include "RXEngine.hpp"
#include "RXRoxane.hpp"


const int RXEngine::MIN_DEPTH_SPLITPOINT = 7;

const int RXEngine::MG_DEEP_TO_SHALLOW = 4;
const int RXEngine::MG_MOVING_WINDOW = 4; //4

#ifdef USE_LMR
    const int RXEngine::LMR_MIN_DEPTH = 8; //standard 10
    const int RXEngine::LMR_DEEP_DEPTH = 16; //standard 20
#endif



// 21/06/2025 : desabled PV EXTENSION
// with vs without : 503 games s8r14 2:00
// W        D       L
// 85       293     125
// 16,9%    58,25%  24,85%
#ifdef PV_EXTENSION
const bool RXEngine::USE_PV_EXTENSION = true;
#else
const bool RXEngine::USE_PV_EXTENSION = false;
#endif
const int RXEngine::PV_EXTENSION_DEPTH = 14;
const int RXEngine::MIN_DEPTH_USE_PV_EXTENSION = 16;


void RXEngine::iterative_deepening(RXBBPatterns& sBoard, RXMove* list, int selectivity, int depth, const int max_depth) {
    
    set_type_search(MIDGAME);
    
    if(dependent_time)
        determine_move_time(sBoard.board);
    
    int eTime = get_current_time();
    
    do {
        
        time_startLevel= eTime;
        
        sBoard.board.n_nodes = 0;
        
        use_pv_ext = false;
        
        if(USE_PV_EXTENSION && depth >= MIN_DEPTH_USE_PV_EXTENSION) {
            
            
            depth_pv_extension = PV_EXTENSION_DEPTH + (depth & 1);
            
            if(abs(list->next->score) > 18)
                depth_pv_extension -= 2;
            
            if(sBoard.board.n_empty-depth <= depth_pv_extension) {
                use_pv_ext = true;
                *log << "                  Use Pv Extension" << std::endl;
            }
            
            
        }
        
        
        aspiration_search(sBoard, selectivity, depth, list);
        
        eTime = get_current_time();
        
        RXHashValue entry;
        if(hTable->get(sBoard.board, type_hashtable, entry) && entry.depth>=depth) {
            int type = EXACT;
            
            int score = list->next->score;
            
            if(entry.lower == entry.upper) {
                if(abort.load(std::memory_order_relaxed))
                    type = INTERRUPT;
                
                if(score >= 64){
                    score = 64;
                    type = SUPERIOR;
                } else if (score <= -64) {
                    score = -64;
                    type = INFERIOR;
                }
                
            } else if(list->next->score == entry.lower || entry.upper == MAX_SCORE) {
                type = SUPERIOR;
                score = entry.lower;
            } else if(list->next->score == entry.upper || entry.lower == -MAX_SCORE) {
                type = INFERIOR;
                score = entry.upper;
            }
            
            *log << display(sBoard.board, type, depth, score, eTime, eTime - time_startLevel) << std::endl;
        }
        
        best_answer.position = list->next->position;
        best_answer.score = list->next->score;
        best_answer.depth = depth;
        best_answer.selectivity = CONFIDENCE[selectivity];
        
        best_answer.nodes += sBoard.board.n_nodes;
        
        if(abort.load(std::memory_order_relaxed) )
            break;
        
        //update probable time for next depth
        
        time_nextLevel = pTime_next_level(sBoard.board, eTime - time_startLevel, depth, depth+2);
        
        if(depth>6)
            if(probable_timeout(time_nextLevel)) {
                abort.store(true);
                *log << "                  MG driver : likely timeout" << std::endl;
                
                break;
            }
        
        //stop conditions EndGame
        if(std::abs(list->next->score) >= ((MAX_SCORE-1) - 64)) {
            break;
        }
        
        
        if(std::abs(list->next->score)>48 && depth>=18 && sBoard.board.n_empty<38)
            break;
        
        
    } while((depth +=((search_client == RXSearch::kIOStd && !dependent_time) ? 1:2)) <= max_depth );
    
}


void RXEngine::aspiration_search(RXBBPatterns& sBoard, const int selectivity, const int depth, RXMove* list) {
    
    const int s_alpha = (search_alpha <= -64? -MAX_SCORE: search_alpha);
    const int s_beta  = (search_beta  >=  64?  MAX_SCORE: search_beta);
    
    int alpha = std::max(s_alpha, std::min(s_beta-1, list->next->score - MG_MOVING_WINDOW));
    int beta  = std::min(s_beta, std::max(s_alpha+1, list->next->score + MG_MOVING_WINDOW));
    
    
    MG_PVS_root(sBoard, selectivity, depth, alpha, beta, list);
    
    int left = 2;
    int right = 2;
    
    while (!abort.load(std::memory_order_relaxed)  && !(alpha < list->next->score && list->next->score < beta)) {
        
        //        *log << "                  MG research : [" << alpha << " < " << list->next->score << " < " << beta << "]" <<std::endl;
        
        
        if (list->next->score <= alpha) {
            
            if(alpha <= s_alpha)
                break;
            
            alpha = list->next->score - (MG_MOVING_WINDOW*left);
            left *=2;
            
        } else if (list->next->score >= beta) {
            
            if(beta >= s_beta)
                break;
            
            beta  = list->next->score + (MG_MOVING_WINDOW*right);
            right *=2;
        }
        
        
        alpha = std::max(s_alpha, std::min(s_beta-1, alpha));
        beta  = std::min(s_beta, std::max(s_alpha+1, beta ));
        
        MG_PVS_root(sBoard, selectivity, depth, alpha, beta, list);
        
    };
    
}


// avec la selectivite et de l'extension de recherche, on ne peut faire totalement confiance a
// un coup etudie en nullptr window (pas d'extension de recherche)
// c.a.d. on ne profite pas de la recherche NWS pour trouver un meilleur coup sauf si score>beta.
//
// root

void RXEngine::MG_PVS_root(RXBBPatterns& sBoard, const int selectivity, const int depth,  const int alpha, const int beta, RXMove* list) {
        
    //int selectivity = MG_SELECT;
    
    RXMove* iter = list->next;
    unsigned int bestmove = iter->position;
    
    int lower = alpha;
    const int upper = beta;
    
    //first move
    first_move.store(true);
    
    sBoard.do_move(*iter);
    
    int bestscore = -MG_PVS_deep(0, sBoard, true, selectivity, depth-1, -upper, -lower, false);
    
    sBoard.undo_move(*iter);
    
    if(!abort.load(std::memory_order_relaxed)  && std::abs(bestscore) != INTERRUPT_SEARCH) {
        
        
        if(search_client == RXSearch::kGGSMode) {	// GGS mode
            if(dependent_time && depth>13) {
                manager->sendMsg(showBestmove(depth, selectivity, lower, upper, bestscore, bestmove));
            }
        }
        
        if(bestscore > lower)
            lower = bestscore;
        else {
            extra_time = 1;
            //            *log << "                  [extra time <=:" << extra_time << "]" << std::endl;
        }
        
        
        //other moves
        first_move.store(false);
        int score;
        for(iter = iter->next; !abort.load(std::memory_order_relaxed)  && lower<upper && iter != nullptr; iter = iter->next) {
            
            
#ifdef USE_SPLIT_AT_ROOT
            
            // Split?
            if(activeThreads > 1 && iter->next != nullptr && depth>(MIN_DEPTH_SPLITPOINT+3) && !abort.load(std::memory_order_relaxed)
               && !thread_should_stop(0) && idle_thread_exists(0)
               && split(sBoard, true, 0, depth, selectivity, lower, upper, bestscore, bestmove, iter, 0, RXSplitPoint::MID_ROOT)) {
                
                break;
            }
#endif
            
            sBoard.do_move(*iter);
            
            //simple_PV pv == false ???
            score = -MG_PVS_deep(0, sBoard, false, selectivity, depth-1, -lower-1, -lower, false); //change
            
            
            if(!abort.load(std::memory_order_relaxed)  && lower < score && score < upper) {
                
                if(search_client == RXSearch::kGGSMode) {	// GGS mode
                    if(dependent_time && depth>13)
                        manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " maybe better? ");
                }
                
                
                ++extra_time;
                //                *log << "                  [extra time >:" << extra_time << "]" << std::endl;
                score = -MG_PVS_deep(0, sBoard, true, selectivity, depth-1, -upper, -lower , false);
                --extra_time;
                //                *log << "                  [extra time end :" << extra_time << "]" << std::endl;
                
                if(search_client == RXSearch::kGGSMode && !abort.load(std::memory_order_relaxed)) {    // GGS mode
                    if(dependent_time && depth>13 && score <= bestscore)
                        manager->sendMsg("         " + RXMove::index_to_coord(iter->position) + " refuted ");
                }
                
                
            }
            
            
            
            sBoard.undo_move(*iter);
            
            if (!abort.load(std::memory_order_relaxed) && std::abs(score) != INTERRUPT_SEARCH) {
                
                if(score > bestscore) {
                    
                    extra_time = 0;
                    
                    bestmove = iter->position;
                    bestscore = score;
                    
                    if(search_client == RXSearch::kGGSMode) {	// GGS mode
                        if(dependent_time && depth>13) {
                            manager->sendMsg(showBestmove(depth, selectivity, lower, upper, bestscore, bestmove));
                        }
                    }
                    
                    if (bestscore > lower)
                        lower = bestscore;
                    
                }
            }
            
        }
        
        extra_time = 0;
        
        
        //move to front bestmove
        list->sort_bestmove(bestmove);
        list->next->score = bestscore;
        
        
        hTable->update(sBoard.board.hashcode(), sBoard.board, type_hashtable, selectivity, depth, alpha, upper, bestscore, bestmove);
        
        
    }
    
}

void RXEngine::MG_SP_search_root(RXSplitPoint* sp, const unsigned int threadID) {
    
    //    assert(threadID >= 0 && threadID < activeThreads);
    //    assert(activeThreads > 1);
    
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    
    //here sp->beta is const
    while(sp->alpha < sp->beta && !abort.load(std::memory_order_relaxed) && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if (sp->list->next == nullptr) break;  // unlock auto via destructeur, même sur le break
            move = sp->list->next;
            sp->list = move;
        }

        const int alpha = sp->alpha; //local copy
        int depth = sp->depth;
        
        sBoard.do_move(*move);
        
        
        int score = -MG_PVS_deep(threadID, sBoard, false, sp->selectivity, depth-1, -alpha-1, -alpha, false); //change
        
        if (!(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID)) && alpha < score && score < sp->beta) {
            
            ++extra_time;
            
            if(dependent_time && depth>13)
                manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " maybe better? ");
            
            score = -MG_PVS_deep(threadID, sBoard, true, sp->selectivity, depth-1, -sp->beta, -alpha, false);
            
            
            if(search_client == RXSearch::kGGSMode && !(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))) {    // GGS mode
                if(dependent_time && depth>13 && score <= sp->bestscore)
                    manager->sendMsg("         " + RXMove::index_to_coord(move->position) + " refuted ");
            }
            
            
            --extra_time; //atomic = thread-safe
            
        }
        
        sBoard.undo_move(*move);
        
//        if(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))
//            break;
        
        //first without mutex
        if(score > sp->bestscore) {
            
            //update
            std::lock_guard<std::mutex> lk(sp->lock);

            if(sp->explored == false) {
                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    
                    if(dependent_time && depth>13)
                        manager->sendMsg(showBestmove(depth, sp->selectivity, sp->alpha, sp->beta, sp->bestscore, sp->bestmove));
                    
                    if(score > sp->alpha) {
                        
                        if(score >= sp->beta) {
                            sp->explored = true;
                        } else {
                            sp->alpha = score;
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


int RXEngine::MG_PVS_deep(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int selectivity, const int depth, const int alpha, const int beta, const bool passed) {
        
    if(depth <= MG_DEEP_TO_SHALLOW)
       return MG_PVS_shallow(threadID, sBoard, pv, depth, alpha, beta, passed);

    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    //time gestion
    if(threadID == 0 && dependent_time && get_current_dependentTime() > time_limit()) {
        abort.store(true);
        return INTERRUPT_SEARCH;
    }

    RXBitBoard& board = sBoard.board;
    
    unsigned int bestmove = NOMOVE;
    int lower = alpha;
    int upper = beta;
    
#ifdef PV_EXTENSION
    //PV EXTENSION
    if (pv && use_pv_ext && board.n_empty <= depth_pv_extension) {
        
        if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
            return EG_PVS_ETC_LTT(threadID, board, true, lower, upper, passed);
        
        return EG_PVS_ETC_mobility(threadID, sBoard, true, lower, upper, passed);
        
    }
#endif
    
    //synchronized acces
    RXHashValue entry;
    const unsigned long long hash_code = board.hashcode();
    if(hTable->get(hash_code, board, type_hashtable, entry)) {
        
        if(entry.selectivity >= selectivity && entry.depth >= depth) {
            
            if(entry.lower > lower) {
                
                lower = entry.lower;
                if(lower >= upper)
                    return lower;
                
            }
            
            if(!pv && entry.upper <= lower) {
                return  entry.upper;
            }
            
        }
        
        bestmove = entry.move;
    }
    
    // IID
    if(pv && bestmove == NOMOVE && depth > 8) {
        
        MG_PVS_deep(threadID, sBoard, pv, selectivity, depth - 2, -MAX_SCORE, MAX_SCORE, passed); //lower, upper,
        
        if(abort.load(std::memory_order_relaxed) || thread_should_stop(threadID))
            return INTERRUPT_SEARCH;
        
        if(hTable->get(hash_code, board, type_hashtable, entry)){
            bestmove = entry.move;
        }

    }
    
    
    RXMove* list = threads[threadID]._move[board.n_empty];
    list->next = nullptr;
    
    if(bestmove != PASS) {
        
        RXMove* move = list + 1;
        RXMove* previous = list;

        const unsigned long long current_P = board.discs[board.player];
        const unsigned long long current_O = board.discs[board.player^1];
        
        const int etc_depth = depth-1;

        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);


            //synchronized acces
#ifdef USE_ETC
            if(!pv) {
                ++board.n_nodes;

                const unsigned long long next_P = current_O ^ move->flipped;
                const unsigned long long next_O = current_P | (move->square | move->flipped);
                const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);

                if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth>=etc_depth) {
                    
                    if(-entry.upper >= upper) {
//                        hTable->update(hash_code, board, type_hashtable, selectivity, depth, alpha, upper, -entry.upper, bestmove);
                        return -entry.upper ;
                    }
                }
            }
#endif
            
            previous = previous->next = move++;
        }
        
        //for all empty square
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(bestmove != NOMOVE)
            legal_movesBB ^= 0x1ULL<<bestmove;
        
        for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
            const int pos = empties->position;
            const unsigned long long bit = 0x1ULL << pos;

            if(legal_movesBB & bit) {
                legal_movesBB ^= bit;
                
                board.generate_flips(pos, *move);
                
                move->score = 0;
                
                //synchronized acces
#ifdef USE_ETC
                
                if(!pv) {
                    ++board.n_nodes;

                    const unsigned long long next_P = current_O ^ move->flipped;
                    const unsigned long long next_O = current_P | (move->square | move->flipped);
                    const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);

                    if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity >= selectivity && entry.depth>=etc_depth) {
                        
                        if(-entry.upper >= upper) {
//                            hTable->update(   hash_code, board, type_hashtable, selectivity, depth, alpha, upper, -entry.upper, pos);
                            return -entry.upper ;
                        }
                        
                        move->score = -3;
                        
                        if(-entry.lower<=lower)
                            move->score = 2;
                        
                    }
                }
                
#endif
                
                previous = previous->next = move++;
                
            }
        }
        
        previous->next = nullptr;
        
    }
    
    int bestscore = UNDEF_SCORE;
    
    if (list->next == nullptr) {
        
        if (passed) {
           return sBoard.final_score();
        } else {
            board.do_pass();
            
            bestscore = -MG_PVS_deep(threadID, sBoard, pv, selectivity, depth-1, -upper, -lower, true);
            
            board.do_pass();
            bestmove = PASS;
            
        }
        
    } else {
        
        
        
        if(bestmove != NOMOVE) {
            
            /* first move */
            list = list->next;
            
            sBoard.patterns_update(*list);
            
            sBoard.do_move(*list);
            
            bestscore = -MG_PVS_deep(threadID, sBoard, pv, selectivity, depth-1, -upper, -lower, false);
            
            sBoard.undo_move(*list);
                        
            bestmove = list->position;
            
            if(bestscore > lower)
                lower = bestscore;
                        
        }
        
        if(lower < upper) {
            
            if(list->next != nullptr) {
                
                if((list->next)->next != nullptr) {
                    // Evaluate moves for a future sort
                    
                    if(depth>=10) {
                        
                        int lower_probcut = -MAX_SCORE;
                        int upper_probcut =  MAX_SCORE;
                        probcut_bounds(board, 3, depth, (4 + depth/4 + (depth & 1)), 0, lower, upper, lower_probcut, upper_probcut); //selectivity 3 = 91%
                        
 
                        if(lower_probcut<= sBoard.get_score<WITHOUT_FM>()) { // && eval_position<=(beta+upper_probcut*4)) { //alpha 95% / beta 99%
                            
                            //stable position -> sorting on evaluation
                            
                            for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                                
                                sBoard.patterns_update(*iter);
                                
                                //stable move -> sorting on evaluation
                                sBoard.do_move(*iter);
                                
                                if(depth >= 26) {
                                    
                                    if((board.n_empty & 1) == 0)
                                        iter->score += PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_6, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    else
                                        iter->score += PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_5, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    
                                } else if(depth >= 20) {
                                    
                                    if((board.n_empty & 1) == 0)
                                        iter->score += PVS_last_ply<WITHOUT_FM>(threadID, sBoard, true, DEPTH_4, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    else
                                        iter->score += alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, true, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    
                                } else if(depth >= 14) {
                                    
                                    if((board.n_empty & 1) == 0)
                                        iter->score += alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    else
                                        iter->score += alphabeta_last_three_ply<WITHOUT_FM>(threadID, sBoard, true, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    
                                } else  if((board.n_empty & 1) == 0) {
                                    
                                    iter->score += alphabeta_last_two_ply<WITHOUT_FM>(threadID, sBoard, -upper_probcut , -lower_probcut, false); //without FM for sort
                                    
                                } else {
                                    
                                    int bestscore1 = UNDEF_SCORE; //masquage
                                    
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
                                                sBoard.patterns_update(lastMove); //without FM for sort
                                                ++board.n_nodes;
                                                
                                                int score= -sBoard.get_score<WITHOUT_FM>(lastMove);
                                                if (score>bestscore1) {
                                                    bestscore1 = score;
                                                }
                                                
                                            }
                                        } while(bestscore1<-lower_probcut && legal_movesBB);
                                        
                                    } else  {
                                        //PASS
                                        sBoard.board.do_pass();
                                        iter->score -= sBoard.get_score<WITHOUT_FM>();
                                        sBoard.board.do_pass();
                                        
                                    }
                                    iter->score += bestscore1;
                                }
                                
                                sBoard.undo_move(*iter);
                                
                                
                            }
                            
                        } else {
                            
                            for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                                ++board.n_nodes;
                                
                                sBoard.patterns_update(*iter);
                                iter->score += sBoard.get_score<WITHOUT_FM>(*iter); //without FM for sort
                            }
                        }
                        
                    } else {
                        
                        for(RXMove* iter = list->next; iter!=nullptr; iter = iter->next) {
                            ++board.n_nodes;
                            
                            sBoard.patterns_update(*iter);
                            iter->score += sBoard.get_score<WITHOUT_FM>(*iter); //without FM for sort
                        }
                        
                    }
                    
                } else {
                    sBoard.patterns_update(*(list->next));
                }
            }
            
            if(bestscore == UNDEF_SCORE) { //first move
                
                RXMove* move = list->pick_next_promising_move();

                bestmove = move->position;
                
                sBoard.do_move(*move);
                bestscore = -MG_PVS_deep(threadID, sBoard, pv, selectivity, depth-1, -upper, -lower, false);
                sBoard.undo_move(*move);
                
                
                if(bestscore>lower)
                    lower = bestscore;
                                
                // next move
                list = list->next;
            }
            
            int score;
            for(;lower < upper && list->next != nullptr; list = list->next) {
                

                RXMove* move = list->next;

                if(move->next != nullptr) {	//more than 1 move
                    
                    // Split?
                    if(activeThreads > 1 && depth>MIN_DEPTH_SPLITPOINT && !abort.load(std::memory_order_relaxed)
                       && !thread_should_stop(threadID) && idle_thread_exists(threadID)
                       && split(sBoard, pv, 1, depth, selectivity, lower, upper, bestscore, bestmove, list, threadID, RXSplitPoint::MID_PVS)) {
                        
                        break;
                    }
                     
                    move = list->pick_next_promising_move();

                }
                
                
                sBoard.do_move(*move);
                
                    
                if(selectivity != NO_SELECT) {
                    score = -MG_NWS_XProbCut(threadID, sBoard, 1, selectivity, depth-1, -lower-1, false);
                    
                    if(lower < score && score < upper) {
                        if(score == lower+1) // Enhanced Forward Pruning.
                            score = lower;
                        score = -MG_PVS_deep(threadID, sBoard, pv, selectivity, depth-1, -upper, -score, false);
                    }
                    
                } else {
                    score = -MG_PVS_deep(threadID, sBoard, 0, selectivity, depth-1, -lower-1, -lower, false);

                    if(lower < score && score < upper)
                        score = -MG_PVS_deep(threadID, sBoard, pv, selectivity, depth-1, -upper, -score, false);
                    
                }
                    
                
                sBoard.undo_move(*move);
                
                if (score>bestscore) {
                    bestmove = move->position;
                    bestscore = score;
                    if (bestscore>lower)
                        lower = bestscore;
                }
                
                
            }
        }
        
    }
    
    
    //interrupt search
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
        
    hTable->update(   hash_code, board, type_hashtable, selectivity, depth, alpha, upper,  bestscore, bestmove);
    hTable_PV->update(hash_code, board, type_hashtable, selectivity, depth, alpha, upper,  bestscore, bestmove);
    
    return bestscore;
    
}

// MG_SP_search_deep() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_PVS_deep() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in MG_SP_search_deep().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::MG_SP_search_deep(RXSplitPoint* sp, const unsigned int threadID) {
    
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard); //operator=
    RXBitBoard& board = sBoard.board;
    
    //here sp->beta is const
    while(sp->alpha < sp->beta && !abort.load(std::memory_order_relaxed)  && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if (sp->list->next == nullptr) break;  // unlock auto via destructeur, même sur le break
            move = sp->list->next;
            sp->list = move;
        }

        int score;
        const int alpha = sp->alpha; //local copy
        
        sBoard.do_move(*move);
        
        if(sp->selectivity != NO_SELECT) {
            
            score = -MG_NWS_XProbCut(threadID, sBoard, sp->pvDev, sp->selectivity, sp->depth-1, -alpha-1, false);
            
            if(alpha < score && score < sp->beta)
                score = -MG_PVS_deep(threadID, sBoard, sp->pv, sp->selectivity, sp->depth-1, -sp->beta, -sp->alpha, false);

        } else {
            
            score = -MG_PVS_deep(threadID, sBoard, false, sp->selectivity, sp->depth-1, -alpha-1, -alpha, false);
            
            if(alpha < score && score < sp->beta)
                score = -MG_PVS_deep(threadID, sBoard, sp->pv, sp->selectivity, sp->depth-1, -sp->beta, -score, false);

        }
        
        sBoard.undo_move(*move);
        
        
        //first without mutex
        if(score > sp->bestscore) {
            
            //update
            std::lock_guard<std::mutex> lk(sp->lock);

            
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
            

        }
    }
    
    std::lock_guard<std::mutex> lk(sp->lock);

    
    sp->sBoard->board.n_nodes += board.n_nodes;
    
    sp->slaves[threadID] = false;
    sp->n_Slaves--;
    
}


int RXEngine::MG_PVS_shallow(const unsigned int threadID, RXBBPatterns& sBoard, const bool pv, const int depth, const int alpha, const int beta, const bool passed) {

    
    RXBitBoard& board = sBoard.board;
    int bestscore = UNDEF_SCORE;
    
    int upper = beta;
    int lower = alpha;

    
    if(depth == DEPTH_0) {
                
        return sBoard.get_score();
        
    } else if(depth == DEPTH_1) {
        
        unsigned long long legal_movesBB = board.get_legal_moves();
        if(legal_movesBB) {
            
            RXMove& move = threads[threadID]._move[board.n_empty][1];
            RXSquareList* empties = board.empties_list;
            do {
                empties = empties->next;

                const int pos = empties->position;
                const unsigned long long bit = 0x1ULL << pos;

                if(legal_movesBB & bit) {
                    legal_movesBB ^= bit;
                    
                    board.generate_flips(pos, move);
                    sBoard.patterns_update(move);
                    ++board.n_nodes;
                    int score= -sBoard.get_score(move);
                    
                    if (score>bestscore) {
                        bestscore = score;
                        if (bestscore>alpha)
                            lower = bestscore;
                    }
                }
                                
            } while (lower < upper && legal_movesBB);

        } else {
            
            if(passed) {
                return sBoard.final_score();
            } else {
                board.do_pass();
                bestscore = -sBoard.get_score(); //-MG_PVS_shallow(threadID, sBoard, pv, depth-1, -beta, -alpha, true);
                board.do_pass();
            }
            
        }

        return bestscore;
    }

#ifdef PV_EXTENSION
    //PV EXTENSION
    if (pv && use_pv_ext && (board.n_empty - depth) <= depth_pv_extension) {
        
        if (board.n_empty < EG_MEDIUM_HI_TO_LOW)
            return EG_PVS_ETC_LTT(threadID, board, true, alpha, beta, passed);
        
        return EG_PVS_ETC_mobility(threadID, sBoard, true, alpha, beta, passed);
        
    }
#endif
    
    //synchronized acces
    const unsigned long long hash_code = board.hashcode();
    
    unsigned int bestmove = NOMOVE;
            
    RXHashValue entry;
    if(hTable->get(hash_code, board, type_hashtable, entry)) {
        
        if(entry.selectivity == NO_SELECT && entry.depth >= depth) {
            
            if(entry.lower > lower) {
                
                lower = entry.lower;
                if(lower >= upper)
                    return lower;
                
            }
            
            if(!pv && entry.upper <= lower) {
                return  entry.upper;
            }
            
            bestmove = entry.move;

        }
        
    }
    
    
    if(bestmove != PASS) {
        
        RXMove* list = threads[threadID]._move[board.n_empty];
        RXMove* move = list + 1;
        
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);
            sBoard.patterns_update(*move);
            
            sBoard.do_move(*move);
            bestscore = -MG_PVS_shallow(threadID, sBoard, pv, depth-1, -upper, -lower, false);
            sBoard.undo_move(*move);
            
            if (bestscore>lower)
                lower = bestscore;
            
        }
        
        if(lower<upper) {
            
            unsigned long long legal_movesBB = board.get_legal_moves();
            if(bestmove != NOMOVE)
                legal_movesBB ^= 0x1ULL<<bestmove;
            
            // Evaluate moves for a future sort
            if(legal_movesBB) {
                
                RXMove *previous = list;
                
                // Bypassing JWC order improves throughput; but that move sequence may vary for equal scores.
                RXSquareList* empties = board.empties_list;
                do {
                    empties = empties->next;
                    
                    const int pos = empties->position;
                    const unsigned long long bit = 0x1ULL << pos;

                    if(legal_movesBB & bit) {
                        legal_movesBB ^= bit;
                        
                        board.generate_flips(pos, *move);
                        sBoard.patterns_update(*move);
                        
                        move->score = sBoard.get_score<WITHOUT_FM>(*move);
                        
                        ++board.n_nodes;
                        
                        previous = previous->next = move++;
                    }
                                        
                } while (legal_movesBB);
                
                previous->next = nullptr;
                
                if(bestscore == UNDEF_SCORE) {
                    
                    move = list->pick_next_promising_move();

                    sBoard.do_move(*move);
                    bestscore = -MG_PVS_shallow(threadID, sBoard, pv, depth-1, -upper, -lower, false);
                    sBoard.undo_move(*move);

                    bestmove = move->position;
                    if (bestscore>lower)
                        lower = bestscore;
                    
                    list = list->next;

                }

                while(lower < upper && list->next != nullptr) {
                    
                    move = list->next;
                    if(move->next != nullptr)    //more than 1 move
                        move = list->pick_next_promising_move();
                    
                    sBoard.do_move(*move);
                    
                    int score = -MG_PVS_shallow(threadID, sBoard, false, depth-1, -lower-1, -lower, false);
                    if(lower < score && score < upper)
                        score = -MG_PVS_shallow(threadID, sBoard, pv, depth-1, -upper, -score, false);
                    
                    sBoard.undo_move(*move);
                    
                    
                    if (score>bestscore) {
                        bestmove = move->position;
                        bestscore = score;
                        if (bestscore>lower)
                            lower = bestscore;
                    }
                    
                    list = list->next;
                    
                }
                
            }
        }
    }
    
    if(bestscore == UNDEF_SCORE) {
        
        if(passed) {
            return sBoard.final_score();
        } else {
            board.do_pass();
            
            bestscore = -MG_PVS_shallow(threadID, sBoard, pv, depth-1, -upper, -lower, true);
            
            board.do_pass();
            bestmove = PASS;
            
        }
    }
    
    
    hTable->update(hash_code, board, type_hashtable, NO_SELECT, depth, alpha, upper,  bestscore, bestmove);
    
    return bestscore;
    
}


/*
 NULL Window Search  + XProbCut
 */
int RXEngine::MG_NWS_XProbCut(const unsigned int threadID, RXBBPatterns& sBoard, const int pvDev, const int selectivity, const int depth, const int alpha, const bool passed) {
    
    if(depth == DEPTH_3)
       return alphabeta_last_three_ply(threadID, sBoard, false, alpha, alpha+1, passed);

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
                
        if(entry.selectivity >= selectivity && entry.depth >= depth) {
            
            if(entry.lower > alpha)
                return entry.lower;
            
            if(entry.upper <= alpha)
                 return  entry.upper;
            
        }
        
        bestmove = entry.move;


    }
    
    
    //param mpc
    int lower_probcut, upper_probcut;
    const int depth_probcut = (depth/4)*2 + (depth & 1);
    probcut_bounds(board, selectivity, depth, depth_probcut, pvDev, alpha, alpha+1, lower_probcut, upper_probcut);
    
    
    if(bestmove != NOMOVE && entry.selectivity >= selectivity && entry.depth>=depth_probcut) {
        
        if(entry.lower >= upper_probcut) {
            return alpha+1;
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
 
        const unsigned long long current_P = board.discs[board.player];
        const unsigned long long current_O = board.discs[board.player^1];
        
        const int etc_depth = depth-1;
        const int etc_depth_probcut = depth_probcut -1;

        //ENHANCED TRANSPOSITION CUTOFF
        if(bestmove != NOMOVE) {
            
            board.generate_flips(bestmove, *move);
            
#ifdef USE_ETC
            ++board.n_nodes;

            const unsigned long long next_P = current_O ^ move->flipped;
            const unsigned long long next_O = current_P | (move->square | move->flipped);
            const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);

            if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry) && entry.selectivity >= selectivity) {
                
                if(entry.depth >= etc_depth && -entry.upper > alpha) {
//                    hTable->update(hash_code, board, type_hashtable, selectivity, depth, alpha, -entry.upper, bestmove);
                    return -entry.upper ;
                }
                
                if(entry.depth >= etc_depth_probcut && -entry.upper >= upper_probcut) {
                    return alpha+1;
                }
            }
#endif
            
            previous = previous->next = move++;
            
        }
        
        if(depth > (bestmove == NOMOVE ? 9 : 8) ) {
            
            //for all empty square
            unsigned long long legal_movesBB = board.get_legal_moves();
            if(bestmove != NOMOVE)
                legal_movesBB ^= 0x1ULL<<bestmove;
            
              for(RXSquareList* empties = board.empties_list->next; legal_movesBB; empties = empties->next) {
                  const int pos = empties->position;
                  const unsigned long long bit = 0x1ULL << pos;
                  
                  if(legal_movesBB & bit) {
                      legal_movesBB ^= bit;
                      
                      board.generate_flips(pos, *move);
                      
                      move->score = 0;
                      
#ifdef USE_ETC
                      ++board.n_nodes;

                      const unsigned long long next_P = current_O ^ move->flipped;
                      const unsigned long long next_O = current_P | (move->square | move->flipped);
                      const unsigned long long next_hashcode = RXBitBoard::hashcode(next_P, next_O);

                      if(hTable->get(next_hashcode, next_P, next_O, type_hashtable, entry)) {
                          
                          if(entry.selectivity >= selectivity) {
                              if(entry.depth>=etc_depth && -entry.upper > alpha) {
//                                  hTable->update(hash_code, board, type_hashtable, selectivity, depth, alpha, -entry.upper, pos);
                                  return -entry.upper ;
                              }

                              if(entry.depth >= etc_depth_probcut && -entry.upper >= upper_probcut) {
                                  return alpha+1;
                              }
                          }

                          move->score = ((-entry.lower<=alpha)*6-2); // bonus= -2 : malus= +4
                      }
#endif
                      
                      previous = previous->next = move++;
                      
                  }
              }
            
            previous->next = nullptr;
            
        } else {
            
            if(bestmove == NOMOVE)
                board.moves_producing(list);
            else
                board.moves_producing(previous, previous->square);
        }
        
    }
    
    int bestscore = UNDEF_SCORE;
    
    
    if(list->next == nullptr) {
        //PASS
        if(passed) {
            return sBoard.final_score();
        } else {
            board.do_pass();
            bestscore = -MG_NWS_XProbCut(threadID, sBoard, pvDev, selectivity, depth-1,  -alpha-1, true);
            board.do_pass();
            bestmove = PASS;
        }
        
    } else {
        
        //XProbcut
        int type_probcut = probcut(threadID, sBoard, selectivity, alpha, depth, depth_probcut, lower_probcut, upper_probcut, list, bestmove != NOMOVE);
        if(type_probcut == BETA_CUT) {
            return alpha + 1;
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
        
        sBoard.do_move(*move);
        bestscore = -MG_NWS_XProbCut(threadID, sBoard, pvDev, selectivity, depth-1, -alpha-1, false);
        sBoard.undo_move(*move);
        
        bestmove = move->position;
        list = list->next;
        
        unsigned int depth_reduction = LMR_NO_REDUCTION;
        unsigned int n_moves = 1;

        int score;
        for(RXMove* iter = list->next; bestscore<=alpha && iter != nullptr; iter = iter->next, list = list->next) {
            
            // Split?
            if(activeThreads > 1 && (depth-depth_reduction)>MIN_DEPTH_SPLITPOINT && iter->next != nullptr && !abort.load(std::memory_order_relaxed)
               && !thread_should_stop(threadID && idle_thread_exists(threadID))
               && split(sBoard, false, pvDev+1, depth, selectivity, alpha, (alpha+1), bestscore, bestmove, list, threadID, RXSplitPoint::MID_XPROBCUT)) {
                
                break;
            }
            
#ifdef USE_LMR
            
            //before pvDev+1
            if(pvDev>1) {
                ++n_moves;
                if(n_moves>3 && depth>LMR_MIN_DEPTH) {
                    depth_reduction = DEPTH_2;
                    if(pvDev>2 && n_moves>5 && depth>LMR_DEEP_DEPTH) {
                        depth_reduction = DEPTH_4;
                    }
                }
            }
            
#endif

            sBoard.do_move(*iter);
            score = -MG_NWS_XProbCut(threadID, sBoard, pvDev+1, selectivity, depth-1-depth_reduction, -alpha-1, false);
#ifdef USE_LMR
            if(depth_reduction>LMR_NO_REDUCTION && score>alpha)
                score = -MG_NWS_XProbCut(threadID, sBoard, pvDev+1, selectivity, depth-1, -alpha-1, false);
#endif

            sBoard.undo_move(*iter);
            
            if (score>bestscore) {
                bestscore = score;
                bestmove = iter->position;
            }
            
        }
    }
    
    
    //interrupt search
    if(abort.load(std::memory_order_relaxed)  || thread_should_stop(threadID))
        return INTERRUPT_SEARCH;
    
    hTable->update(hash_code, board, type_hashtable, selectivity, depth, alpha, bestscore, bestmove);
    if(pvDev < 4)
       hTable_PV->update(hash_code, board, type_hashtable, selectivity, depth, alpha, bestscore, bestmove);

    
    return bestscore;
    
    
}

// MG_SP_search_XProbcut() is used to search from a PV split point.  This function
// is called by each thread working at the split point.  It is similar to
// the normal EG_NWS_XEndCut() function, but simpler.  Because we have already
// probed the hash table and searched the first move before splitting, we
// don't have to repeat all this work in MG_SP_search_XProbcut().  We also don't
// need to store anything to the hash table here:  This is taken care of
// after we return from the split point.

void RXEngine::MG_SP_search_XProbcut(RXSplitPoint* sp, const unsigned int threadID) {
    
    //    assert(threadID >= 0 && threadID < activeThreads);
    //    assert(activeThreads > 1);
    
    RXBBPatterns& sBoard = sp->sBoardStack[threadID];
    sBoard = *(sp->sBoard);                                 //operator=
    RXBitBoard& board = sBoard.board;
    
    unsigned int depth_reduction = LMR_NO_REDUCTION;

    unsigned int n_moves = 1;
    
    //here sp->alpha is const
    while(sp->bestscore <= sp->alpha && !abort.load(std::memory_order_relaxed)  && !thread_should_stop(threadID)) {
        
        RXMove* move = nullptr;
        {
            std::lock_guard<std::mutex> lk(sp->lock);
            if (sp->list->next == nullptr) break;  // unlock auto via destructeur, même sur le break
            move = sp->list->next;
            sp->list = move;
        }
 
#ifdef USE_LMR
        
        //after pvDev+1

        //LMR
        if(sp->pvDev > 2) {
            ++n_moves;

            if(n_moves>3 && sp->depth>LMR_MIN_DEPTH) {
                depth_reduction = DEPTH_2;
                if(sp->pvDev>3 && n_moves>5 && sp->depth>LMR_DEEP_DEPTH) {
                    depth_reduction = DEPTH_4;
                }
            }
        }
        
#endif

        const int alpha = sp->alpha; //local copy

        sBoard.do_move(*move);
        
        // here depth > MIN_DEPTH_SPLITPOINT <=> depth > 7
        int score = -MG_NWS_XProbCut(threadID, sBoard, sp->pvDev, sp->selectivity, sp->depth-1-depth_reduction, -alpha-1, false);
#ifdef USE_LMR
        if(depth_reduction > LMR_NO_REDUCTION && score>alpha)
            score = -MG_NWS_XProbCut(threadID, sBoard, sp->pvDev, sp->selectivity, sp->depth-1, -alpha-1, false);
#endif

        sBoard.undo_move(*move);
        
        //first without mutex
        if(score > sp->bestscore) {
            
            //update
            std::lock_guard<std::mutex> lk(sp->lock);

            if(sp->explored == false) {
                                
                // New best move?
                if(score > sp->bestscore) {
                    sp->bestscore = score;
                    sp->bestmove = move->position;
                    if(score > sp->alpha) {
                        sp->explored =true;
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

