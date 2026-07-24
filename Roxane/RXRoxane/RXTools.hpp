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
#include <chrono>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <random>

#include <bit>
#include <cstdint>

#if defined(_MSC_VER) && !defined(__clang__)
    #include <intrin.h>
    #define RX_ALWAYS_INLINE __forceinline
    #define RX_LAMBDA_INLINE [[msvc::forceinline]]
    #define RX_RESTRICT      __restrict
    #if defined(_M_ARM64)
        #define RX_PREFETCH(addr) __prefetch(addr)
    #else
        #include <xmmintrin.h>
        #define RX_PREFETCH(addr) _mm_prefetch((const char*)(addr), _MM_HINT_T0)
    #endif
#elif defined(__clang__)
    #define RX_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define RX_LAMBDA_INLINE [[clang::always_inline]]
    #define RX_RESTRICT      __restrict__
    #define RX_PREFETCH(addr) __builtin_prefetch(addr, 0, 3)
#else
    #define RX_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define RX_LAMBDA_INLINE [[gnu::always_inline]]
    #define RX_RESTRICT      __restrict__
    #define RX_PREFETCH(addr) __builtin_prefetch(addr, 0, 3)
#endif

static inline unsigned long long _bsrll(const unsigned long long Mask) {
    if (Mask == 0)
        return 0;
    return 63 - static_cast<unsigned long long>(std::countl_zero(Mask));
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

// returns the current system time, measured in
// milliseconds.
static inline int get_system_time() {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return static_cast<int>(duration.count());
}

//random int [min ; max]
static inline int random_bounds(int min, int max)
{
    static std::mt19937 rng(
        static_cast<unsigned int>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// --- Sigmoïde lisse pour les transitions ---
inline double sigmoid(double x, double x0, double k = 0.3)
{
    return 1.0 / (1.0 + std::exp(-k * (x - x0)));
}


#endif
