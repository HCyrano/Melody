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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <chrono>

#include "RXRoxane.hpp"
#include "RXEvaluation.hpp"
#include "RXBitBoard.hpp"


extern "C"
void* init_process(void* pt)  {
	
	RXRoxane* roxane = (RXRoxane*)(pt);
	
	roxane->run();
	
	return nullptr;
	
}


/*________________________________________________________________________________________________________*/


RXRoxane::RXRoxane(int size_hashtable, int max_thread): GGSClient(nullptr) {

	pthread_mutex_init(&mutex, nullptr);
    
    max_threads = max_thread;
    
/*
    //initialisations
	engine[BLACK] = new RXEngine(this, "../build/log/engine_0.txt", max_thread);
    engine[WHITE] = new RXEngine(this, "../build/log/engine_1.txt", max_thread);
*/
    

    //initialisations pour IOStd
    engine[BLACK] = new RXEngine(this, "./log/engine_0.txt", max_thread);
    engine[WHITE] = new RXEngine(this, "./log/engine_1.txt", max_thread);

    
	hTable = new RXHashTable(size_hashtable);
	main_PV = new RXHashTable(std::min(size_hashtable, 19));
	expected_PV = new RXHashTable(std::min(size_hashtable, 19));
	
    RXBitBoard::static_init();
    
 	RXEvaluation::load();
	
}

RXRoxane::~RXRoxane() {
    
    RXEvaluation::unload();
	
	delete expected_PV;
	delete main_PV;
	delete hTable;

	delete engine[WHITE];
	delete engine[BLACK];
	
	pthread_mutex_destroy(&mutex);
}

void RXRoxane::connectGGS(CODKStream* client) {
	GGSClient = client;
}

void RXRoxane::connectIOStd(IOStdProtocol* client) {
    IOClient = client;
}

void RXRoxane::stop_engine(COsGame* g) {
    
    int player = g->pos.board.fBlackMove? BLACK : WHITE;    
    engine[player]->resume();
    
}

void RXRoxane::stop_engine() {
    
    engine[BLACK]->resume();
    engine[WHITE]->resume();

}


/* unSynchronized method */
void RXRoxane::resume() {
		
	resume_flag.store(true);

	engine[BLACK]->resume();
	engine[WHITE]->resume();
	
	game[BLACK].Clear();
	game[WHITE].Clear();
    	
	hTable->reset();
	main_PV->reset();
	expected_PV->reset();
    
		
}

RXEngine* RXRoxane::getEngine(const int color) const {
    if (color == BLACK || color == WHITE)
        return engine[color];
    return nullptr;
}

