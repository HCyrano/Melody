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

#include <fstream>
#include <iomanip>

#include "sockbuf.hpp"
#include "types.hpp"


sockbuf::sockbuf() : sock(INVALID_SOCK) {
	// log incoming data to file?
	fplog=nullptr;
	loglast=kLogNone;
	if (fLogging) {
		fplog=new std::ofstream("recv.txt");
        if(!fplog->is_open())
            std::cout << "unable to open log file for writing" << std::endl;
	}

	// create get buffer
	buf=new char[2*nBufSize];
	if (buf) {
		setg(buf,buf+nBufSize, buf+nBufSize);
		setp(buf+nBufSize,buf+2*nBufSize);
		err=0;
	}
	else {
		err=kErrMem;
	}

	fConnected=false;
}

sockbuf::~sockbuf() {
	if (fConnected)
		disconnect();

	if (fLogging) {
		if (fplog && fplog->is_open())
			fplog->close();
        delete fplog;
	}
    
	if (buf)
		delete [] buf;
}

int sockbuf::connect(const std::string& sServer, int nPort) {
    
    const int RECV_TIMEOUT_SEC = 90;
    const int SEND_TIMEOUT_SEC = 10;
    
	if (err)
		return err;

	if (fConnected) {
		return kErrAlreadyConnected;
	}
    
    // Résolution DNS avec getaddrinfo (IPv4 + IPv6, thread-safe, non dépréciée)
    struct addrinfo hints{};
    hints.ai_family   = AF_UNSPEC;      // Accepte IPv4 et IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo* res = nullptr;
    const std::string sPort = std::to_string(nPort);

    int gai_err = getaddrinfo(sServer.c_str(), sPort.c_str(), &hints, &res);
    if (gai_err != 0) {
        std::cerr << "[SOCKBUF] getaddrinfo failed: " << GaiErrString(gai_err) << std::endl;
        return kErrNoHost;
    }

    // Essayer chaque adresse retournée jusqu'à ce qu'une connexion réussisse
    sock = INVALID_SOCK;
    for (struct addrinfo* p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == INVALID_SOCK) continue;

        // ═══════════════════════════════════════════════════════════════════════════
        // CONFIGURATION MINIMALE - options portables (POSIX + Winsock)
        // ═══════════════════════════════════════════════════════════════════════════
        
        std::cout << "\n[SOCKBUF] ═══════════════════════════════════════" << std::endl;
        std::cout << "[SOCKBUF] Configuring socket for network detection" << std::endl;
        std::cout << "[SOCKBUF] ═══════════════════════════════════════\n" << std::endl;
        
        // ────────────────────────────────────────────────────────────────────────
        // OPTION 1 : SO_RCVTIMEO - LA CLÉ POUR DÉTECTER LES COUPURES
        // ────────────────────────────────────────────────────────────────────────
        // C'est l'option la plus importante !
        // recv() retournera une erreur si aucune donnée n'arrive pendant ce délai
        //
        // NB: sous Winsock, SO_RCVTIMEO/SO_SNDTIMEO attendent un DWORD en
        // millisecondes, alors que POSIX attend une struct timeval. On
        // branche donc les deux formes ici.

#ifdef _WIN32
        DWORD recvTimeoutMs = static_cast<DWORD>(RECV_TIMEOUT_SEC) * 1000;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                       reinterpret_cast<const char*>(&recvTimeoutMs), sizeof(recvTimeoutMs)) == 0) {
            std::cout << "[SOCKBUF] ✓ SO_RCVTIMEO = " << RECV_TIMEOUT_SEC << " seconds" << std::endl;
        } else {
            std::cerr << "[SOCKBUF] ✗ Failed to set SO_RCVTIMEO: " << SockErrString(sock_errno()) << std::endl;
            std::cerr << "[SOCKBUF]   WARNING: Network failures may not be detected!" << std::endl;
        }
