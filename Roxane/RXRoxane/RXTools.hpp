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

#ifndef RXTOOLS_HPP
#define RXTOOLS_HPP

#include <string>
#include <sys/time.h>
#include <sstream>
#include <cmath>
#include <iomanip>


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
