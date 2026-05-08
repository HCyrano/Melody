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

#include "IOStdProtocol.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib> // Pour atoi

/*
 *******************************************************************************
 * Constructeur                                                              *
 *******************************************************************************
 */
// Initialise le moteur et le mutex pour la synchronisation
IOStdProtocol::IOStdProtocol(RXRoxane* e) : engine(e) {
    // Initialisation du mutex (important en C++ pour l'objet StdInput)
    pthread_mutex_init(&IOSync, nullptr);
}

/*
 *******************************************************************************
 * MainLoop() : boucle principale                                            *
 *******************************************************************************
 */
void IOStdProtocol::MainLoop(void) 
{ 
    // On n'appelle plus SetReadStream(stdin) ici, car StdInput est initialisé 
    // par défaut dans son constructeur pour utiliser stdin.
    
    running = 1;

    while (running) {
        
        // 1. Nouvelle méthode C++ : Lecture sans tampon C
        std::string command_line = input_handler.Read();
                
        // Read retourne une chaîne vide ("") en cas de EOF ou d'erreur
        if (command_line.empty()) {
            // Dans l'ancienne version, readstatus < 0 provoquait un break.
            // Si la lecture est vide, on peut considérer la fin du flux.
            if (input_handler.ReadInput() == 0) { // Vérifie si l'EOF est atteint
                 break;
            }
            continue; // Si vide mais pas EOF, on continue
        }
        
        // 2. Nouvelle méthode C++ : Analyse en vector<string>
        // Le délimiteur " " est conservé
        std::vector<std::string> args = input_handler.ReadParse(command_line, " ");
        
        if (!args.empty()) {
            
            // Vérification du mot-clé "IOStd" (args[0])
            if (args[0] == "IOStd") {
                InterpretCommand(args);
            } else {
                SyntaxError(args);
            }
        } else {
            // Ligne vide
            Print("ready.\n");
        }
    }
    
    //engine->stop();
    running = 0;
}


/*
 *******************************************************************************
 * InterpretCommand() : interpretation d'une commande reçue                  *
 *******************************************************************************
 */
void IOStdProtocol::InterpretCommand(const std::vector<std::string>& args)
{ 
    // args[0] est "IOStd", la commande réelle est args[1]
    if (args.size() < 2) {
        SyntaxError(args);
        return;
    }
    
    const std::string& command = args[1];

    if (command == "init") {
        engine->resume(); // engine->init(); // not implemented
        Print("ready.");
    }
    
    else if (command == "stop") {
        engine->stop_engine();
    }
    
    else if (command == "quit")  { // engine->free();	// not implemented
        engine->stop_engine();
        running = 0;
    }

    else if (command == "empty-hash")  {
        engine->resume();
        Print("ready.");
    }
        
    else if (command == "search-fixed-depth") {
        // La commande 'search' nécessite 5 arguments au total (IOStd search pos depth precision)
        // pos : O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X contient un delimitateur ' ' = traitement special
        if(args.size() == 6) {
            
            // Utilisation des chaînes C++
            const std::string& position  = args[2] + " " + args[3];

            // Conversion en int
            int depth = std::atoi(args[4].c_str());
            int precision = std::atoi(args[5].c_str());
            
            engine->get_move_fixed_depth(position, depth, precision);
        } else {
             SyntaxError(args);
        }
    }
 
    else if (command == "search-limited-time") {
        // La commande 'search' nécessite 5 arguments au total (IOStd search pos depth precision)
        // pos : O--OOOOX-OOOOOOXOOXXOOOXOOXOOOXXOOOOOOXX---OOOOX----O--X-------- X contient un delimitateur ' ' = traitement special
        if(args.size() == 5) {
            
            // Utilisation des chaînes C++
            const std::string& position  = args[2] + " " + args[3];

            // Conversion en int
            int time_remaining = std::atoi(args[4].c_str());
            
            engine->get_move_time_limited(position, time_remaining);
        } else {
             SyntaxError(args);
        }
    }

    else
        SyntaxError(args);
}

/*
 *******************************************************************************
 * SyntaxError() : affice les lexemes reçus en cas d'erreur de syntaxe       *
 *******************************************************************************
 */
void IOStdProtocol::SyntaxError(const std::vector<std::string>& args)
{
	
	Print("\nSYNTAX ERROR :\n");
    int i = 0;
	for (const std::string& token : args) { // Utilisation d'une boucle C++11 range-based
		std::ostringstream line;
		line  << "Token[" << i << "] = " << token << "\n"; // Ajout de \n dans le Print
		Print(line.str());
        i++;
	}
	Print("\n");
}


/*
 *******************************************************************************
 * Print() : ecrit sur la sortie standart de facon synchronisée              *
 * *
 *******************************************************************************
 */
// Conservation de la méthode Print inchangée
void IOStdProtocol::Print(const std::string msg) const {
    pthread_mutex_lock(&IOSync);
	std::cout << msg << std::endl;
    pthread_mutex_unlock(&IOSync);
}

// Assurez-vous d'ajouter le destructeur pour libérer le mutex

IOStdProtocol::~IOStdProtocol() {
    pthread_mutex_destroy(&IOSync);
}

