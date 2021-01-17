

#ifndef INETADDRESS_H_INCLUDED
#define INETADDRESS_H_INCLUDED

#include <netinet/in.h>
#include <string>
#include <string_view>
#include "SocketsOps.h"

namespace CEventLoop
{
    class InetAddress
    {
    public:
        InetAddress(std::string_view addr)
        {
            addr_.sin_family = AF_INET;
        }

        InetAddress(struct sockaddr_in6 addr)
        {
            addr6_ = addr;
        }

        InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

        sa_family_t family() const { return addr_.sin_family; }
        const struct sockaddr* getSockAddr() const { return reinterpret_cast<struct sockaddr*>((void*)&addr6_); }

        socklen_t len() const { return sizeof(sockaddr_in6); }

        void setSockAddrInet6(struct sockaddr_in6* addr)
        {
            addr6_ = *addr;
        }

        std::string toIpPort() const
        {
            char buf[64] = "";
            SocketsOps::ToIpPort(buf, sizeof(buf), getSockAddr());
            return std::string(buf);
        }

    private:
        union
        {
            struct sockaddr_in addr_;
            struct sockaddr_in6 addr6_;
        };
    };
}


#endif