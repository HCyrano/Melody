/*
 *  RXTools.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 04/11/08.
 *  Copyleft 2008-2025 personnel.
 *
 */

#ifndef RXTOOLS_HPP
#define RXTOOLS_HPP

#include <string>
#include <sys/time.h>
#include <sstream>
#include <cmath>


static inline unsigned long long _bsrll(const unsigned long long Mask)
{
    return __builtin_clzll(Mask)^63;
}

static inline std::string toHMS(double t) {
	
	std::ostringstream buffer;
	buffer.fill('0');
	
	buffer << std::setw(2) << static_cast<int>(t/(3600)) << ':';
	buffer << std::setw(2) << static_cast<int>(t/(60)) %(60) << ':';
	buffer << std::setw(2) << static_cast<int>(t) %(60) << '.';
	buffer << std::setw(2) << static_cast<int>(t*100)%100;
	
	return buffer.str();
}

// get_system_time() returns the current system time, measured in
// milliseconds.
static inline int get_system_time() {
	struct timeval t;
	gettimeofday(&t, nullptr);
	return static_cast<int>(t.tv_sec*1000 + t.tv_usec/1000); 
}

//random int [min ; max]
static inline int random_bounds(int min, int max)
{
    static bool rand_is_seeded = false;
    if(!rand_is_seeded)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        rand_is_seeded = true;
    }
    return rand()%(max-min+1) + min;
}

// --- Sigmoïde lisse pour les transitions ---
inline double sigmoid(double x, double x0, double k = 0.3)
{
    return 1.0 / (1.0 + std::exp(-k * (x - x0)));
}


#endif