#else
        struct timeval recv_timeout{RECV_TIMEOUT_SEC, 0};
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                       reinterpret_cast<const char*>(&recv_timeout), sizeof(recv_timeout)) == 0) {
            std::cout << "[SOCKBUF] ✓ SO_RCVTIMEO = " << recv_timeout.tv_sec << " seconds" << std::endl;
            std::cout << "[SOCKBUF]   recv() will timeout after " << recv_timeout.tv_sec
                      << "s with no data" << std::endl;
        } else {
            std::cerr << "[SOCKBUF] ✗ Failed to set SO_RCVTIMEO: " << SockErrString(sock_errno()) << std::endl;
            std::cerr << "[SOCKBUF]   WARNING: Network failures may not be detected!" << std::endl;
        }
#endif
        
        // ────────────────────────────────────────────────────────────────────────
        // OPTION 2 : SO_SNDTIMEO - Timeout pour send()
        // ────────────────────────────────────────────────────────────────────────
        
#ifdef _WIN32
        DWORD sendTimeoutMs = static_cast<DWORD>(SEND_TIMEOUT_SEC) * 1000;
        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                       reinterpret_cast<const char*>(&sendTimeoutMs), sizeof(sendTimeoutMs)) == 0) {
            std::cout << "[SOCKBUF] ✓ SO_SNDTIMEO = " << SEND_TIMEOUT_SEC << " seconds" << std::endl;
        } else {
            std::cerr << "[SOCKBUF] ✗ Failed to set SO_SNDTIMEO: " << SockErrString(sock_errno()) << std::endl;
        }
#else
        struct timeval send_timeout{SEND_TIMEOUT_SEC, 0};
        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,
                       reinterpret_cast<const char*>(&send_timeout), sizeof(send_timeout)) == 0) {
            std::cout << "[SOCKBUF] ✓ SO_SNDTIMEO = " << send_timeout.tv_sec << " seconds" << std::endl;
        } else {
            std::cerr << "[SOCKBUF] ✗ Failed to set SO_SNDTIMEO: " << SockErrString(sock_errno()) << std::endl;
        }
#endif
        
        // ────────────────────────────────────────────────────────────────────────
        // OPTION 3 : SO_KEEPALIVE - Keepalive TCP basique
        // ────────────────────────────────────────────────────────────────────────
        // Active le keepalive avec les paramètres système par défaut.
        // macOS: ~2h par défaut. Windows: ~2h par défaut également.
        
        int keepalive = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE,
                       reinterpret_cast<const char*>(&keepalive), sizeof(keepalive)) == 0) {
            std::cout << "[SOCKBUF] ✓ SO_KEEPALIVE enabled (system defaults)" << std::endl;
            std::cout << "[SOCKBUF]   Note: System default is typically ~2 hours" << std::endl;
        } else {
            std::cerr << "[SOCKBUF] ✗ Failed to enable SO_KEEPALIVE: " << SockErrString(sock_errno()) << std::endl;
        }
        
        // ────────────────────────────────────────────────────────────────────────
        // FIN DE LA CONFIGURATION
        // ────────────────────────────────────────────────────────────────────────

        if (::connect(sock, p->ai_addr, static_cast<int>(p->ai_addrlen)) == 0)
            break;  // Succès !

        CLOSESOCK(sock);
        sock = INVALID_SOCK;
    }
    freeaddrinfo(res);  // Toujours libérer, même en cas d'échec

    if (sock == INVALID_SOCK)
		return kErrCantConnect;

    
    std::cout << "\n[SOCKBUF] ═══════════════════════════════════════" << std::endl;
    std::cout << "[SOCKBUF] Detection Strategy:" << std::endl;
    std::cout << "[SOCKBUF] ───────────────────────────────────────" << std::endl;
    std::cout << "[SOCKBUF]  1. Process() continuously reads data" << std::endl;
    std::cout << "[SOCKBUF]  2. HeartbeatLoop() sends 't /os continue' every 60s" << std::endl;
    std::cout << "[SOCKBUF]  3. If network fails, no data arrives" << std::endl;
    std::cout << "[SOCKBUF]  4. After " << RECV_TIMEOUT_SEC << "s, recv() times out" << std::endl;
    std::cout << "[SOCKBUF]  5. recv() returns -1 (timeout error)" << std::endl;
    std::cout << "[SOCKBUF]  6. underflow() detects error" << std::endl;
    std::cout << "[SOCKBUF]  7. Process() exits read loop" << std::endl;
    std::cout << "[SOCKBUF]  8. TryReconnect() is called!" << std::endl;
    std::cout << "[SOCKBUF] ═══════════════════════════════════════\n" << std::endl;
    
    // ═══════════════════════════════════════════════════════════════════════════


	fConnected=true;
	return 0;
}

