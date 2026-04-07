/*
 *  RXHashTable.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 18/05/05.
 *  Copyleft 2005-2025 personnel.
 *
 */


#ifndef RXHASHTABLE_HPP
#define RXHASHTABLE_HPP

#include <string>
#include <map>
#include <list>
#include <vector>

#include "RXConstantes.hpp"
#include "RXBitBoard.hpp"
#include "RXMove.hpp"

#ifdef __ARM_ACLE
#include "arm_acle.h"
#endif






class RXHashValue {
	
	public:
	
		unsigned char date;				//	  8 bits
		unsigned char depth;			//	  8 bits
		unsigned char selectivity;		//	  8 bits
		unsigned char move;				//	  8 bits
		short lower;					//	 16 bits
		short upper;					//	 16 bits
		// = 64 bits
		
		RXHashValue(const unsigned long long packed = 0);
		
		unsigned long long wide_2_compact() const;
		void compact_2_wide(unsigned long long packed);
	
};


inline unsigned long long RXHashValue::wide_2_compact() const {
	
	unsigned long long packed = date;
	packed <<= 8;
	packed 	|= depth;
	packed <<= 8;
	packed 	|= selectivity;
	packed <<= 8;
	packed 	|= move;
	packed <<= 16;
	packed 	|= (static_cast<long long>(upper) & 0x000000000000FFFFULL);
	packed <<= 16;
	packed 	|= (static_cast<long long>(lower) & 0x000000000000FFFFULL);
	
	return packed;
}

inline void RXHashValue::compact_2_wide(unsigned long long packed) {
	
	lower = static_cast<short>(packed & 0x000000000000FFFFULL);
	packed >>= 16;
	upper = static_cast<short>(packed & 0x000000000000FFFFULL);
	packed >>= 16;
	move = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
	packed >>= 8;	
	selectivity = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
	packed >>= 8;	
	depth = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
	packed >>= 8;	
	date = static_cast<unsigned char>(packed & 0x00000000000000FFULL);
	
}

class RXHashRecord {
	
	public :
		unsigned long long lock;
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

class RXHashEntry {
	
	public :
		RXHashRecord deepest;
		RXHashRecord newest;
};


class RXHashTable {
    
    public:
    enum t_hash {HASH_SHARED, HASH_BLACK, HASH_WHITE};

    
    private :
    mutable RXMove _move[61][2];
    
    mutable std::vector<RXHashEntry> table;
    unsigned int nBitsTable;
    unsigned int _offsetTable[3];
    unsigned int _maskTable[3];
    
    unsigned char date[2];
    
    bool _shared;

    RXHashRecord* get_record(const RXBitBoard& board, const t_hash type_hashtable) const;


    void copyPV(RXBitBoard& board, const t_hash from_hashtable, const t_hash to_hashtable);
    void protectPV(RXBitBoard& board, const t_hash type_hashtable, const bool passed = false);
 
    
    public :
    
    RXHashTable(unsigned int nBTable);
    ~RXHashTable() {};
    
    void reset();
    
    void shared(const bool flag);
    
    bool is_shared() const;
    
    void entry_prefetch(const unsigned long long hash_code, const t_hash type_hashtable) const;
    
    bool get(const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& entry) const;
    bool get(const unsigned long long hash_code, const t_hash type_hashtable, RXHashValue& entry) const;
    
    
    void copyPV(RXHashTable* from_hashtable, const t_hash from_type_hash, RXBitBoard&board, const t_hash to_type_hash);
    
    void copyPV_shared_to_color(RXBitBoard& board, const int color);
    void copyPV_color_to_shared(RXBitBoard& board, const int color);
    void mergePV(RXBitBoard& board);
    void protectPV(RXBitBoard& board);
    
    void update(const unsigned long long hash_code, const t_hash type_hashtable, const unsigned char selectivity, const unsigned char depth, const int alpha, const int beta, const int score, const char move);
    void update(const unsigned long long hash_code, const t_hash type_hashtable, const unsigned char selectivity, const unsigned char depth, const int alpha, const int score, const char move);
    
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


inline bool RXHashTable::get(const RXBitBoard& board, const t_hash type_hashtable, RXHashValue& hValue) const {
    
    const unsigned long long hash_code = board.hashcode();
	
	const RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	
	unsigned long long packed = entry.deepest.packed;
	unsigned long long lock = entry.deepest.lock ^ packed;
	if (hash_code == lock) {
		
		hValue.compact_2_wide(packed);
		
		return true;
	}
	
	
	packed = entry.newest.packed;
	lock = entry.newest.lock ^ packed;
	if (hash_code == lock) {
		
		hValue.compact_2_wide(packed);
		
		return true;
	}
	
	return false;
}

inline bool RXHashTable::get(const unsigned long long hash_code, const t_hash type_hashtable, RXHashValue& hValue) const {

	const RXHashEntry& entry = table[_offsetTable[type_hashtable] | (static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	
	unsigned long long packed = entry.deepest.packed;
	unsigned long long lock = entry.deepest.lock ^ packed;
	if (hash_code == lock) {
		
		hValue.compact_2_wide(packed);
		
		return true;
	}
	
	
	packed = entry.newest.packed;
	lock = entry.newest.lock ^ packed;
	if (hash_code == lock) {
		
		hValue.compact_2_wide(packed);
		
		return true;
	}
	
	return false;
}

//*********** Attention **********
//        Aucun moteur actif
//*********** Attention **********

inline RXHashRecord* RXHashTable::get_record(const RXBitBoard& board, const t_hash type_hashtable) const {
    
    const unsigned long long hash_code = board.hashcode();
	
	RXHashEntry& entry = table[_offsetTable[type_hashtable] |(static_cast<unsigned int>(hash_code>>32) & _maskTable[type_hashtable])];
	
	if((entry.deepest.lock ^ entry.deepest.packed) == hash_code)
		return &(entry.deepest);
	
	if((entry.newest.lock ^ entry.newest.packed) == hash_code)
		return &(entry.newest);
	
	return nullptr;
	
	
}





#endif
