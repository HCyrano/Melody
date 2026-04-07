/*
 *  RXHashShallow.cpp
 *  Roxane
 *
 *  Created by Bruno Causse on 28/03/06.
 *  Copyleft 2025 personnel.
 *
 */

#include "RXHashShallow.hpp"

#include <iostream>
#include <new>
#include <sstream>


#include "RXHashTable.hpp"




RXHashShallow::RXHashShallow(unsigned int nBitsTable) : table(0), date(0) {
    
    _maskTable = (1UL<<nBitsTable) -1;
    table.resize(1UL<<nBitsTable);
    
}



void RXHashShallow::reset() {
    
    
    for(std::vector<RXHashEntry>::iterator iter = table.begin(); iter!=table.end(); iter++) {
        (iter->deepest).packed = 0x0ULL;
        (iter->newest).packed = 0x0ULL;
    }
    
    date = 0;
}


//implementation LockFree
void RXHashShallow::update(const unsigned long long hash_code, const unsigned char depth, const int alpha, const int beta, const int score, const char move) {
    
    RXHashEntry& entry = table[static_cast<unsigned int>(hash_code>>32) & _maskTable];
    
    int _date = date;
    
    if(alpha < score && score < beta)
        ++_date; //bonus for exact score

    
    RXHashRecord& deepest = entry.deepest;
    
    const unsigned long long deepest_packed = deepest.packed;
    const unsigned long long deepest_lock = deepest.lock;
    
    const unsigned long long deepest_hashcode = deepest_lock ^ deepest_packed;
    RXHashValue deepest_value(deepest_packed);
    
    
    /* try to update deepest entry */
    if (hash_code == deepest_hashcode && depth == deepest_value.depth) {
        
        if (score < beta && score < deepest_value.upper)
            deepest_value.upper = static_cast<short>(score);
        if (score > alpha) {
            deepest_value.move = move;
            
            if(score > deepest_value.lower)
                deepest_value.lower = static_cast<short>(score);
        }
        
        /* control if lower>upper : Instability */
        if(deepest_value.lower>deepest_value.upper) {
                        
            if(score<beta)
                deepest_value.upper = static_cast<short>(score);
            else
                deepest_value.upper = MAX_SCORE;
            
            if(score>alpha)
                deepest_value.lower = static_cast<short>(score);
            else
                deepest_value.lower = -MAX_SCORE;
            
            deepest_value.move = move;
            
        } else if(deepest_value.lower == deepest_value.upper) {
            if(_date == date)
                ++_date;
        }
        
        deepest_value.date = _date;
        
        deepest.packed = deepest_value.wide_2_compact();
        deepest.lock   = hash_code ^ deepest.packed;
        
    } else {
        
        RXHashRecord& newest = entry.newest;
        
        const unsigned long long newest_packed = newest.packed;
        const unsigned long long newest_hashcode= newest.lock ^ newest_packed;
        RXHashValue newest_value(newest_packed);
        
        
        /* else try to update newest entry */
        if (hash_code == newest_hashcode && depth == newest_value.depth) {
            
            if (score < beta && score < newest_value.upper)
                newest_value.upper = static_cast<short>(score);
            if (score > alpha) {
                newest_value.move =  move;
                
                if(score > newest_value.lower)
                    newest_value.lower =  static_cast<short>(score);
            }
            
            /* control if lower>upper : Instability */
            if(newest_value.lower>newest_value.upper) {

                if(score<beta)
                    newest_value.upper = static_cast<short>(score);
                else
                    newest_value.upper = MAX_SCORE;
                
                if(score>alpha)
                    newest_value.lower = static_cast<short>(score);
                else
                    newest_value.lower = -MAX_SCORE;
                
                newest_value.move =  move;
                
            } else if(newest_value.lower == newest_value.upper) {
                if(_date == date)
                    ++_date;
            }
            
            newest_value.date = _date;

            //implementation 2025-08-05 (en test)
            if(newest_value.date > deepest_value.date) {
                
                //copy
                newest.lock   = deepest_lock;
                newest.packed = deepest_packed;
                
                deepest.packed = newest_value.wide_2_compact();
                deepest.lock   = hash_code ^ deepest.packed;
                
            } else {
                
                newest.packed = newest_value.wide_2_compact();
                newest.lock   = hash_code ^ newest.packed;
            }
            
            /* else try to add to deepest entry */
        } else if (deepest_hashcode == hash_code || deepest_value.date < _date ||  deepest_value.depth < depth) { // priority
            
            if(deepest_hashcode != hash_code &&	(newest_hashcode == hash_code || (newest_value.date < deepest_value.date ||
                                                                                  ((newest_value.date == deepest_value.date) && (newest_value.depth <= deepest_value.depth))))) {
                
                //copy
                newest.lock = deepest_lock;
                newest.packed = deepest_packed;
            }
            
            deepest_value.depth = depth;
            deepest_value.date = _date;
            deepest_value.lower = -MAX_SCORE;
            deepest_value.upper = +MAX_SCORE;
            if (score < beta) deepest_value.upper = static_cast<short>(score);
            if (score > alpha) deepest_value.lower = static_cast<short>(score);
            deepest_value.move =  move;
            
            deepest.packed = deepest_value.wide_2_compact();
            deepest.lock = hash_code ^ deepest.packed;
            
            
            /* else add to newest entry */
        } else {
            
            newest_value.depth = depth;
            newest_value.date = _date;
            newest_value.lower = -MAX_SCORE;
            newest_value.upper = +MAX_SCORE;
            if (score < beta) newest_value.upper = static_cast<short>(score);
            if (score > alpha) newest_value.lower = static_cast<short>(score);
            newest_value.move =  move;
            
            newest.packed = newest_value.wide_2_compact();
            newest.lock = hash_code ^ newest.packed;
            
            
        }
    }
    
}