///* synchronized method */
//void RXRoxane::check_allWTHOR() {
//	
//	//	../../WTH_7708/WTH_1977.wtb
//	
//	for(int annee = 1977; annee < 2009; annee++) {
//		
//		std::ostringstream file_name;
//		file_name << "../../WTH_7708/WTH_" << annee << ".wtb";
//		
//		std::ostringstream file_error;
//		file_error << "../../WTH_7708/WTH_" << annee << ".err";
//		
//		std::cout << file_name.str() << std::endl;
//		
//		check_WTHOR(file_name.str(), file_error.str());
//		
//	}
//}
//
//
///* synchronized method */
//void RXRoxane::check_WTHOR(const std::string& file_name, const std::string& file_error) {
//	
//	static const char wthor2BitBoard[] ={
//		0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
//		0, A1, B1, C1, D1, E1, F1, G1, H1, 0,
//		0, A2, B2, C2, D2, E2, F2, G2, H2, 0,
//		0, A3, B3, C3, D3, E3, F3, G3, H3, 0,
//		0, A4, B4, C4, D4, E4, F4, G4, H4, 0,
//		0, A5, B5, C5, D5, E5, F5, G5, H5, 0,
//		0, A6, B6, C6, D6, E6, F6, G6, H6, 0,
//		0, A7, B7, C7, D7, E7, F7, G7, H7, 0,
//	0, A8, B8, C8, D8, E8, F8, G8, H8, 0	};
//	
//	
//	mutex.lock();
//	
//	clientMode = RXSearch::kPrivate;
//
//	hTable->shared(true);
//	
//	int n_threads = engine[SHARED]->get_THREAD_MAX();
//	
//	parameter.idEngine = SHARED;
//	parameter.nThreads = std::max(1, n_threads);
//	parameter.clientMode = clientMode; //kPrivate
//	parameter.dependent_time = false;	
//	parameter.search_on_opponent_time = false;
//	
//	
//	std::ofstream to(file_error.c_str());
//	if(to) {
//		
//		
//		std::ifstream from(file_name.c_str(), std::ios::binary);
//		if(from) {
//			
//			//read header
//			char  c[] = {0};	// 1 octet		
//			short s[] = {0};	// 2 octets
//			int   i[] = {0};	// 4 octets
//			
//			//read date
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //siecle de creation
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //annee de creation
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //mois de creation
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //jour de creation
//			
//			//nombres d'enregistrements
//			from.read(reinterpret_cast<char*>(i), sizeof(int)); //nombre d'enregistrement 1
//			int nRecords = i[0];
//			from.read(reinterpret_cast<char*>(s), sizeof(short)); //nombre d'enregistrement 2
//			
//			//annee des parties
//			from.read(reinterpret_cast<char*>(s), sizeof(short));
//			
//			//parametres
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //P1
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //P2
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //P3
//			int solved = c[0];
//			
//			//reserved
//			from.read(reinterpret_cast<char*>(c), sizeof(char)); //
//			
//			for(int iRecord = 0; iRecord < nRecords; iRecord++) {
//				
//				from.read(reinterpret_cast<char*>(s), sizeof(short)); //N du tournoi
//				from.read(reinterpret_cast<char*>(s), sizeof(short)); //N joueur noir
//				from.read(reinterpret_cast<char*>(s), sizeof(short)); //N joueur blanc
//				
//				from.read(reinterpret_cast<char*>(c), sizeof(char)); //score reel
//				from.read(reinterpret_cast<char*>(c), sizeof(char)); //score theorique
//				int score_theorique = c[0];
//				
//				RXBBPatterns sBoard;
//				RXBitBoard& board = sBoard.board;
//				RXMove move;
//				
//				char moves[60];
//				from.read(reinterpret_cast<char*>(moves), 60*sizeof(char)); //move
//				int iMove = 0;
//				
//				bool error = false;
//				
//				do {
//					
//					int position = wthor2BitBoard[moves[iMove++]];
//					
//					if(board.squareIsEmpty(position) && board.generate_flips(position, move)) {
//						sBoard.patterns_update(move);
//
//						board.do_move(move);
//						*(sBoard.pattern) = *(move.pattern); //copy
//					} else {
//						error = true;
//						to << "***** RED ALERT *****" << std::endl;
//						
//						to << "record : " << iRecord << std::endl;
//						
//						to << "BAD GAME" << std::endl;
//						
//						for(int i = 0; i<60; i++)
//							to << RXMove::index_to_coord(wthor2BitBoard[moves[i]]) << " ";
//						
//						to << "\n" << std::endl;
//						
//					}
//					
//					if(board.isPassed())
//						board.do_pass();
//					
//				} while (!error && !board.isEndGame() && board.n_empty>solved);
//				
//				
//				if (!error && !board.isEndGame()) {
//					
//					std::cout << "record : " << iRecord << std::endl;
//					
//					RXBestPosition bPosition;
//					
//					engine[SHARED]->get_move(sBoard, hTable, bPosition, parameter);
//					
//					score_theorique = 2*score_theorique - 64;
//					
//					bPosition.score /= 1;
//					
//					if ((bPosition.score) != (board.player == BLACK? score_theorique:-score_theorique)) {
//						to << "***** RED ALERT *****" << std::endl;
//						
//						to << "record : " << iRecord << std::endl;
//						
//						to << "BAD RESULT" << std::endl;
//						
//						to << board << std::endl;
//						to << "score theorique : " << (board.player == BLACK? score_theorique:-score_theorique) << std::endl;
//						to << "score Reel      : " << bPosition.score << std::endl;
//						
//						to << std::endl;
//					}
//				}
//			}
//			
//			from.close();
//			
//		}
//		
//		to.close();
//	}
//	
//	mutex.unlock();
//	
//}


