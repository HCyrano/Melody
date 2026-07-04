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

#include <iostream> // pour std::cout eventuel
#include <sstream>
#include <algorithm>

#include "RXHashTable.hpp"
#include "RXEngine.hpp"
#include "RXSetting.hpp"

//RXHashValue::RXHashValue(unsigned long long packed) {
//
//    lower = static_cast<short>(packed & 0x000000000000FFFFULL);
//    packed >>= 16;
//    upper = static_cast<short>(packed & 0x000000000000FFFFULL);
//    packed >>= 16;
//    move = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//    packed >>= 8;
//    selectivity = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//    packed >>= 8;
//    depth = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//    packed >>= 8;
//    date = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//
//}

RXHashValue::RXHashValue(unsigned long long packed) {
    std::memcpy(this, &packed, sizeof(unsigned long long));
}


RXHashTable::RXHashTable(unsigned int nBitsTable) : table(0),  _shared(true) {

    _offsetTable[HASH_SHARED] = 0;
    _offsetTable[HASH_BLACK] = 0;
    _offsetTable[HASH_WHITE] = 1<<(nBitsTable-1);
    
    _maskTable[HASH_SHARED] = (1<<nBitsTable)-1;
    _maskTable[HASH_BLACK] = (1<<(nBitsTable-1))-1;
    _maskTable[HASH_WHITE] = (1<<(nBitsTable-1))-1;
    
    table.resize(1UL<<nBitsTable);
         
    date[0] = date[1] = 0;
    
    
}


void RXHashTable::shared(const bool flag) {
    _shared = flag;
    
    if (date[1]>date[0])
        date[0] = date[1];
    else
        date[1] = date[0];
        
}

bool RXHashTable::is_shared() const {
    return _shared;
}


void RXHashTable::reset() {
    _shared = true;

    // "Pour chaque entrée par référence dans la table"
    for (auto& entry : table) {
        
        entry.spin.store(0, std::memory_order_relaxed);

        entry.deepest.packed  = 0x0ULL;
        entry.deepest.discs_P = 0x0ULL;
        entry.deepest.discs_O = 0x0ULL;
        
        entry.newest.packed   = 0x0ULL;
        entry.newest.discs_P  = 0x0ULL;
        entry.newest.discs_O  = 0x0ULL;

    }

    date[BLACK] = date[WHITE] = 0;
}

void RXHashTable::update(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable,
                         const unsigned char selectivity, const unsigned char depth, const int alpha, const int beta, const int score, const char move) {

    const unsigned int base_date = date[_shared ? 0 : (type_hashtable == HASH_WHITE ? WHITE : BLACK)];
    unsigned int _date = base_date;
    
    if(alpha < score && score < beta)
        ++_date; // bonus pour score exact
    
    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player^1];
    
    RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
    
    // --- Acquisition du Lock ---
    entry.lock();
    
    RXHashValue deepest_val(entry.deepest.packed);
    const bool is_deepest = (P == entry.deepest.discs_P && O == entry.deepest.discs_O);

    if (is_deepest && depth == deepest_val.depth && selectivity == deepest_val.selectivity) {
        // Mise à jour de l'entrée la plus profonde existante
        if (score < beta && score < deepest_val.upper) deepest_val.upper = static_cast<short>(score);
        if (score > alpha) {
            deepest_val.move = move;
            if (score > deepest_val.lower) deepest_val.lower = static_cast<short>(score);
        }
        // Correction instabilité
        if (deepest_val.lower > deepest_val.upper) {
            deepest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            deepest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            deepest_val.move = move;
        } else if (deepest_val.lower == deepest_val.upper && _date == base_date) {
            _date++;
        }
        deepest_val.date = _date;
        entry.deepest.packed = deepest_val.wide_2_compact();
    }
    else {
        
        RXHashValue newest_val(entry.newest.packed);
        const bool is_newest  = (P == entry.newest.discs_P  && O == entry.newest.discs_O);

        
        if (is_newest && depth == newest_val.depth && selectivity == newest_val.selectivity) {
            // Mise à jour de l'entrée la plus récente existante
            if (score < beta && score < newest_val.upper) newest_val.upper = static_cast<short>(score);
            if (score > alpha) {
                newest_val.move = move;
                if (score > newest_val.lower) newest_val.lower = static_cast<short>(score);
            }
            // Correction instabilité
            if (newest_val.lower > newest_val.upper) {
                newest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
                newest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
                newest_val.move = move;
            }
            newest_val.date = _date;
            
            // Promotion : si newest devient plus "importante" que deepest, on swap
            if (newest_val.date > deepest_val.date || (newest_val.date == deepest_val.date && depth > deepest_val.depth)) {
                entry.newest = entry.deepest; // L'ancienne deepest descend en newest
                
                entry.deepest.discs_P = P;
                entry.deepest.discs_O = O;
                entry.deepest.packed = newest_val.wide_2_compact();
                
            } else {
                entry.newest.packed = newest_val.wide_2_compact();
            }
            
        } else if (is_deepest || deepest_val.date  < _date ||
                                (deepest_val.date == _date &&  (deepest_val.depth  < depth ||
                                                               (deepest_val.depth == depth && deepest_val.selectivity < selectivity)))) { // priority
            
            if(!is_deepest &&  (is_newest ||  newest_val.date  < deepest_val.date ||
                                             (newest_val.date == deepest_val.date && (newest_val.depth  < deepest_val.depth ||
                                                                                     (newest_val.depth == deepest_val.depth && newest_val.selectivity <= deepest_val.selectivity))))) {
                //copy
                entry.newest = entry.deepest;
            }
                        
            entry.deepest.discs_P = P;
            entry.deepest.discs_O = O;
            
            deepest_val.depth = depth;
            deepest_val.date = _date;
            deepest_val.selectivity = selectivity;
            deepest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            deepest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            deepest_val.move = move;
            
            entry.deepest.packed = deepest_val.wide_2_compact();
            
        } else {
            // On remplace juste la newest
            entry.newest.discs_P = P;
            entry.newest.discs_O = O;
            
            newest_val.depth = depth;
            newest_val.date = _date;
            newest_val.selectivity = selectivity;
            newest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            newest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            newest_val.move = move;
            
            entry.newest.packed = newest_val.wide_2_compact();
            
        }
    }

    // --- Relâchement du Lock ---
    entry.unlock();
}



