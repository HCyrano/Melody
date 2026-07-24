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

#ifndef STANDART_PROTOCOL_H
#define STANDART_PROTOCOL_H

#include <string>
#include <atomic>
#include <vector>
#include <mutex>

#include "RXRoxane.hpp"
#include "StdInput.hpp"

class RXRoxane;

class IOStdProtocol {
	
	RXRoxane* engine;
	StdInput input_handler; // Ajouter une instance de notre nouvelle classe StdInput
	
    std::atomic<int> running;
	
	// La signature de l'interpréteur de commandes change pour utiliser std::vector<std::string>
	void InterpretCommand(const std::vector<std::string>& args);
	void SyntaxError(const std::vector<std::string>& args);
	
	//synchro std::cout
	mutable  std::mutex IOSync;

public:
	
	// Le constructeur devra initialiser IOSync, si ce n'est pas déjà fait dans le .cpp
	IOStdProtocol(RXRoxane* engine);
	
	void MainLoop(void);
	void Print(const std::string msg) const;

};

#endif