/* synchronized method */
void RXRoxane::get_move(const std::string& _idg, COsGame* g) {
	
	pthread_mutex_lock(&mutex);

	resume_flag.store(false);
    
	idg = _idg;

	RXBBPatterns& sBoard = search.sBoard;
	sBoard.build(g->pos.board.fromGGS());

	RXBitBoard& board = sBoard.board;

	
	//True if it is black's move
	int player = g->pos.board.fBlackMove? BLACK : WHITE;
	
	int id_engine = SHARED;
	
	int n_threads = 1;
	
	//verification que les deux jeux sont identiques
	//sauf 1er coup
	if(g->pos.board == g->posStart.board) {
		
		hTable->shared(true);
		main_PV->shared(true);
		expected_PV->shared(true);
		
		n_threads = engine[SHARED]->get_THREAD_MAX();
		
	} else {
		
		//same game -> engine[Shared]
		if(game[player^1].board.bt.n == 0 || game[player^1].board == g->pos.board) {
			
			if(!hTable->is_shared()) {
				engine[BLACK]->stop("engine BLACK stop : become SHARED");
				engine[WHITE]->stop("engine WHITE stop : become SHARED");
								
				hTable->mergePV(board);
				main_PV->mergePV(board);
				expected_PV->mergePV(board);

				hTable->shared(true);
				main_PV->shared(true);
				expected_PV->shared(true);

			}
			
			id_engine = SHARED;
			n_threads = engine[SHARED]->get_THREAD_MAX();
			
		} else {
			
			if(hTable->is_shared()) {
				
				engine[SHARED]->stop("engine SHARED stop");

				hTable->copyPV(expected_PV, RXHashTable::HASH_SHARED, board, board.player==BLACK? RXHashTable::HASH_BLACK:RXHashTable::HASH_WHITE);
				hTable->copyPV(main_PV, RXHashTable::HASH_SHARED, board, board.player==BLACK? RXHashTable::HASH_BLACK:RXHashTable::HASH_WHITE);

				hTable->copyPV_shared_to_color(   (engine[SHARED]->get_board()).board, player^1);
				main_PV->copyPV_shared_to_color((engine[SHARED]->get_board()).board, player^1);
				expected_PV->copyPV_shared_to_color((engine[SHARED]->get_board()).board, player^1);

				
				hTable->shared(false);
				main_PV->shared(false);
				expected_PV->shared(false);
								
                n_threads = engine[SHARED]->get_THREAD_MAX();
				
            } else {
                
                n_threads = std::max(1, (engine[player]->get_THREAD_MAX())/2);
                
            }
			
			id_engine = player;
		}
	}
	
	
	//affectation
	game[player] = g->pos;

	search.clientMode              = RXSearch::kGGSMode;
	search.idEngine                = id_engine;
	search.nThreads                = std::max(1, n_threads);
	search.htable                  = hTable;
	search.main_PV                 = main_PV;
	search.expected_PV             = expected_PV;
	search.search_on_opponent_time = (max_threads == 1? false:true);
	
	search.dependent_time = true;	
    
	search.tRemaining     = static_cast<int>(game[player].cks[player^1].tCurrent*1000);
    
    //debug
    //std::cout << "time remaining : " << search.tRemaining << std::endl;

    

	search.sBoard.build(g->pos.board.fromGGS());
	
	search.depth       = search.sBoard.board.n_empty;
	search.alpha       = -MAX_SCORE;
	search.beta        =  MAX_SCORE;
	search.selectivity = RXEngine::NO_SELECT;

	search.bestMove.position    = NOMOVE;
	search.bestMove.score       = UNDEF_SCORE;
	search.bestMove.selectivity = 0;
	search.bestMove.tElapsed    = 0.0;
	search.bestMove.nodes	    = 0;
	
	if(GGSClient->IsConnected() && board.n_empty>19) {
		sendMsg("--");
		sendMsg("game " + idg);

		std::ostringstream buffer, buffer1;
		buffer	<< "HashTable " << (hTable->is_shared()? "SHARED" : (player == BLACK? "BLACK" : "WHITE")) << std::endl;
		sendMsg(buffer.str());
		buffer1 << "Active thread" << (n_threads >1? "s : ":" : ") <<  n_threads  << std::endl;
		sendMsg(buffer1.str());
		
	}
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	
	if(!resume_flag.load() && pthread_create(process, &attr, init_process, (void*)(this)) != 0)
		std::cout << "Echec: Thread main Roxane" << std::endl;
	
	
	pthread_attr_destroy(&attr);
	
	pthread_mutex_unlock(&mutex);
}

