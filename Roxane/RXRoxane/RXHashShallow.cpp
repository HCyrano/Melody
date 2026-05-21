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

#include "RXHashShallow.hpp"


RXHashShallow::RXHashShallow(unsigned int nBitsTable) : table(0), date(0) {
    
    _maskTable = (1UL<<nBitsTable) -1;
    table.resize(1UL<<nBitsTable);
    
}



void RXHashShallow::reset() {
    
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

    date = 0;
}


void RXHashShallow::update(const unsigned long long hash_code, const RXBitBoard& board,
                            const unsigned char depth, const int alpha, const int beta,
                            const int score, const char move) {
    
    unsigned int _date = date;
    if (alpha < score && score < beta)
        ++_date; // bonus pour score exact


    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player^1];
    
    RXHashEntry& entry = table[static_cast<unsigned int>(hash_code>>32) & _maskTable];


    entry.lock();

    RXHashValue deepest_val(entry.deepest.packed);
    const bool is_deepest = (P == entry.deepest.discs_P && O == entry.deepest.discs_O);

    if (is_deepest && depth == deepest_val.depth) {
        // Mise à jour entrée profonde existante
        if (score < beta && score < deepest_val.upper)
            deepest_val.upper = static_cast<short>(score);
        if (score > alpha) {
            deepest_val.move = move;
            if (score > deepest_val.lower)
                deepest_val.lower = static_cast<short>(score);
        }
        // Correction instabilité
        if (deepest_val.lower > deepest_val.upper) {
            deepest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            deepest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            deepest_val.move = move;
        } else if (deepest_val.lower == deepest_val.upper && _date == date) {
            _date++;
        }
        deepest_val.date = _date;
        entry.deepest.packed = deepest_val.wide_2_compact();

    } else {

        RXHashValue newest_val(entry.newest.packed);
        const bool is_newest = (P == entry.newest.discs_P && O == entry.newest.discs_O);

        if (is_newest && depth == newest_val.depth) {
            // Mise à jour entrée récente existante
            if (score < beta && score < newest_val.upper)
                newest_val.upper = static_cast<short>(score);
            if (score > alpha) {
                newest_val.move = move;
                if (score > newest_val.lower)
                    newest_val.lower = static_cast<short>(score);
            }
            // Correction instabilité
            if (newest_val.lower > newest_val.upper) {
                newest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
                newest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
                newest_val.move = move;
            }
            newest_val.date = _date;

            // Promotion si newest devient plus importante que deepest
            if (newest_val.date > deepest_val.date ||
               (newest_val.date == deepest_val.date && depth > deepest_val.depth)) {
                entry.newest  = entry.deepest;
                entry.deepest.discs_P = P;
                entry.deepest.discs_O = O;
                entry.deepest.packed  = newest_val.wide_2_compact();
            } else {
                entry.newest.packed = newest_val.wide_2_compact();
            }

        } else if (is_deepest || deepest_val.date  < _date ||
                                (deepest_val.date == _date && deepest_val.depth < depth)) {
            // Remplacement deepest
            if (!is_deepest && (is_newest || newest_val.date  < deepest_val.date ||
                                            (newest_val.date == deepest_val.date && newest_val.depth <= deepest_val.depth))) {
                entry.newest = entry.deepest;
            }

            entry.deepest.discs_P = P;
            entry.deepest.discs_O = O;
            deepest_val.depth = depth;
            deepest_val.date  = _date;
            deepest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            deepest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            deepest_val.move  = move;
            entry.deepest.packed = deepest_val.wide_2_compact();

        } else {
            // Remplacement newest
            entry.newest.discs_P = P;
            entry.newest.discs_O = O;
            newest_val.depth = depth;
            newest_val.date  = _date;
            newest_val.lower = (score > alpha) ? static_cast<short>(score) : (short)-MAX_SCORE;
            newest_val.upper = (score < beta)  ? static_cast<short>(score) : (short)MAX_SCORE;
            newest_val.move  = move;
            entry.newest.packed = newest_val.wide_2_compact();
        }
    }

    entry.unlock();
}



