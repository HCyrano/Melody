/*
 *  StdInput.cpp
 *
 *
 *  Created by bruno Causse on 10/12/25.
 *
 */

#include "StdInput.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <sstream> // Optionnel, pour d'autres méthodes de découpage


// Constructeur
StdInput::StdInput() {
    // Initialisation
    ReadClear();
    InitializeArguments();
    initialized_ = true;
    // On pourrait initialiser input_stream_ ici si on voulait un comportement différent
}

// Destructeur (pour libérer la mémoire allouée dans InitializeArguments)
StdInput::~StdInput() {
    for (int i = 0; i < 512; i++) {
        free(args_[i]);
    }
}

// Initialisation des arguments (extrait de SetReadStream)
void StdInput::InitializeArguments() {
    for (int i = 0; i < 512; i++) {
        // En C++, on pourrait préférer new char[128] mais free() est mieux pour un code mixte
        args_[i] = (char *) malloc(128); 
        if (args_[i] == nullptr) {
             // Gérer l'erreur d'allocation mémoire
             std::cerr << "Erreur d'allocation pour args[" << i << "]" << std::endl;
             exit(EXIT_FAILURE);
        }
    }
}

/*
 *******************************************************************************
 * SetReadStream() définit le flux de lecture.
 *******************************************************************************
 */
void StdInput::SetReadStream(FILE *stream) {
    if (!initialized_) {
        // En C++, l'initialisation est faite par le constructeur.
        // On pourrait appeler InitializeArguments() ici si le constructeur ne l'a pas fait.
    }
    input_stream_ = stream;
    ReadClear();
}


/*
 *******************************************************************************
 * ReadClear() efface le tampon d'entrée.
 *******************************************************************************
 */
void StdInput::ReadClear() {
    cmd_buffer_[0] = 0;
}

/*
 *******************************************************************************
 * ReadInput() lit les données du flux d'entrée.
 *******************************************************************************
 */
int StdInput::ReadInput() {
    char buffer[BUFFER_SIZE];
    char *end;
    long long bytes;
    
    // Utilisation de l'opérateur de classe sur le membre
    do
        bytes = read(fileno(input_stream_), buffer, BUFFER_SIZE);
    while (bytes < 0 && errno == EINTR);
    
    
    if (bytes == 0) {
        
        if (input_stream_ != stdin)
            fclose(input_stream_);
        input_stream_ = stdin;
        return (0);
        
    } else if (bytes < 0) {
        
        std::cerr << "ERROR! input I/O stream is unreadable, exiting." << std::endl;
        return (-1); // Ajout d'un retour explicite en cas d'erreur
    }
    
    end = cmd_buffer_ + strlen(cmd_buffer_);
    
    // Vérification de la taille avant de copier
    if (end - cmd_buffer_ + bytes < BUFFER_SIZE) {
        memcpy(end, buffer, bytes);
        *(end + bytes) = 0;
    } else {
        // Gérer le dépassement de tampon
        std::cerr << "Warning: Command buffer overflow prevented." << std::endl;
        return (-1);
    }
    
    return (1);
}

/*
 *******************************************************************************
 * Read() (Version C++ Moderne)
 * Lit une ligne complète du tampon de commande et la retourne sous forme de 
 * std::string. Retourne une chaîne vide en cas d'erreur ou de fin de fichier.
 *******************************************************************************
 */
std::string StdInput::Read() {
    
    char *eol, *ret;
    bool readdata;
    
    // Logique de lecture jusqu'à ce qu'on trouve un '\n'
    if (!strchr(cmd_buffer_, '\n')) {
        while (!strchr(cmd_buffer_, '\n')) {
            readdata = ReadInput();
            if (!readdata) {
                // Retourne une chaîne vide en cas d'erreur ou EOF
                return ""; 
            }
        }
    }
    
    // 1. Trouver la fin de ligne
    eol = strchr(cmd_buffer_, '\n');
    
    // Calculer la taille de la ligne (jusqu'à l'indice de eol)
    size_t line_length = eol - cmd_buffer_;

    // 2. Extraire la ligne dans un std::string
    // Le constructeur de std::string peut prendre un char* et une longueur
    std::string line(cmd_buffer_, line_length);

    // 3. Supprimer le '\r' s'il existe (le remplacer par un espace n'est plus nécessaire 
    // car nous allons le supprimer dans l'étape suivante, mais si la logique d'origine
    // le voulait, on le ferait ici)
    ret = strchr(cmd_buffer_, '\r');
    if (ret) {
        // En C++, on peut supprimer le caractère 'r' de la string extraite
        size_t r_pos = line.find('\r');
        if (r_pos != std::string::npos) {
            line.erase(r_pos, 1);
        }
    }
    
    // 4. Déplacer le reste du cmd_buffer_ au début
    // La position de départ pour le reste du buffer est eol + 1 (après le '\n')
    memmove(cmd_buffer_, eol + 1, strlen(eol + 1) + 1);
    
    return line;
}

/*
 *******************************************************************************
 * ReadParse() (Version C++ Moderne)
 * Découpe une chaîne d'entrée en tokens et retourne un vecteur de chaînes.
 *******************************************************************************
 */
std::vector<std::string> StdInput::ReadParse(const std::string& input, const std::string& delims) {
    std::vector<std::string> tokens;
    
    // Si l'entrée est vide, on retourne un vecteur vide
    if (input.empty()) {
        return tokens;
    }
    
    // Utilisation d'indices de position pour le découpage
    size_t start = 0;
    size_t end = input.find_first_of(delims, start);

    while (end != std::string::npos) {
        // Extrait le sous-segment (le jeton)
        std::string token = input.substr(start, end - start);
        
        // N'ajoute pas le jeton s'il est vide (ce qui arrive avec des délimiteurs consécutifs)
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        // Avance le point de départ après les délimiteurs trouvés
        start = input.find_first_not_of(delims, end);
        if (start == std::string::npos) {
            break; // Plus de jetons valides
        }
        
        // Trouve le prochain délimiteur
        end = input.find_first_of(delims, start);
    }

    // Gère le dernier jeton (s'il n'est pas suivi d'un délimiteur)
    if (start != std::string::npos) {
        std::string last_token = input.substr(start);
        if (!last_token.empty()) {
            tokens.push_back(last_token);
        }
    }
    
    return tokens;
}
