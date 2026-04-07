/*
 *  IOStdProtocol.hpp
 *  Roxane
 *
 *  Created by BrunoCausse on 08/12/2025.
 *  Copyright 2025 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef STANDART_PROTOCOL_H
#define STANDART_PROTOCOL_H

#include "RXRoxane.hpp"
#include "StdInput.hpp" // Inclure notre nouvelle classe C++
#include <string>
#include <atomic>
#include <vector>
#include <pthread.h>

class RXRoxane;

class IOStdProtocol {
	
	RXRoxane* engine;
	StdInput input_handler; // Ajouter une instance de notre nouvelle classe StdInput
	
    std::atomic<int> running;
	
	// La signature de l'interpréteur de commandes change pour utiliser std::vector<std::string>
	void InterpretCommand(const std::vector<std::string>& args);
	void SyntaxError(const std::vector<std::string>& args);
	
	//synchro std::cout
	mutable pthread_mutex_t IOSync;

public:
	
	// Le constructeur devra initialiser IOSync, si ce n'est pas déjà fait dans le .cpp
	IOStdProtocol(RXRoxane* engine);
    ~IOStdProtocol();
	
	void MainLoop(void);
	void Print(const std::string msg) const;

};

#endif