/* synchronized method */
// solved fforum test
void RXRoxane::get_move(const std::string& file_name) {
    
    
    pthread_mutex_lock(&mutex);
    
    //    while(true) {
    
    resume_flag.store(false);
    
    double T = 0;
    unsigned long long nodes = 0;
    
    hTable->shared(true);
    
    int n_threads = engine[SHARED]->get_THREAD_MAX();
    
    search.clientMode = RXSearch::kPrivate;
    search.idEngine = SHARED;
    search.nThreads = std::max(1, n_threads);
    search.htable = hTable;
    search.main_PV = main_PV;
    search.expected_PV = expected_PV;
    search.search_on_opponent_time = false;
    
    search.dependent_time = false;

#ifdef GENERATE_RES_FILE
    std::string out_file_name = file_name; //copy
    out_file_name.replace(out_file_name.end()-3, out_file_name.end(), "res"); //change extension
    
    std::ofstream ofs(out_file_name.c_str()); //ouverture en ecriture

#endif
    
    std::ifstream in(file_name.c_str());
    
    if(in) {
        
        
        std::string line;

#ifdef STATS_FM
        RXBBPatterns::init_stats();
#endif
                
        while(!resume_flag.load() && std::getline(in, line)) {
            
            
#ifdef EG_CHECK_PV
            std::stringstream ss;
            int score = UNDEF_SCORE;

            ss << line.substr(line.find(":")+1);

            ss >> score;
#endif
            
            
#ifdef EG_CHECK_SOLVER
            std::vector<std::string> best_moves_attendus; // Contiendra uniquement les meilleurs coups (ex: ["H8", "H1"])
            int best_score_attendu = UNDEF_SCORE;

            size_t pos_coups = line.find(';');
            if (pos_coups != std::string::npos) {
                std::stringstream ss(line.substr(pos_coups + 1));
                std::string token;
                
                while (ss >> token) {
                    size_t double_point = token.find(':');
                    if (double_point != std::string::npos) {
                        std::string coord = token.substr(0, double_point);
                        std::string score_str = token.substr(double_point + 1);
                        if (!score_str.empty() && score_str.back() == ';') {
                            score_str.pop_back();
                        }
                        int score_courant = std::stoi(score_str);

                        // Premier coup analysé -> il possède obligatoirement le bestscore
                        if (best_score_attendu == UNDEF_SCORE) {
                            best_score_attendu = score_courant;
                            best_moves_attendus.push_back(coord);
                        }
                        // Coups suivants : on les ajoute seulement s'ils ont le même score maximal
                        else if (score_courant == best_score_attendu) {
                            best_moves_attendus.push_back(coord);
                        }
                        // Dès que le score baisse, inutile d'aller plus loin (grâce au tri du fichier)
                        else {
                            break;
                        }
                    }
                }
            }
#endif // EG_CHECK_SOLVER
            
            search.htable->reset();
            search.main_PV->reset();
            search.expected_PV->reset();
            engine[search.idEngine]->resume(); //hTable_shallow->reset()
            
            
            search.sBoard.build(line);
            search.depth       = search.sBoard.board.n_empty;
            search.alpha       = -MAX_SCORE;
            search.beta        = +MAX_SCORE;
            search.selectivity = RXEngine::NO_SELECT;
            
            search.bestMove.position    = NOMOVE;
            search.bestMove.score       = UNDEF_SCORE;
            search.bestMove.selectivity = 0;
            search.bestMove.tElapsed    = 0.0;
            search.bestMove.nodes        = 0;
            
            if(search.sBoard.board.n_moves() > 1) {
                engine[search.idEngine]->get_move(search);
                
#ifdef GENERATE_RES_FILE
                ofs << line << ":" << std::setw(3) << std::setfill(' ') << search.bestMove.score << std::endl;
#endif
                
#ifdef EG_CHECK_PV
                if(score != (UNDEF_SCORE) && search.bestMove.score != score) {
                    std::cout << "critical error in solver" << std::endl;
                    std::cout << search.sBoard.board << std::endl;
                    std::cout << "resultat attendu : " << score << std::endl;
                    std::cout << "resultat trouvé  : " << search.bestMove.score << std::endl;
                }
#endif
                
#ifdef EG_CHECK_SOLVER
                if (best_score_attendu != UNDEF_SCORE) {
                    // 1. Vérification stricte du score unique
                    if (search.bestMove.score != best_score_attendu) {
                        std::cout << "CRITICAL ERROR: Score incorrect!" << std::endl;
                        
                        std::cout << line << std::endl;
                        std::cout << "Attendu (BestScore): " << best_score_attendu << " | Trouvé: " << search.bestMove.score << std::endl;
                    } else {
                        // 2. Le score est correct, on vérifie si le coup joué fait partie des BestMoves
                        std::string coup_trouve_str = RXMove::index_to_coord(search.bestMove.position);
                        
                        bool coup_valide = false;
                        for (const auto& move : best_moves_attendus) {
                            if (move == coup_trouve_str) {
                                coup_valide = true;
                                break;
                            }
                        }
                        
                        if (!coup_valide) {
                            std::cout << "CRITICAL ERROR: Le coup trouvé n'est pas un BestMove!" << std::endl;
                            std::cout << line << std::endl;

                            std::cout << "Coup trouvé par le moteur: " << coup_trouve_str << " (Score: " << search.bestMove.score << ")" << std::endl;
                            std::cout << "Coups valides attendus pour ce score: ";
                            for (const auto& move : best_moves_attendus) std::cout << move << " ";
                            std::cout << std::endl;
                        }
                    }
                }
#endif // EG_CHECK_SOLVER
            }
            
            T += search.bestMove.tElapsed;
            nodes += search.bestMove.nodes;
            
        }
        
        engine[SHARED]->writeLog("---------------------------------------------------------------------------------------------");
        engine[SHARED]->writeLog("Total time  :     " + toHMS(T));
        
        
        std::ostringstream buffer;
        
        std::locale loc(std::locale(),new My_punct);
        buffer.imbue(loc);
        
        
        buffer << "Total nodes : " << std::setw(15) <<  nodes << "\n";
        
        int speed = 0;
        if(T !=0)
            speed = static_cast<int>((nodes/1000)/T);
        
        buffer << "Speed       : " << std::setw(15);
        if (nodes > 300000) {
            buffer  << speed << " kN/s";
        } else {
            buffer << "N/A";
        }
        engine[SHARED]->writeLog(buffer.str());
        
        engine[SHARED]->writeLog("");
        
        in.close();
        
        std::cout.imbue(loc);
        
        std::cout << "Total time  : " << toHMS(T) << std::endl;
        std::cout << "Total nodes : " << nodes << std::endl;
        if (T>0) {
            std::cout << "N/s         : " << static_cast<int>(nodes/T) << std::endl;
        }

#ifdef STATS_FM
        RXBBPatterns::print_stats();
#endif
        
    }
    
    //    }

#ifdef GENERATE_RES_FILE
    ofs.close();
#endif

    pthread_mutex_unlock(&mutex);
    
}

