/*
 *  RXHashShallow.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 28/03/06.
 *  Copyleft 2025 personnel.
 *
 */


#ifndef RXHASHSHALLOW_HPP
#define RXHASHSHALLOW_HPP

#include <string>

#include "RXHashTable.hpp"
#include "RXConstantes.hpp"
#include "RXBitBoard.hpp"
#include "RXMove.hpp"

#ifdef __ARM_ACLE
#include "arm_acle.h"
#endif



class RXHashShallow {
	
	std::vector<RXHashEntry> table;
	unsigned long _maskTable;	
	
	unsigned char date;
	

	public : 
	
		RXHashShallow(unsigned int nBT);
		~RXHashShallow() {};

		void reset();

        void entry_prefetch(const unsigned long long hash_code) const;

		bool get(const RXBitBoard& board, RXHashValue& entry) const;
		bool get(const unsigned long long hash_code, RXHashValue& entry) const;
		
		void update(const unsigned long long hash_code, const unsigned char depth, const int alpha, const int beta, const int score, const char move);
			
	void new_search(const int n_empty);

};

inline void RXHashShallow::new_search(const int n_empty) {
	
	int stage = 2*(60-n_empty);
	
	if(stage>date)
		date = stage;
	
}

inline void RXHashShallow::entry_prefetch(const unsigned long long hash_code) const {
    const RXHashEntry* ptr = &(table[static_cast<unsigned int>(hash_code>>32) & _maskTable]);
#ifdef __ARM_ACLE
    __pld(ptr);
#else
    __builtin_prefetch(ptr);
#endif
}



//implementation LockFree
inline bool RXHashShallow::get(const RXBitBoard& board, RXHashValue& value) const {
	
    const unsigned long long hash_code = board.hashcode();
    
	const RXHashEntry& entry = table[static_cast<unsigned int>(hash_code>>32) & _maskTable];
	
	unsigned long long packed = entry.deepest.packed;
	unsigned long long lock = entry.deepest.lock ^ packed;
	if (hash_code == lock) {
		
		value.compact_2_wide(packed);
		
		return true;
	}
	
	
	packed = entry.newest.packed;
	lock = entry.newest.lock ^ packed;
	if (hash_code == lock) {
		
		value.compact_2_wide(packed);
		
		return true;
	}
	
	return false;
	
}

inline bool RXHashShallow::get(const unsigned long long hash_code, RXHashValue& value) const {
		
	
	const RXHashEntry& entry = table[static_cast<unsigned int>(hash_code>>32) & _maskTable];
	
	unsigned long long packed = entry.deepest.packed;
	unsigned long long lock = entry.deepest.lock ^ packed;
	if (hash_code == lock) {
		
		value.compact_2_wide(packed);
		
		return true;
	}
	
	
	packed = entry.newest.packed;
	lock = entry.newest.lock ^ packed;
	if (hash_code == lock) {
		
		value.compact_2_wide(packed);
		
		return true;
	}
	
	return false;
	
}

								  

#endif
