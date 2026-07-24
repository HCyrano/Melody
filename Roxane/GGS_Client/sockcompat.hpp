/*
 * sockcompat.hpp
 *
 * Small shim isolating the platform differences between BSD sockets
 * (macOS/Linux) and Winsock (Windows) so that sockbuf.cpp/hpp can stay
 * platform-agnostic in its logic.
 *
 * Copyright (C) 2026  bruno Causse
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

#ifndef SOCKCOMPAT_HPP
#define SOCKCOMPAT_HPP

#include <string>

#ifdef _WIN32

    // ─────────────────────────────────────────────────────────────────
    // Windows / Winsock
    // ─────────────────────────────────────────────────────────────────
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <BaseTsd.h>

    #pragma comment(lib, "Ws2_32.lib")

    typedef SOCKET      sock_t;
    typedef SSIZE_T     ssize_t_compat;

    #define INVALID_SOCK INVALID_SOCKET
    #define CLOSESOCK    closesocket

    inline int sock_errno() { return WSAGetLastError(); }

    inline std::string GaiErrString(int gai_err) {
        return gai_strerrorA(gai_err);
    }

    inline std::string SockErrString(int e) {
        char buf[256] = {0};
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, e, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       buf, sizeof(buf), nullptr);
        std::string s(buf);
        while (!s.empty() && (s.back() == '\n' || s.back() == '\r'))
            s.pop_back();
        return s;
    }

    // RAII, ref-counted Winsock startup/cleanup. Safe to instantiate one
    // per sockbuf; WSAStartup/WSACleanup are only actually called once.
    class WinsockInit {
    public:
        WinsockInit() {
            if (RefCount()++ == 0) {
                WSADATA wsaData;
                WSAStartup(MAKEWORD(2, 2), &wsaData);
            }
        }
        ~WinsockInit() {
            if (--RefCount() == 0) {
                WSACleanup();
            }
        }
    private:
        static int& RefCount() { static int n = 0; return n; }
    };

#else

    // ─────────────────────────────────────────────────────────────────
    // POSIX (macOS / Linux)
    // ─────────────────────────────────────────────────────────────────
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <cerrno>
    #include <cstring>

    typedef int      sock_t;
    typedef ssize_t  ssize_t_compat;

    #define INVALID_SOCK (-1)
    #define CLOSESOCK    close

    inline int sock_errno() { return errno; }

    inline std::string GaiErrString(int gai_err) {
        return gai_strerror(gai_err);
    }

    inline std::string SockErrString(int e) {
        return std::string(strerror(e));
    }

    // No-op on POSIX; kept so sockbuf can use the same RAII member
    // uniformly on every platform.
    class WinsockInit {
    public:
        WinsockInit() = default;
        ~WinsockInit() = default;
    };

#endif

// ─────────────────────────────────────────────────────────────────────
// Unified error categories, independent of platform. sockbuf.cpp
// switches on this instead of on raw errno/WSAGetLastError() codes.
// ─────────────────────────────────────────────────────────────────────
enum class SockErrCategory {
    kNone,
    kWouldBlockTimeout,   // EAGAIN/EWOULDBLOCK      <-> WSAEWOULDBLOCK
    kConnReset,           // ECONNRESET               <-> WSAECONNRESET
    kTimedOut,            // ETIMEDOUT                <-> WSAETIMEDOUT
    kNetUnreachable,      // ENETUNREACH/EHOSTUNREACH <-> WSAENETUNREACH/WSAEHOSTUNREACH
    kNotConnected,        // EPIPE/ENOTCONN           <-> WSAESHUTDOWN/WSAENOTCONN
    kUnknown
};

inline SockErrCategory ClassifySockError(int e) {
#ifdef _WIN32
    switch (e) {
        case WSAEWOULDBLOCK:   return SockErrCategory::kWouldBlockTimeout;
        case WSAECONNRESET:    return SockErrCategory::kConnReset;
        case WSAETIMEDOUT:     return SockErrCategory::kTimedOut;
        case WSAENETUNREACH:
        case WSAEHOSTUNREACH:  return SockErrCategory::kNetUnreachable;
        case WSAESHUTDOWN:
        case WSAENOTCONN:      return SockErrCategory::kNotConnected;
        default:                return SockErrCategory::kUnknown;
    }
#else
    switch (e) {
        case EAGAIN:
        #if EAGAIN != EWOULDBLOCK
        case EWOULDBLOCK:
        #endif
            return SockErrCategory::kWouldBlockTimeout;
        case ECONNRESET:   return SockErrCategory::kConnReset;
        case ETIMEDOUT:    return SockErrCategory::kTimedOut;
        case ENETUNREACH:
        case EHOSTUNREACH: return SockErrCategory::kNetUnreachable;
        case EPIPE:
        case ENOTCONN:     return SockErrCategory::kNotConnected;
        default:            return SockErrCategory::kUnknown;
    }
#endif
}

#endif // SOCKCOMPAT_HPP
