#include "Socket.h"
#include "SocketsOps.h"
#include "Logging.h"
#include <netinet/in.h>
#include <string.h>
using namespace CEventLoop;

void Socket::listen()
{
    SocketsOps::Listen(sockfd_);
}

void CEventLoop::Socket::bindAddress(const InetAddress& listenAddr)
{
    int ret = ::bind(sockfd_, listenAddr.getSockAddr(), listenAddr.len());
    if (ret < 0)
    {
        LOG_INFO << "Socket::bindAddreee() failed";
    }
}

int CEventLoop::Socket::accept(InetAddress* peerAddr)
{
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof(addr));
    int ret = SocketsOps::Accept(sockfd_, &addr);
    peerAddr->setSockAddrInet6(&addr);

    return ret;
}
