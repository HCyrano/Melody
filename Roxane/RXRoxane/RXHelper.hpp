/*
 *  RXThreadHelper.hpp
 *  Roxane
 *
 *  Created by Bruno Causse on 10/09/08.
 *  Copyleft 2008-2025 personnel.
 *
 */
#ifndef RXHELPER_HPP
#define RXHELPER_HPP

#include <pthread.h>


// Interface
class Runnable {
	public :
    virtual ~Runnable() = default;
	virtual void* run() = 0;
};

#endif