void RXHashTable::update(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable,
                         const unsigned char selectivity, const unsigned char depth, const int alpha, const int score, const char move) {

    const unsigned int base_date = date[_shared ? 0 : (type_hashtable == HASH_WHITE ? WHITE : BLACK)];
    unsigned int current_date = base_date;
    
    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player^1];

    RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code >> 32) & _maskTable[type_hashtable])];

    // --- Acquisition du Lock ---
    entry.lock();

    RXHashValue deepest_val(entry.deepest.packed);
    const bool is_deepest = (P == entry.deepest.discs_P && O == entry.deepest.discs_O);

    if (is_deepest && depth == deepest_val.depth && selectivity == deepest_val.selectivity) {
        if (score > alpha) {
            deepest_val.move = move;
            if (score > deepest_val.lower) deepest_val.lower = static_cast<short>(score);
        } else {
            if (score < deepest_val.upper) deepest_val.upper = static_cast<short>(score);
        }
        
        // Correction instabilité
        if (deepest_val.lower > deepest_val.upper) {
            if (score > alpha) {
                deepest_val.lower = static_cast<short>(score);
                deepest_val.upper = (short)MAX_SCORE;
            } else {
                deepest_val.lower = (short)-MAX_SCORE;
                deepest_val.upper = static_cast<short>(score);
            }
            deepest_val.move = move;
        }
        deepest_val.date = current_date;
        entry.deepest.packed = deepest_val.wide_2_compact();
    }
    else {

        RXHashValue newest_val(entry.newest.packed);
        const bool is_newest  = (P == entry.newest.discs_P  && O == entry.newest.discs_O);

        if (is_newest && depth == newest_val.depth && selectivity == newest_val.selectivity) {
            
            if (score > alpha) {
                newest_val.move = move;
                if (score > newest_val.lower) newest_val.lower = static_cast<short>(score);
            } else {
                if (score < newest_val.upper) newest_val.upper = static_cast<short>(score);
            }
            
            if (newest_val.lower > newest_val.upper) {
                if (score > alpha) {
                    newest_val.lower = static_cast<short>(score);
                    newest_val.upper = (short)MAX_SCORE;
                } else {
                    newest_val.lower = (short)-MAX_SCORE;
                    newest_val.upper = static_cast<short>(score);
                }
                newest_val.move = move;
            }
            newest_val.date = current_date;
            
            // Promotion vers deepest
            if (current_date > deepest_val.date || (current_date == deepest_val.date && depth > deepest_val.depth)) {
                entry.newest = entry.deepest;
                
                entry.deepest.discs_P = P;
                entry.deepest.discs_O = O;
                entry.deepest.packed = newest_val.wide_2_compact();
                
                
            } else {
                entry.newest.packed = newest_val.wide_2_compact();
            }
            
        } else if (is_deepest || deepest_val.date  < current_date ||
                                (deepest_val.date == current_date && (deepest_val.depth  < depth ||
                                                                     (deepest_val.depth == depth && deepest_val.selectivity < selectivity)))) { // priority
            
            if(!is_deepest &&  (is_newest ||  newest_val.date  < deepest_val.date ||
                                             (newest_val.date == deepest_val.date && (newest_val.depth  < deepest_val.depth ||
                                                                                     (newest_val.depth == deepest_val.depth && newest_val.selectivity <= deepest_val.selectivity))))) {
                //copy
                entry.newest = entry.deepest;
            }
        
            
            entry.deepest.discs_P = P;
            entry.deepest.discs_O = O;
            
            deepest_val.depth = depth;
            deepest_val.date = current_date;
            deepest_val.selectivity = selectivity;
            deepest_val.move = move;

            if (score > alpha) {
                deepest_val.lower = static_cast<short>(score);
                deepest_val.upper = (short)MAX_SCORE;
            } else {
                deepest_val.lower = (short)-MAX_SCORE;
                deepest_val.upper = static_cast<short>(score);
            }
            entry.deepest.packed = deepest_val.wide_2_compact();
            

            
        } else {
            entry.newest.discs_P = P;
            entry.newest.discs_O = O;
            
            newest_val.depth = depth;
            newest_val.date = current_date;
            newest_val.selectivity = selectivity;
            newest_val.move = move;

            if (score > alpha) {
                newest_val.lower = static_cast<short>(score);
                newest_val.upper = (short)MAX_SCORE;
            } else {
                newest_val.lower = (short)-MAX_SCORE;
                newest_val.upper = static_cast<short>(score);
            }
            entry.newest.packed = newest_val.wide_2_compact();
        }
    }

    // --- Libération du Lock ---
    entry.unlock();
}