void RXRoxane::metrix(const int stage) {
    
    pthread_mutex_lock(&mutex);
        
    resume_flag.store(false);
    
    std::string dir_str = "/Users/caussebruno/Documents/developpement/";
    
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << stage;
    
    std::string file_name = dir_str + "/database/Edax_Egrcd_Roxane/stages/stage_" + oss.str() + ".txt";

    std::ifstream in(file_name.c_str());
    
    if(in) {
        
        std::string line;

        // Accumulateurs RMSE / MAE
        double sum_sq_err    = 0.0;
        double fm_sum_sq_err = 0.0;
        double sum_abs_err   = 0.0;
        double fm_sum_abs_err= 0.0;
        long   n_positions   = 0;

        while(!resume_flag.load() && std::getline(in, line)) {
            
            std::stringstream ss;
            int score;
            ss << line.substr(line.find(" ")+1);
            ss >> score;
            
            std::string othellier = line.substr(0, 65) + 'X';
            
            RXBBPatterns sBoard;
            sBoard.build(othellier);
            
            int eval    = sBoard.get_score<false>();
            int eval_fm = sBoard.get_score();

            // RMSE
            double err = static_cast<double>(eval - score);
            sum_sq_err += err * err;
            sum_abs_err += std::abs(err);

            double err_fm = static_cast<double>(eval_fm - score);
            fm_sum_sq_err  += err_fm * err_fm;
            fm_sum_abs_err += std::abs(err_fm);

            ++n_positions;
        }

        if(n_positions > 0) {
            const double n = static_cast<double>(n_positions);

            double rmse    = std::sqrt(sum_sq_err    / n);
            double rmse_fm = std::sqrt(fm_sum_sq_err / n);
            double mae     = sum_abs_err    / n;
            double mae_fm  = fm_sum_abs_err / n;

            std::cout << "stage " << oss.str()
                      << "  RMSE= "    << std::fixed << std::setprecision(4) << rmse
                      << "  MAE= "     << std::fixed << std::setprecision(4) << mae
                      << "  RMSE FM= " << std::fixed << std::setprecision(4) << rmse_fm
                      << "  MAE FM= "  << std::fixed << std::setprecision(4) << mae_fm
                      << "  delta= "   << std::fixed << std::setprecision(4) << (rmse - rmse_fm)
                      << "  n positions= " << n_positions
                      << std::endl;
        } else {
            std::cout << "  aucune position " << std::endl;
        }
        
        in.close();
    }
    
    pthread_mutex_unlock(&mutex);
}


