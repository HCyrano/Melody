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

#include <cmath>
#include <random>


#include "RXMove.hpp"
#include "RXConstantes.hpp"

/*
void RXMove::index_to_COsMove(int index, COsMove& move) {

	if (index == PASS) {
		move.fPass = true;
		move.row = -1;
		move.col = -1;
		
	} else {
		move.fPass = false;
		move.row = 7 - static_cast<int>(floor(index/8));
		move.col = 7 - index%8;
	}
	
	
}

std::string RXMove::COsMove_to_coord(COsMove& move) {

	if (move.fPass)
		return std::string("PA");
	 
	return (std::string(1, "ABCDEFGH"[move.col]) + std::string(1, "12345678"[move.row]));

}
*/

std::string RXMove::index_to_coord(int index) {

	if (index == PASS)
		return std::string("PA");

    if (index == NOMOVE)
        return std::string("--");

	return (std::string(1, "HGFEDCBA"[index%8]) + std::string(1, "87654321"[index/8]));

}

int RXMove::coord_to_index(std::string coord) {

	if( coord == "PA" || coord == "PASS")
		return PASS;
		
	return (('8' - coord[1])*8 + ('h' - std::tolower(coord[0])));
}

//static std::random_device rd;
static std::random_device& get_rd() {
    // L'instance est créée une seule fois, mais jamais détruite explicitement
    static std::random_device* rd = new std::random_device();
    return *rd;
}


unsigned long long RXMove::random_pick_bit_in_legalmoves(const unsigned long long legal_moves) {
    
    // Masques de zones (à initialiser une seule fois avec les bits correspondants)
    // Chaque bit à 1 représente une case RARE ou MEDIUM
    static const unsigned long long MASK_RARE   = 0x0042000000004200ULL; // B2, G2, B7 & G7
    static const unsigned long long MASK_MEDIUM = 0x4281000000008142ULL; // B1, G1, A2, H2, A7, H7, B8 & G8

    if (legal_moves == 0) return 0;

    std::vector<unsigned long long> moves;
    std::vector<int> weights;
    
    // On réserve un peu d'espace pour éviter trop de réallocations
    moves.reserve(std::bitset<64>(legal_moves).count());
    weights.reserve(moves.capacity());

    // Extraction des bits et attribution des poids
    unsigned long long temp_moves = legal_moves;
    while (temp_moves) {
        // Isoler le bit de poids faible (LSB)
        // Expression mathématique : bit = temp_moves & -temp_moves
        unsigned long long bit = temp_moves & -temp_moves;
        
        moves.push_back(bit);

        // Vérification de la zone via masques binaires (très rapide)
        if (bit & MASK_RARE) {
            weights.push_back(1);
        } else if (bit & MASK_MEDIUM) {
            weights.push_back(2);
        } else {
            weights.push_back(4);
        }

        // Supprimer le bit traité
        temp_moves ^= bit;
    }

    // Moteur aléatoire

    // Utilisation :
    auto seed = get_rd()();
    
    static std::mt19937 gen(seed);
    
    std::discrete_distribution<> d(weights.begin(), weights.end());
    
    return moves[d(gen)];
}



//debug
std::ostream& operator<<(std::ostream& os, RXMove* list) {

	for(RXMove* move = list->next; move != nullptr; move = move->next)
		os << RXMove::index_to_coord(move->position) << ' ';
		
	return os;
}