/* ************************************* Attention *************************************/
/*         ces methodes sont utilisées avant ou aprés une recherche JAMAIS pendant     */
/* ************************************* Attention *************************************/


std::string RXHashTable::line2String(RXBitBoard& board, const int n_moves, const t_hash type_hashtable) const {
    
    std::vector<unsigned char> pv;
    mainVariation(pv, board, type_hashtable, n_moves);
    
    std::ostringstream buffer;
    bool player = false;
    for(auto it : pv) {

        std::string coord = RXMove::index_to_coord(it);
        if(player)
            std::transform(coord.begin(), coord.end(), coord.begin(), ::tolower);
        buffer << coord << ' ';

        player = !player;
        
    }
    
    return buffer.str();
    
}


void RXHashTable::mainVariation(std::vector<unsigned char>& pv, RXBitBoard& board, const t_hash type_hashtable, const int n_moves) const {
    
    if(n_moves <= 0) return;
    
    RXHashValue entry;
    if(board.n_empty >= RXEngine::EG_MEDIUM_HI_TO_LOW && get(board, type_hashtable, entry) && entry.move != NOMOVE) {
        
            
            pv.push_back(entry.move);
            
            if(entry.move == PASS) {
                board.do_pass();
                mainVariation(pv, board, type_hashtable, n_moves - 1);
                board.do_pass();
            } else {
                RXMove local_move; // Local pour thread-safety
                board.generate_flips(entry.move, local_move);
                board.do_move(local_move);
                mainVariation(pv, board, type_hashtable, n_moves - 1);
                board.undo_move(local_move);
            }

    } else {
        pv.push_back(NOMOVE);
        mainVariation(pv, board, type_hashtable, n_moves-1);
    }

}

bool RXHashTable::get_record(const RXBitBoard& board, const t_hash type_hashtable, RXHashRecord& record) const {
    
    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player ^ 1];
    const unsigned long long hash_code = board.hashcode();

    
    const RXHashEntry& entry = table[_offsetTable[type_hashtable] |(static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];

    // ==========================================
    // CRITICAL SECTION
    // ==========================================

    bool found = false;
    entry.lock();
    
    if (P == entry.deepest.discs_P && O == entry.deepest.discs_O) {
    
        record = entry.deepest;
        found = true;
        
    } else if (P == entry.newest.discs_P && O == entry.newest.discs_O) {
    
        record = entry.newest;
        found = true;
    }

    entry.unlock();
    return found;
    
    
}


