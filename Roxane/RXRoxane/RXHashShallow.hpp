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

#ifndef RXHASHSHALLOW_HPP
#define RXHASHSHALLOW_HPP

#include "RXHashTable.hpp"
#include "RXTools.hpp"



class RXHashShallow {
    
    std::vector<RXHashEntry> table;
    unsigned long _maskTable;
    
    unsigned char date;
    

    public :
    
    RXHashShallow(unsigned int nBitsTable);
    ~RXHashShallow() {};
    
    void reset();
    
    void entry_prefetch(const unsigned long long hash_code) const;
    
    bool get(const RXBitBoard& board, RXHashValue& entry) const;
    bool get(const unsigned long long hash_code,const RXBitBoard& board, RXHashValue& entry) const;
    
    void update(const unsigned long long hash_code, const RXBitBoard& board,
                               const unsigned char depth, const int alpha, const int beta,
                               const int score, const char move);
    
    void new_search(const int n_empty);

};

inline void RXHashShallow::new_search(const int n_empty) {
    
    int stage = 2*(60-n_empty);
    
    if(stage>date)
        date = stage;
    
}

inline void RXHashShallow::entry_prefetch(const unsigned long long hash_code) const {
    const RXHashEntry* ptr = &(table[static_cast<unsigned int>(hash_code>>32) & _maskTable]);
    RX_PREFETCH(ptr);
}


// 1. La fonction "source" (avec les paramètres bruts)
inline bool RXHashShallow::get(const unsigned long long hash_code, const RXBitBoard& board, RXHashValue& hValue) const {

    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player ^ 1];

    const RXHashEntry& entry = table[static_cast<unsigned int>(hash_code>>32) & _maskTable];
    
    // ==========================================
    // CRITICAL SECTION
    // ==========================================

    bool found = false;
    entry.lock();
    
    if (P == entry.deepest.discs_P && O == entry.deepest.discs_O) {

        hValue.compact_2_wide(entry.deepest.packed);
        found = true;
        
    } else if (P == entry.newest.discs_P && O == entry.newest.discs_O) {

        hValue.compact_2_wide(entry.newest.packed);
        found = true;
    }

    entry.unlock();
    return found;
}

// 2. La fonction "wrapper" (qui délègue le travail)
RX_ALWAYS_INLINE bool RXHashShallow::get(const RXBitBoard& board, RXHashValue& hValue) const {
    // On calcule la hash_code et on appelle la version mutualisée
    const unsigned long long hash_code = board.hashcode();
    return get(hash_code, board, hValue);
}

                                  

#endif