void RXRoxane::get_move_fixed_depth(const std::string& position, const int depth, const int selectivity) {
    
    pthread_mutex_lock(&mutex);

    resume_flag = false;
        
    hTable->shared(true);
    
    int n_threads = engine[SHARED]->get_THREAD_MAX();

    search.clientMode = RXSearch::kIOStd;
    search.idEngine = SHARED;
    search.nThreads = std::max(1, n_threads);
    
    search.htable = hTable;
    search.main_PV = main_PV;
    search.expected_PV = expected_PV;
    
    search.search_on_opponent_time = false;
        
    search.dependent_time = false;
    
    search.sBoard.build(position);
    search.depth       = std::min(search.sBoard.board.n_empty, depth);
    search.alpha       = -MAX_SCORE;
    search.beta        = +MAX_SCORE;
    search.selectivity = RXEngine::confidence_to_selectivity(selectivity);

    search.bestMove.position    = NOMOVE;
    search.bestMove.score       = UNDEF_SCORE;
    search.bestMove.selectivity = 0;
    search.bestMove.tElapsed    = 0.0;
    search.bestMove.nodes        = 0;
    
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    
    if(!resume_flag && pthread_create(process, &attr, init_process, (void*)(this)) != 0) {
        std::cout << "Echec: Thread main Roxane" << std::endl;
    }
    
    
    pthread_attr_destroy(&attr);
            
    pthread_mutex_unlock(&mutex);

}

void RXRoxane::get_move_time_limited(const std::string& position, const int time_remaining) {
    
    pthread_mutex_lock(&mutex);

    resume_flag = false;
        
    hTable->shared(true);
    
    int n_threads = engine[SHARED]->get_THREAD_MAX();

    search.clientMode = RXSearch::kIOStd;
    search.idEngine = SHARED;
    search.nThreads = std::max(1, n_threads);
    
    search.htable = hTable;
    search.main_PV = main_PV;
    search.expected_PV = expected_PV;
    
    search.search_on_opponent_time = false;
        
    search.dependent_time = true;
    search.tRemaining     = time_remaining;

    search.sBoard.build(position);
    search.depth       = search.sBoard.board.n_empty;
    search.alpha       = -MAX_SCORE;
    search.beta        = +MAX_SCORE;
    search.selectivity = RXEngine::NO_SELECT;

    search.bestMove.position    = NOMOVE;
    search.bestMove.score       = UNDEF_SCORE;
    search.bestMove.selectivity = 0;
    search.bestMove.tElapsed    = 0.0;
    search.bestMove.nodes        = 0;
    
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    
    
    if(!resume_flag && pthread_create(process, &attr, init_process, (void*)(this)) != 0) {
        std::cout << "Echec: Thread main Roxane" << std::endl;
    }
    
    
    pthread_attr_destroy(&attr);
            
    pthread_mutex_unlock(&mutex);

}

#ifdef GENERATE_RAWDATA

