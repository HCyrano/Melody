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


#ifndef RXHASHTABLE_HPP
#define RXHASHTABLE_HPP

#include <string>
#include <vector>
#include <atomic>
#include <cassert>


#include "RXConstantes.hpp"
#include "RXBitBoard.hpp"
#include "RXMove.hpp"

#ifdef __ARM_ACLE
#include "arm_acle.h"
#else
#include <immintrin.h> // Indispensable pour _mm_pause()
#endif

class RXEngine;

//class RXHashValue {
//
//public:
//
//    short lower;               // 16 bits
//    short upper;               // 16 bits
//    unsigned char move;        //  8 bits
//    unsigned char selectivity; //  8 bits
//    unsigned char depth;       //  8 bits
//    unsigned char date;        //  8 bits
//    // = 64 bits
//
//    RXHashValue(const unsigned long long packed = 0);
//
//    inline unsigned long long wide_2_compact() const {
//
//        unsigned long long packed = date;
//        packed <<= 8;
//        packed |= depth;
//        packed <<= 8;
//        packed |= selectivity;
//        packed <<= 8;
//        packed |= move;
//        packed <<= 16;
//        packed |= (static_cast<long long>(upper) & 0x000000000000FFFFULL);
//        packed <<= 16;
//        packed |= (static_cast<long long>(lower) & 0x000000000000FFFFULL);
//
//        return packed;
//    }
//
//    inline void compact_2_wide(unsigned long long packed) {
//
//        lower = static_cast<short>(packed & 0x000000000000FFFFULL);
//        packed >>= 16;
//        upper = static_cast<short>(packed & 0x000000000000FFFFULL);
//        packed >>= 16;
//        move = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//        packed >>= 8;
//        selectivity = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//        packed >>= 8;
//        depth = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//        packed >>= 8;
//        date = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
//
//    }
//
//};


// ⚠️  AVERTISSEMENT : ordre des membres critiques
//
// Les fonctions wide_2_compact() / compact_2_wide() reposent sur un memcpy
// direct entre le layout mémoire de la classe et un uint64_t.
//
// Sur x86 (little-endian), le layout est :
//
//   offset 0 → lower       → bits  0-15  du uint64
//   offset 2 → upper       → bits 16-31  du uint64
//   offset 4 → move        → bits 32-39  du uint64
//   offset 5 → selectivity → bits 40-47  du uint64
//   offset 6 → depth       → bits 48-55  du uint64
//   offset 7 → date        → bits 56-63  du uint64
//
// ⛔ Ne jamais :
//   - Réordonner les membres
//   - Ajouter un membre ou du padding
//   - Porter ce code sur une architecture big-endian sans adaptation
//
// La version équivalente "lisible" utilisant des shifts explicites
// est commentée, et fait foi en cas de doute sur le format du uint64.

class RXHashValue {
public:
    short lower;               // offset 0 → bits  0-15
    short upper;               // offset 2 → bits 16-31
    unsigned char move;        // offset 4 → bits 32-39
    unsigned char selectivity; // offset 5 → bits 40-47
    unsigned char depth;       // offset 6 → bits 48-55
    unsigned char date;        // offset 7 → bits 56-63


    RXHashValue(const uint64_t packed = 0);

    inline uint64_t wide_2_compact() const {
        uint64_t packed;
        std::memcpy(&packed, this, sizeof(uint64_t));
        return packed;
    }

    inline void compact_2_wide(uint64_t packed) {
        std::memcpy(this, &packed, sizeof(uint64_t));
    }
};

static_assert(std::is_trivially_copyable_v<RXHashValue>,
    "RXHashValue doit rester trivialement copiable (memcpy layout)");

static_assert(sizeof(RXHashValue) == sizeof(unsigned long long),
    "RXHashValue doit faire exactement 8 octets");

class RXHashRecord {
    
    public :
        unsigned long long discs_P;
        unsigned long long discs_O;

        unsigned long long packed;
    
    unsigned char get_move() const {
        return static_cast<unsigned char>((packed>>32) & 0x00000000000000FFULL);
    }

    unsigned char get_selectivity() const {
        return static_cast<unsigned char>((packed>>40) & 0x00000000000000FFULL);
    }
    
    unsigned char get_depth() const {
        return static_cast<unsigned char>((packed>>48) & 0x00000000000000FFULL);
    }
    
};

// Shelved for now; seqlock implementation on ARM is more complex
static_assert(std::is_trivially_copyable_v<RXHashRecord>,
    "RXHashRecord doit rester trivialement copiable (seqlock)");

// Entry de 64 octets (1 Cache Line)
class alignas(64) RXHashEntry {
    
    public :
        mutable std::atomic<int> spin{0};
        
        RXHashRecord deepest;
        RXHashRecord newest;
                
        // 1. Default constructor (RAII)
        RXHashEntry() : deepest(), newest() {
            spin.store(0, std::memory_order_relaxed);
        }

        // 2. Required by std::vector, never called at runtime
        RXHashEntry(const RXHashEntry& other) {
            assert(false && "RXHashEntry ne doit pas être copié à runtime");

            spin.store(0, std::memory_order_relaxed); // Le verrou copié naît libre !
            deepest = other.deepest;
            newest = other.newest;
        }

