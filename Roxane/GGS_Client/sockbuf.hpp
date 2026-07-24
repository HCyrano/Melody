/*
 * Copyright (C) 2001  Chris Welty
 * Copyright (C) 2026  bruno Causse (modified)
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

#ifndef SOCKBUF_HPP
#define SOCKBUF_HPP

#include <iostream>
#include <fstream>

#include "types.hpp"
#include "sockcompat.hpp"   // sock_t, CLOSESOCK, sock_errno, WinsockInit, ...




class sockbuf : public std::streambuf {
    
public:
    
    // construction/destruction
    sockbuf();
    virtual ~sockbuf();
    
    // overrides
    virtual int underflow();
    virtual int overflow(int c=EOF);
    virtual int sync();
    
    enum {
        kErrMem=0x8100,
        kErrUnknown=0x8600,
        kErrNoHost,
        kErrCantConnect,
        kErrConnectionReset,
        kErrConnectionClosed,
        kErrNotConnected,
        kErrAlreadyConnected
    };
    int Err() const;
    static const char* ErrText(int err);
    
    
    bool IsConnected() const;
    
    int connect(const std::string& sServer, int nPort);
    int disconnect();
    
protected:
    enum { fLogging=1, nBufSize=1024 };
    
    bool fConnected;
    
    WinsockInit wsaInit;    // RAII: WSAStartup/WSACleanup on Windows, no-op elsewhere
    sock_t sock;            // SOCKET on Windows, int on POSIX
    std::ofstream *fplog;
    enum {kLogNone, kLogRecv, kLogSend} loglast;
    char *buf;
    int err;
};

#endif
