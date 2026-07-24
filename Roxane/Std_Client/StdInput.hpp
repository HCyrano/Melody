/*
 * Copyright (C) 2026  Bruno Causse Stéphane Nicolet
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

#ifndef STANDARD_INPUT_H
#define STANDARD_INPUT_H

#include <iostream> // Pour std::cin/std::cout (alternative à stdio.h)
#include <cstdio>   // Pour FILE*
#include <cstdlib>  // Pour malloc/free, si nécessaire
#include <cstring>  // Pour memcpy, strlen
#include <cerrno>   // Pour errno
#if defined(_MSC_VER) || defined(_WIN32)
    #include <io.h>       // Pour _read et _fileno sous Windows
#else
    #include <unistd.h>   // Pour read et fileno sous Linux/macOS
#endif
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
