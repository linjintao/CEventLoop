

#include "InetAddress.h"
#include <string.h>
#include <endian.h>

CEventLoop::InetAddress::InetAddress(uint16_t port, bool loopbackOnly, bool ipv6)
{
    if (ipv6)
    {
        bzero(&addr6_, sizeof(addr6_));
        addr6_.sin6_family = AF_INET6;
        in6_addr ip = in6addr_any;
        addr6_.sin6_addr = ip;
        addr6_.sin6_port = htobe16(port);
    }
    else
    {
        bzero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        in_addr_t ip = INADDR_ANY;
        addr_.sin_port = htobe16(port);
        addr_.sin_addr.s_addr = htobe32(ip);
    }
}