/*
 copie la pv d'une hash SHARED vers la hash HASH_COLOR
 */
void RXHashTable::copyPV_shared_to_color(RXBitBoard& board, const int color) {


    if(color == BLACK) {
        copyPV(board, HASH_SHARED, HASH_BLACK);
    } else { //player == WHITE
        copyPV(board, HASH_SHARED, HASH_WHITE);
    }


}

/*
 copie la pv d'une hash HASH_COLOR vers la hash SHARED
 */
void RXHashTable::copyPV_color_to_shared (RXBitBoard& board, const int color) {

    
    if(color == BLACK) {
        copyPV(board, HASH_BLACK, HASH_SHARED);
    } else { //player == WHITE
        copyPV(board, HASH_WHITE, HASH_SHARED);
    }
    

}

void RXHashTable::copyPV(RXBitBoard& board, const t_hash from_hashtable, const t_hash to_hashtable, const bool passed) {
    
    RXHashRecord from_Record;
    // On utilise la version sécurisée de get_record (qui copie les données sous lock)
    if (get_record(board, from_hashtable, from_Record)) {
        
        int square = from_Record.get_move();
        
        if (square != NOMOVE) {
            
            // 1. Calcul de l'index de destination
            const unsigned long long hash_code = board.hashcode();
            const uint32_t idx = _offsetTable[to_hashtable] | (static_cast<unsigned int>(hash_code >> 32) & _maskTable[to_hashtable]);
            RXHashEntry& entry_to = table[idx];
            
            // 2. Verrouillage de l'entrée de destination
            entry_to.lock();
            
            // 3. Copie intégrale (on vérifie si c'est déjà la même pour éviter l'écriture inutile)
            if (entry_to.deepest.discs_P != from_Record.discs_P || entry_to.deepest.discs_O != from_Record.discs_O) {
                entry_to.deepest.discs_P = from_Record.discs_P;
                entry_to.deepest.discs_O = from_Record.discs_O;
                entry_to.deepest.packed  = from_Record.packed;
            }
            
            // 4. Relâchement du lock
            entry_to.unlock();
            
            // 5. Récursion
            if (square == PASS) {
                if(!passed){
                    board.do_pass();
                    copyPV(board, from_hashtable, to_hashtable, true);
                    board.do_pass();
                }
            } else {
                // IMPORTANT : Utiliser un objet local pour éviter les conflits entre threads
                RXMove local_move;
                board.generate_flips(square, local_move);
                board.do_move(local_move);
                copyPV(board, from_hashtable, to_hashtable, false);
                board.undo_move(local_move);
            }
        }
    }
}

/*
 copie la pv d'une hash secondaire (main_PV ou expected_PV) dans la hash principale
 */
void RXHashTable::copyPV(RXHashTable* from_hash, const t_hash from_type_hash, RXBitBoard& board, const t_hash to_type_hash, const bool passed) {
    
    RXHashRecord from_Record;
    // On utilise la version sécurisée de get_record (qui copie les données sous lock)
    if (from_hash->get_record(board, from_type_hash, from_Record)) {
        
        int square = from_Record.get_move();
        
        if (square != NOMOVE) {
            
            // 1. Calcul de l'index de destination
            const unsigned long long hash_code = board.hashcode();
            const uint32_t idx = _offsetTable[to_type_hash] | (static_cast<unsigned int>(hash_code >> 32) & _maskTable[to_type_hash]);
            RXHashEntry& entry_to = table[idx];
            
            // 2. Verrouillage de l'entrée de destination
            entry_to.lock();
            
            // 3. Copie intégrale (on vérifie si c'est déjà la même pour éviter l'écriture inutile)
            if (entry_to.deepest.discs_P != from_Record.discs_P || entry_to.deepest.discs_O != from_Record.discs_O) {
                entry_to.deepest.discs_P = from_Record.discs_P;
                entry_to.deepest.discs_O = from_Record.discs_O;
                entry_to.deepest.packed  = from_Record.packed;
            }
            
            // 4. Relâchement du lock
            entry_to.unlock();
            
            // 5. Récursion
            if (square == PASS) {
                if(!passed) {
                    board.do_pass();
                    copyPV(from_hash, from_type_hash, board,  to_type_hash, true);
                    board.do_pass();
                }
            } else {
                // IMPORTANT : Utiliser un objet local pour éviter les conflits entre threads
                RXMove local_move;
                board.generate_flips(square, local_move);
                board.do_move(local_move);
                copyPV(from_hash, from_type_hash, board,  to_type_hash, false);
                board.undo_move(local_move);
            }
        }
    }
}

