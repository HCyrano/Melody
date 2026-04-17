// Copyleft 2001 Chris Welty
//	All Rights Reserved

#ifndef SOCKBUF_HPP
#define SOCKBUF_HPP

#include <sys/socket.h>
#include <iostream>
#include <fstream>

#include "types.hpp"




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
    
    int sock; //SOCKET sock;
    std::ofstream *fplog;
    enum {kLogNone, kLogRecv, kLogSend} loglast;
    char *buf;
    int err;
};

#endif
