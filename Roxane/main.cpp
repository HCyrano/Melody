/*
 *  main.cpp
 *  Roxane
 *
 *  Created by B Causse modify 12/09/2024.
 *  Copyleft 2005-2025 personnel.
 *
 */

#include <iostream>
#include <string>
#include <sstream>
#include <locale>
#include <pthread.h>

#include "ODKStream.hpp"
#include "IOStdProtocol.hpp"
#include "RXRoxane.hpp"
#include "RXBBPatterns.hpp"
#include "RXEvaluation.hpp"

#include "RXEngine.hpp"
#include "RXTools.hpp"
#include "RXSetting.hpp"


int main (int argc, char * const argv[]) {
        
#if ARCH ==  ARCH_ARM_NEON
    std::cout << "ARM NEON available" << std::endl;
#elif ARCH == ARCH_X86_AVX2
    std::cout << "AVX2 available" << std::endl;
#else
    std::cout << "X86-64 generic" << std::endl;
#endif


	std::string login, password, file_name, mode, imposed_opening = "";
	unsigned int nBitsTable = 20;
	unsigned int nThreads = 1;

    unsigned int offset_start = 0;
    unsigned int n_games = 1000;

    bool rsme_flag = false;
    int stage_start = 0;
    int stage_end   = 59;

	for(int i = 1; i<argc; i++) {
		
		std::string arg(argv[i]);
		if(arg == "-login" && i+1<argc) {
			login = argv[++i];
		} else if(arg == "-passw" && i+1<argc) {
			password =  argv[++i];
		} else if(arg == "-fixeline" && i+1<argc) {
			imposed_opening = argv[++i];
		}  else if(arg == "-h" && i+1<argc) {
			std::istringstream iss(argv[++i]);
			iss >> nBitsTable;
		} else if(arg == "-t" && i+1<argc) {
			std::istringstream iss(argv[++i]);
			iss >> nThreads;
        } else if(arg == "-rawdata" && i+2<argc) {
            std::istringstream iss_offset(argv[++i]);
            iss_offset >> offset_start;
            std::istringstream iss_game(argv[++i]);
            iss_game >> n_games;
        } else if(arg == "-rmse" && i+2<argc) {
            
            rsme_flag = true;
            std::istringstream iss_start(argv[++i]);
            iss_start >> stage_start;
            std::istringstream iss_end(argv[++i]);
            iss_end >> stage_end;

        } else if(arg == "-mode" && i+1<argc) {
			mode =  argv[++i];
		} else if(file_name.empty()) {
			file_name = argv[i];
		}
	}

    std::string version = "build-2026-05-07 12h00";
    std::string vers_eval = RXEvaluation::get_version();

	std::cout << "Version Melody " << version << std::endl;
    std::cout << "Version Evaluation " << vers_eval << std::endl;

	std::cout << "Number of threads: " << nThreads << std::endl;
	std::cout << "Size hashTable: 2^" << nBitsTable << std::endl;
    std::cout << file_name << std::endl;
        
	RXRoxane roxane(nBitsTable, nThreads);
    

#ifdef TUNE_PROBCUT_MID
    
    roxane.get_probcut_mid_data();
    
#else
    
    
#ifdef TUNE_PROBCUT_END
    
    roxane.get_probcut_end_data();
    
    
#else
    
#ifdef TUNE_PROBCUT_END2
    
    if(!file_name.empty())
        roxane.get_probcut_end2_data(file_name);
    
    
#else

#ifdef GENERATE_RAWDATA

    //base de donnée
    if(!file_name.empty())
        roxane.rawdata(file_name, offset_start, n_games);
    
#else
    
    //warm up
    if(!file_name.empty()) {
        roxane.get_move(file_name);
    }
    
#endif
    
    if(rsme_flag) {
        stage_start= std::max(0, stage_start);
        stage_end = std::min(59, std::max(stage_start, stage_end));
        
        std::cout << stage_start << "-" << stage_end << std::endl;

        
        for(int stage = stage_start; stage<= stage_end; ++stage)
            roxane.RSME(stage);
    }
     
    
    if(imposed_opening != "")
        roxane.imposed_opening(imposed_opening);
    
    
    //	if(mode == "wthor")
    //		roxane.check_allWTHOR(); //try for robust endgame
    
    
    if(mode == "ggs" && !login.empty() && !password.empty()) {
        
        int err;
        CODKStream gs;
        
        //liaison Roxane GGS
        roxane.connectGGS(&gs);
        gs.pComputer = &roxane;

        // Activer reconnexion auto : 5 tentatives, 2 secondes entre chaque
        gs.EnableAutoReconnect(true, 5, 2000);

        // Connect(server, port)
        if ((err = gs.Connect("www.skatgame.net",5000))) {
            std::cout << "erreur conection" << std::endl;
            
            std::cerr << gs.ErrText(err) << std::endl;
            return err;
        }
        
        // Login(name, password)
        if ((err = gs.Login(login.c_str(), password.c_str()))) {
            std::cerr << gs.ErrText(err) << std::endl;
            gs.Disconnect();
            return err;
        }
        
        // Pour obtenir 5, 10, 20... secondes :
        gs.EnableAutoReconnect(true, 10, 5000);
        
        gs.SendMsg("version : " + version);
        gs.SendMsg("evaluation : " + vers_eval);


        gs.Process();			// receive, parse, and pass on messages
        
    }
    
    if(mode == "IOStd") {
        
        IOStdProtocol StdIO(&roxane);
        roxane.connectIOStd(&StdIO);
        
        StdIO.MainLoop();
        
    }

    
    
#endif
#endif
#endif
    
    return 0;
}
 