int sockbuf::disconnect() {
	if (fConnected) {
		CLOSESOCK(sock);
        //fplog->close();
		fConnected=false;
		return 0;
	}
	else {
		return kErrNotConnected;
	}
}

bool sockbuf::IsConnected() const {
	return fConnected;
}

int sockbuf::underflow() {
    
	char* p0 = nullptr;
	int nGetSize = 0;

	if (!fConnected || err)
		return EOF;

	if (eback()) {
		p0=eback();
		nGetSize=nBufSize;
	} else {
        return EOF; // *p0;
    }
	
    // ═══════════════════════════════════════════════════════════════════════════
    // APPEL CRITIQUE : recv()
    // Avec SO_RCVTIMEO configuré, recv() retournera -1 après timeout
    // ═══════════════════════════════════════════════════════════════════════════
    
    ssize_t_compat nrecv = recv(sock, p0, nGetSize, 0);
    
    if (nrecv < 0) {
        
        int e = sock_errno();
        //std::cout << "[DEBUG] recv() returned: " << nrecv << ", err=" << e << std::endl;

        switch (ClassifySockError(e)) {
            case SockErrCategory::kWouldBlockTimeout:
                // ⚡ TIMEOUT ! C'est CE cas qui détecte la coupure réseau
                err = kErrConnectionReset;
                std::cerr << "\n";
                std::cerr << "════════════════════════════════════════════════════════════════" << std::endl;
                std::cerr << "⚠️  ⚠️  ⚠️  NETWORK FAILURE DETECTED  ⚠️  ⚠️  ⚠️" << std::endl;
                std::cerr << "════════════════════════════════════════════════════════════════" << std::endl;
                std::cerr << "[SOCKBUF] recv() timeout" << std::endl;
                std::cerr << "[SOCKBUF] No data received for 90 seconds" << std::endl;
                std::cerr << "[SOCKBUF] Connection is considered DEAD" << std::endl;
                std::cerr << "[SOCKBUF] Returning EOF to trigger reconnection..." << std::endl;
                std::cerr << "════════════════════════════════════════════════════════════════" << std::endl;
                std::cerr << "\n";
                break;

            case SockErrCategory::kConnReset:
                err = kErrConnectionReset;
                std::cerr << "[SOCKBUF] ⚠️  Connection reset by peer" << std::endl;
                break;

            case SockErrCategory::kTimedOut:
                err = kErrConnectionReset;
                std::cerr << "[SOCKBUF] ⚠️  Connection timed out" << std::endl;
                break;

            case SockErrCategory::kNetUnreachable:
                err = kErrConnectionReset;
                std::cerr << "[SOCKBUF] ⚠️  Network unreachable" << std::endl;
                break;

            case SockErrCategory::kNotConnected:
                err = kErrConnectionClosed;
                std::cerr << "[SOCKBUF] ⚠️  Socket not connected" << std::endl;
                break;

            default:
                err = kErrUnknown;
                std::cerr << "[SOCKBUF] ⚠️  recv() error: " << SockErrString(e)
                          << " (code=" << e << ")" << std::endl;
                break;
        }
        return EOF;
    }

	if (nrecv==0) {
		// connection closed
		err=kErrConnectionClosed;
		return EOF;
	} else {
		if (gptr() >= egptr())
			setg(p0, p0, p0+nrecv);
		//else
		//	setg(unbuf, unbuf-1, unbuf+1);
		if (fplog && fplog->is_open()) {
			if (loglast!=kLogRecv) {
				loglast=kLogRecv;
               fplog->write("[recv]",6);
			}
            fplog->write(p0, nrecv);
			*fplog << std::flush;
		}
		return *p0;
	}
}