        // 3. Required by std::vector, never called at runtime
        RXHashEntry& operator=(const RXHashEntry& other) = delete;
    
//        // 3. Opérateur d'assignation explicite
//        RXHashEntry& operator=(const RXHashEntry& other) {
//            if (this != &other) {
//                spin.store(0, std::memory_order_relaxed); // On réinitialise le verrou
//                deepest = other.deepest;
//                newest = other.newest;
//            }
//            return *this;
//        }
    
    
//    void lock() const {
//        for (;;) {
//            // Tente de prendre le lock (Ecriture atomique)
//            if (spin.exchange(1, std::memory_order_acquire) == 0) {
//                return;
//            }
//
//            // Si échoué, boucle de lecture seule (ne pollue pas le bus mémoire)
//            while (spin.load(std::memory_order_relaxed) == 1) {
//#if defined(__x86_64__) || defined(_M_X64)
//                _mm_pause(); // Spécifique x86
//#elif defined(__aarch64__) || defined(__arm__)
//                asm volatile("yield"); // Spécifique ARM (AArch64)
//#endif
//            }
//        }
//    }

    
    void lock() const noexcept {
        for (;;) {
            // Relaxed load : avoid costly RMW exchange while lock is held
            if (spin.load(std::memory_order_relaxed)) {
                do {
    #if defined(__x86_64__) || defined(_M_X64)
                    _mm_pause();
    #elif defined(__aarch64__) || defined(__arm__)
                    // "memory" clobber : mandatory compiler barrier
                    asm volatile("yield" ::: "memory");
    #endif
                } while (spin.load(std::memory_order_relaxed));
            }

            // acquire exchange : synchronizes with the release in unlock()
            if (spin.exchange(1, std::memory_order_acquire) == 0) {
                return;
            }
        }
    }
    
    void unlock() const {
        spin.store(0, std::memory_order_release);
    }
};


class RXHashTable {
    
    public:
    enum t_hash {HASH_SHARED, HASH_BLACK, HASH_WHITE};

    
    private :
    
    mutable std::vector<RXHashEntry> table;
    unsigned int _offsetTable[3];
    unsigned int _maskTable[3];
    
    unsigned char date[2];
    
    bool _shared;

    bool get_record(const RXBitBoard& board, const t_hash type_hashtable, RXHashRecord& record) const;


    void copyPV(RXBitBoard& board, const t_hash from_hashtable, const t_hash to_hashtable, const bool passed = false);
    void protectPV(RXBitBoard& board, const t_hash type_hashtable, const bool passed = false);
 
    
    public :
    
    RXHashTable(unsigned int nBTable);
    ~RXHashTable() {};
    
    void reset();
    
    void shared(const bool flag);
    
    bool is_shared() const;
    
    void entry_prefetch(const unsigned long long hash_code, const t_hash type_hashtable) const;
    
    bool get(const unsigned long long hash_code, const unsigned long long P, const unsigned long long O, const t_hash type_hashtable, RXHashValue& hValue) const;
    bool get(const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& entry) const;
    bool get(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& entry) const;
    
    
    void copyPV(RXHashTable* from_hashtable, const t_hash from_type_hash, RXBitBoard&board, const t_hash to_type_hash, const bool passed = false);
    
    void copyPV_shared_to_color(RXBitBoard& board, const int color);
    void copyPV_color_to_shared(RXBitBoard& board, const int color);
    void mergePV(RXBitBoard& board, const bool passed = false);
    void protectPV(RXBitBoard& board);
    
    void update(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable,
                const unsigned char selectivity, const unsigned char depth, const int alpha, const int beta, const int score, const char move);
    void update(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable,
                             const unsigned char selectivity, const unsigned char depth, const int alpha, const int score, const char move);

    std::string line2String(RXBitBoard& board, const int depth, const t_hash type_hashtable) const;
    void mainVariation(std::vector<unsigned char>& pv, RXBitBoard& board, const t_hash type_hashtable, const int depth) const;

    void new_search(const unsigned int color, const int n_empty);
    int get_date(const unsigned int color) { return date[_shared ? 0:color];};
    
};

inline void RXHashTable::entry_prefetch(const unsigned long long hash_code, const t_hash type_hashtable) const {
    const RXHashEntry* ptr = &(table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])]);
#ifdef __ARM_ACLE
    __pld(ptr);
#else
    __builtin_prefetch(ptr);
#endif
}



inline void RXHashTable::new_search(const unsigned int color, const int n_empty) {
    
    int stage = 2*(60-n_empty);
    
    if(stage>date[_shared ? 0:color])
        date[_shared ? 0:color] = stage;
    
}



// The 'source' method (with the raw parameters)
__attribute__((always_inline))
inline bool RXHashTable::get(const unsigned long long hash_code, const unsigned long long P, const unsigned long long O, const t_hash type_hashtable, RXHashValue& hValue) const {

    const RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
    
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

// 2. method "wrapper"
__attribute__((always_inline))
inline bool RXHashTable::get(const unsigned long long hash_code, const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& hValue) const {

    const unsigned long long P = board.discs[board.player];
    const unsigned long long O = board.discs[board.player^1];

    return get(hash_code, P, O, type_hashtable, hValue);

}


// 2. method "wrapper"
__attribute__((always_inline))
inline bool RXHashTable::get(const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& hValue) const {
    // On calcule le hash_code et on appelle la version mutualisée
    const unsigned long long hash_code = board.hashcode();
    
    return get(hash_code, board, type_hashtable, hValue);
}


#endif
