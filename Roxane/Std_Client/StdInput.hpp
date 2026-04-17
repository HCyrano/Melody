/*
 *  StdInput.cpp
 *
 *  Input/output routines for the engine and the IOStd protocol.
 *
 *  Created by Bruno Causse on 10/12/25. (base Stephane Nicolet)
 *
 *  Most of the code below in taken from Robert Hyatt's chess
 *  program "crafty" (thanks). See http://www.craftychess.com/
 *  for the crafty source code.
 *
 *
 */

#ifndef STANDARD_INPUT_H
#define STANDARD_INPUT_H

#include <iostream> // Pour std::cin/std::cout (alternative à stdio.h)
#include <cstdio>   // Pour FILE*
#include <cstdlib>  // Pour malloc/free, si nécessaire
#include <cstring>  // Pour memcpy, strlen
#include <cerrno>   // Pour errno
#include <unistd.h> // Pour read
#include <string>   // La bibliothèque string de C++ est souvent meilleure
#include <vector>   // Pour gérer les arguments de manière dynamique

// On peut utiliser la constante du code C
#define BUFFER_SIZE (1024 * 16)

class StdInput {
public:
    // Constructeur et Destructeur
    StdInput();
    ~StdInput();

    // Fonctions/Méthodes publiques
    void SetReadStream(FILE *stream);
    void ReadClear();
    int ReadInput();

	// Nouvelle version C++
	std::string Read();
    
    // Une version plus C++ pour ReadParse :
    std::vector<std::string> ReadParse(const std::string& input, const std::string& delims);

private:
    // Membres de la classe
    FILE *input_stream_ = stdin;
    char cmd_buffer_[BUFFER_SIZE];
    char line_buffer_[BUFFER_SIZE];
    char *args_[512]; // Nous conservons le tableau de char* du code C pour l'instant
    
    // État d'initialisation
    bool initialized_ = false;

    // Fonction interne pour la lecture (similaire à SetReadStream)
    void InitializeArguments();
};

#endif  // STANDARD_INPUT_H