/*
 merge la PV des deux Hash BLACK & WHITE dans la hash SHARED
 */
void RXHashTable::mergePV(RXBitBoard& board, const bool passed) {
    
    RXHashRecord from_Black, from_White;
    bool hasBlack = get_record(board, HASH_BLACK, from_Black);
    bool hasWhite = get_record(board, HASH_WHITE, from_White);
    
    RXHashRecord* chosen = nullptr;
    if (hasBlack && from_Black.get_move() != NOMOVE) {
        chosen = &from_Black;
        if (hasWhite && from_White.get_move() != NOMOVE) {
            if (from_White.get_depth() > from_Black.get_depth()) chosen = &from_White;
            else if (from_White.get_depth() == from_Black.get_depth() && from_White.get_selectivity() > from_Black.get_selectivity())
                chosen = &from_White;
        }
    } else if (hasWhite && from_White.get_move() != NOMOVE) {
        chosen = &from_White;
    }

    if (chosen) {
        const unsigned long long hash_code = board.hashcode();
        RXHashEntry& entry_to = table[_offsetTable[HASH_SHARED] | (static_cast<unsigned int>(hash_code >> 32) & _maskTable[HASH_SHARED])];
        
        entry_to.lock();
        entry_to.deepest = *chosen; // Copie sécurisée sous lock
        entry_to.unlock();

        int square = chosen->get_move();
        if (square == PASS) {
            if(!passed) {
                board.do_pass();
                mergePV(board, true);
                board.do_pass();
            }
        } else {
            RXMove local_move; // Utilisation d'un move LOCAL (très important)
            board.generate_flips(square, local_move);
            board.do_move(local_move);
            mergePV(board, false);
            board.undo_move(local_move);
        }
    }
}

/*
 Protection de la PV copy en deepest avec une date+1
 */
void RXHashTable::protectPV(RXBitBoard& board) {

    t_hash type_hashtable = HASH_SHARED;
        
    if(!_shared) {
        if (board.player == BLACK)
            type_hashtable = HASH_BLACK;
        else
            type_hashtable = HASH_WHITE;
            
    }
        
    protectPV(board, type_hashtable);
    
}


void RXHashTable::protectPV(RXBitBoard& board, const t_hash type_hashtable, const bool passed) {
    
    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player ^ 1];
    const unsigned long long hash_code = board.hashcode();

    RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
    
    // --- Acquisition du LOCK ---
    entry.lock();
    
    bool found_in_newest = (P == entry.newest.discs_P && O == entry.newest.discs_O);
    bool found_in_deepest = (P == entry.deepest.discs_P && O == entry.deepest.discs_O);

    if(found_in_newest) {
        // Swap complet des records (24 octets chacun)
        RXHashRecord temp = entry.deepest;
        entry.deepest = entry.newest;
        entry.newest = temp;
        found_in_deepest = true; // Maintenant c'est dans deepest
    }
    
    int square = NOMOVE;
    if(found_in_deepest) {
        RXHashValue hValue(entry.deepest.packed);
        square = hValue.move;
        
        if(square != NOMOVE) {
            // Mise à jour de la date pour protéger l'entrée contre le remplacement
            hValue.date = date[_shared ? 0 : (type_hashtable == HASH_WHITE ? WHITE : BLACK)] + 1;
            entry.deepest.packed = hValue.wide_2_compact();
        }
    }
    
    // --- Relâchement du LOCK ---
    entry.unlock();
    
    // --- Récursion (Hors Lock pour ne pas bloquer les autres threads trop longtemps) ---
    if(square != NOMOVE) {
        if(square == PASS) {
            if(!passed) {
                board.do_pass();
                protectPV(board, type_hashtable, true);
                board.do_pass();
            }
        } else {
            RXMove local_move; // Sur la pile pour la sécurité multithread
            board.generate_flips(square, local_move);
            board.do_move(local_move);
            protectPV(board, type_hashtable, false);
            board.undo_move(local_move);
        }
    }
}

