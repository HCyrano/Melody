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

#ifndef RXROXANE_HPP
#define RXROXANE_HPP

#include <string>
#include <atomic>

#include "OsObjects.hpp"
#include "ODKStream.hpp"

#include "IOStdProtocol.hpp"

#include "RXConstantes.hpp"
#include "RXBBPatterns.hpp"
#include "RXHashTable.hpp"
#include "RXEngine.hpp"
#include "RXHelper.hpp"
#include "RXSearch.hpp"

extern "C"
void* init_process(void* pt);

class CODKStream;
class IOStdProtocol;

class RXRoxane: public Runnable {

	RXHashTable* hTable;
	RXHashTable* main_PV;
	RXHashTable* expected_PV;

	RXEngine* engine[2]; //BLACK and WHITE
	RXSearch search;

	pthread_t process[1];
	
	pthread_mutex_t mutex;
	
    std::atomic_bool resume_flag;
    
	//interface GGS server
	CODKStream* GGSClient;
	COsPosition game[2];
    std::string idg;

    //interface Std IO
    IOStdProtocol* IOClient;

    
	unsigned long long hash_opening[60];
	char move_opening[60];

	

public :
    
    int max_threads;


	RXRoxane(int size_hashtable = 20, int max_thread = 1);	
	~RXRoxane();
	
	
	void resume();

	virtual void* run();
    
    RXEngine* getEngine(const int color) const;
    

#ifdef TUNE_PROBCUT_MID
    void get_probcut_mid_data();
#endif

#ifdef TUNE_PROBCUT_END
    void get_probcut_end_data();
#endif
    
#ifdef TUNE_PROBCUT_END2
    void get_probcut_end2_data(const std::string& file_name);
#endif
    
	//privateProtocol
	void get_move(const std::string& file_name);
    void stop_engine();

    
    void rawdata(const std::string& dir_name, const int offset_start, const int n_games);


	//wthorProtocol
	void check_allWTHOR();
	void check_WTHOR(const std::string& file_name, const std::string& file_error);
	
	
	//GGSProtocol
	void connectGGS(CODKStream* client);
	void imposed_opening(const std::string& line);
    
    void stop_engine(COsGame* game);

	void get_move(const std::string& _idg, COsGame* game);

	void sendMsg(std::string msg);
    
    //IOProtocol
    void connectIOStd(IOStdProtocol* client);

    void get_move_fixed_depth(const std::string& position, const int depth, const int selectivity);
    void get_move_time_limited(const std::string& position, const int time_remaining);
    
    void RSME(const int stage);

};


#endif