void RXRoxane::rawdata(const std::string& dir_name, const int offset_start, const int n_games) {

    /* synchronized method */
    pthread_mutex_lock(&mutex);
    
    /* preparation du moteur*/
    resume_flag.store(false);
    
    hTable->shared(true);
    
    int n_threads = engine[SHARED]->get_THREAD_MAX();
    
    search.clientMode = RXSearch::kPrivate;
    search.idEngine = SHARED;
    search.nThreads = std::max(1, n_threads);
    search.htable = hTable;
    search.main_PV = main_PV;
    search.expected_PV = expected_PV;
    search.search_on_opponent_time = false;
    
    search.dependent_time = false;
    
    /* initialisation time */
    // 1. Enregistrer l'heure de début
    auto start_time = std::chrono::high_resolution_clock::now();
    auto start_iter = start_time;
        
    // Convertir l'heure de début en une représentation lisible (facultatif mais utile)
    std::time_t start_tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "🚀 Début du calcul à : " << std::put_time(std::localtime(&start_tt), "%H:%M:%S") << std::endl;
    
    
    std::string path_in  = "/Users/caussebruno/Documents/developpement/referee/game_log_detailed.txt";
    std::string path_out = dir_name + "/Roxane/base_00.txt";

    /* fichier de sortie */
    // Write at the end of the file if it exists and offset_start != 0; otherwise, create it
    std::ofstream ofs(path_out.c_str(), offset_start == 0 ? std::ios::trunc:std::ios::app);


    std::ifstream ifs(path_in.c_str());
    
    if(ifs) {

        std::string line;
        
        int idx = -1;
        int idx_end = offset_start+n_games;
        
        while(!resume_flag.load() && ++idx < idx_end && std::getline(ifs, line)) {
            
            if(idx < offset_start)
                continue;
            
            if(idx != offset_start && idx % 1000 == 0) {
                // 2. Enregistrer l'heure de fin
                auto end_time = std::chrono::high_resolution_clock::now();
                    
                // 3. Calculer le temps écoulé
                std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_iter);
                    
                // Convertir l'heure de fin en une représentation lisible
                std::time_t end_tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::cout << "                     : " << std::put_time(std::localtime(&end_tt), "%H:%M:%S");

                // 4. Afficher le résultat
                std::cout << " ✅ Temps total écoulé pour 1000 itérations : "
                          << duration.count() << " millisecondes" << std::endl;
                
                start_iter = end_time;
            }
            
            std::istringstream iss(line);
            
            //            int type_data;
            //
            //            iss >> type_data;
            //            if(type_data == 2) {
            
            //parser la ligne
            
            //extraire le score
            int score;
            iss >> score;
            
            //extraire les 8 lignes du plateau
            std::string board_txt;
            
            //                std::string ligne;
            //
            //                for (int i = 0; i < 8; i++) {
            //                    iss >> ligne;
            //                    board_txt += ligne;
            //                }
            
            iss >> board_txt;
            
            //extraire la couleur
            std::string color;
            iss >> color;
            board_txt += " " + color;
            
            //extraire la liste des coups
            std::string list_moves;
            iss >> list_moves;
            
            std::vector<std::string> moves_tab;
            for (size_t i = 0; i < list_moves.size(); i += 2) {
                moves_tab.push_back(list_moves.substr(i, 2));
            }
            
            search.sBoard.build(board_txt);
            RXBBPatterns& sBoard = search.sBoard;
            RXBitBoard& board = sBoard.board;
            int player = board.player;
            
            RXPattern* pattern = sBoard.pattern;
            
            RXMove move;
            
            for( int id_move = 0; id_move < static_cast<int>(moves_tab.size()); ++id_move) {
                
                
                if(14 < board.n_empty) {
                    
                    search.alpha       = -MAX_SCORE;
                    search.beta        = +MAX_SCORE;
                    if (26 < board.n_empty){
                        search.depth       = 13;
                        search.selectivity = RXEngine::MG_SELECT;
                    } else if(22 < board.n_empty) {
                        search.depth       = board.n_empty;
                        search.selectivity = 3; //91%
                    } else {
                        search.depth       = board.n_empty;
                        search.selectivity = RXEngine::NO_SELECT;
                    }
                    
                    
                    search.bestMove.position    = NOMOVE;
                    search.bestMove.score       = UNDEF_SCORE;
                    search.bestMove.selectivity = 0;
                    search.bestMove.tElapsed    = 0.0;
                    search.bestMove.nodes       = 0;
                    
                    if(search.sBoard.board.n_moves() > 1) {
                        
                        engine[search.idEngine]->get_move(search);
                        
                        ofs << board.string_rawdata() << " " << search.bestMove.score << std::endl;
                    }
                } else {
                    
                    if(search.sBoard.board.n_moves() > 0)
                        ofs << board.string_rawdata() << " " << (board.player == player? score:-score) << std::endl;
                    
                }
                
                
                
                int pos = RXMove::coord_to_index(moves_tab[id_move]);
                
                if(pos == PASS) {
                    board.do_pass();
                } else {
                    board.generate_flips(pos, move);
                    sBoard.patterns_update(move);
                    
                    sBoard.do_move(move);
                }
                
                
            }
            
            sBoard.pattern = pattern;
            
            
            //reset hashtables
            search.htable->reset();
            search.main_PV->reset();
            search.expected_PV->reset();
            engine[search.idEngine]->resume(); //hTable_shallow->reset()
            
            
            //            }
        }

        ifs.close();

    }
    
    ofs.close();

    // Convertir l'heure de fin en une représentation lisible
    std::time_t end_tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::cout << "🏁 Fin du calcul à   : " << std::put_time(std::localtime(&end_tt), "%H:%M:%S") << std::endl;
    
    pthread_mutex_unlock(&mutex);

}