int sockbuf::overflow(int c) {
    
    if (!fConnected || err)
        return EOF;
    
    // Envoi du buffer principal en boucle (gère les envois partiels)
    long nSend     = pptr() - pbase();
    long nTotalSent = 0;
    
    while (nTotalSent < nSend) {
        ssize_t_compat nSent = send(sock, pbase() + nTotalSent,
                                     static_cast<int>(nSend - nTotalSent), 0);
        
        if (nSent < 0) {
            int e = sock_errno();
            switch (ClassifySockError(e)) {
                case SockErrCategory::kWouldBlockTimeout:
                    err = kErrConnectionReset;
                    std::cerr << "[SOCKBUF] ⚠️  send() timeout (buffer full)" << std::endl;
                    break;
                case SockErrCategory::kNotConnected:
                    err = kErrConnectionClosed;
                    std::cerr << "[SOCKBUF] ⚠️  send() failed: Broken pipe" << std::endl;
                    break;
                case SockErrCategory::kConnReset:
                    err = kErrConnectionReset;
                    std::cerr << "[SOCKBUF] ⚠️  send() failed: Connection reset" << std::endl;
                    break;
                case SockErrCategory::kTimedOut:
                    err = kErrConnectionReset;
                    std::cerr << "[SOCKBUF] ⚠️  send() failed: Timeout" << std::endl;
                    break;
                default:
                    err = kErrUnknown;
                    std::cerr << "[SOCKBUF] ⚠️  send() error: " << SockErrString(e) << std::endl;
                    break;
            }
            setp(pbase(), epptr());
            return EOF;
        }
        
        nTotalSent += nSent;
    }
    
    // Log du buffer envoyé
    if (fplog && fplog->is_open()) {
        if (loglast != kLogSend) {
            loglast = kLogSend;
            fplog->write("[send]", 6);
        }
        fplog->write(pbase(), nSend);
    }
    
    // Envoi du caractère débordant (c), s'il existe
    if (c != EOF) {
        char cc = static_cast<char>(c);
        ssize_t_compat nSent = send(sock, &cc, 1, 0);
        if (nSent != 1) {
            err = kErrConnectionClosed;
            std::cerr << "[SOCKBUF] ⚠️  send() failed on overflow char" << std::endl;
            setp(pbase(), epptr());
            return EOF;
        }
        if (fplog && fplog->is_open())
            fplog->write(&cc, 1);
    }
    
    if (fplog && fplog->is_open())
        fplog->flush();
    
    setp(pbase(), epptr());
    return 0;
}

int sockbuf::sync() {
	return overflow(EOF);
}

int sockbuf::Err() const {
	return err;
}

const char* sockbuf::ErrText(int err) {
    switch(err) {
        case 0:
            return "No error";
        case kErrMem:
            return "Out of memory";
        case kErrNoHost:
            return "Host not found (DNS lookup failed)";
        case kErrCantConnect:
            return "Connection refused or server unreachable";
        case kErrConnectionReset:
            return "Connection reset by peer (the server dropped the link)";
        case kErrConnectionClosed:
            return "Connection closed gracefully by the server";
        case kErrNotConnected:
            return "Operation failed: Socket is not connected";
        case kErrAlreadyConnected:
            return "Operation failed: Socket is already connected";
        case kErrUnknown:
            return "An unknown socket error occurred";
        default:
            return "Unspecified network error";
 }
}