#endif



#ifdef TUNE_PROBCUT_MID
void RXRoxane::get_probcut_mid_data() {
    
    pthread_mutex_lock(&mutex);
   
    engine[SHARED]->probcut_mid_data(hTable, main_PV);
    
    pthread_mutex_unlock(&mutex);

}
#endif

#ifdef TUNE_PROBCUT_END
void RXRoxane::get_probcut_end_data() {
    
    pthread_mutex_lock(&mutex);
   
    engine[SHARED]->probcut_end_data(hTable, main_PV);
    
    pthread_mutex_unlock(&mutex);

}
#endif

#ifdef TUNE_PROBCUT_END2
void RXRoxane::get_probcut_end2_data(const std::string& file_name) {
    
    pthread_mutex_lock(&mutex);
   
    engine[SHARED]->probcut_end2_data(file_name, hTable, main_PV);
    
    pthread_mutex_unlock(&mutex);

}
#endif

void* RXRoxane::run() {


	RXSearch s(search); //copy
	RXBitBoard& board = s.sBoard.board;
	
	if(board.hashcode() == hash_opening[60-board.n_empty])
		s.bestMove.position = move_opening[60-board.n_empty];
	else
		engine[s.idEngine]->get_move(s);
	
	
    if(!resume_flag.load()) {
        
        if(s.clientMode == RXSearch::kGGSMode) {
            
            COsMoveListItem mli;
            s.bestMove.to_COsMoveListItem(mli);
            
            game[board.player].Update(mli);
            
            if(!resume_flag.load() && GGSClient->IsConnected())
                GGSClient->SendMove(idg, mli);
            
            
        } else if(s.clientMode == RXSearch::kIOStd) {
            
            IOClient->Print("move " + RXMove::index_to_coord(s.bestMove.position));
            
        }
    } else { //test bug
       std::cerr << "run(): move suppressed by resume_flag"
                 << " move=" << RXMove::index_to_coord(s.bestMove.position)
                 << std::endl;
   }
    
    return nullptr;
	
	
	
}


void RXRoxane::imposed_opening(const std::string& line) {
	
	RXBitBoard board;
	RXMove move;
	
	for(unsigned int index = 0; index<line.length()-1; index++) {
		char abscise = std::toupper(line[index]);
		if(	abscise == 'A' ||
		   abscise == 'B' ||	
		   abscise == 'C' ||	
		   abscise == 'D' ||	
		   abscise == 'E' ||	
		   abscise == 'F' ||	
		   abscise == 'G' ||	
		   abscise == 'H') {
			
			char ordonne = std::toupper(line[index+1]);
			if(	ordonne == '1' ||
			   ordonne == '2' ||	
			   ordonne == '3' ||	
			   ordonne == '4' ||	
			   ordonne == '5' ||	
			   ordonne == '6' ||	
			   ordonne == '7' ||	
			   ordonne == '8') {
				
				int square = (('8' - ordonne)*8 + ('H' - abscise));
				
                board.generate_flips(square, move);
                
				if(move.flipped) {
					hash_opening[60-board.n_empty] = board.hashcode();
					move_opening[60-board.n_empty] = square;
					
					board.do_move(move);
					if(board.isPassed())
						board.do_pass();
					
				} else {
					break;
				}
				
				++index;
				
			}
		}
	}
	
	std::cout << "imposed line" << std::endl;
	
}

void RXRoxane::sendMsg(std::string msg) {
	
	if(GGSClient != nullptr && search.clientMode == RXSearch::kGGSMode && GGSClient->IsConnected())
		GGSClient->SendMsg(msg);
	else
		std::cout << msg << std::endl;
	
}


