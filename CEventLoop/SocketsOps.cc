
#include "SocketsOps.h"
#include "Logging.h"
#include <unistd.h>
#include <string.h>

using namespace CEventLoop;

int SocketsOps::CreateNonblocking(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0)
    {
        LOG_FATAL << "sockets::createNonblockingOrDie";
    }

    return sockfd;
}

int SocketsOps::Connect(int sockfd, const struct sockaddr* addr)
{
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

void SocketsOps::Close(int fd)
{
    if (::close(fd) < 0)
    {
        LOG_SYSERR << "SocketsOps::Close";
    }
}

void CEventLoop::SocketsOps::ShutdownWrite(int sockfd)
{
    if (::shutdown(sockfd, SHUT_WR) < 0)
        LOG_SYSERR << "SockOps::ShutdownWrite";
}

size_t CEventLoop::SocketsOps::Write(int fd, const void* data, size_t len)
{
    return ::write(fd, data, len);
}

ssize_t CEventLoop::SocketsOps::Read(int fd, void* buf, size_t len)
{
    int ret = ::read(fd, buf, len);
    if (ret < 0)
    {
        LOG_SYSERR << "Reading error, fd = " << fd;
    }
    return ret;
}

int CEventLoop::SocketsOps::Accept(int fd, sockaddr_in6* addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof(sockaddr_in6));
    int ret = ::accept(fd, reinterpret_cast<struct sockaddr*>(addr), &addrlen);
    if (ret < 0)
    {
        LOG_SYSERR << "SocketOps::Accept";
    }
    return ret;
}

int CEventLoop::SocketsOps::getSocketError(int sockfd)
{
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
        return errno;
    return optval;
}

bool CEventLoop::SocketsOps::IsSelfConnect(int sockfd)
{
    struct sockaddr_in6 localAddr = GetLocalAddr(sockfd);
    struct sockaddr_in6 peerAddr = GetPeerAddr(sockfd);
    if (localAddr.sin6_family == AF_INET)
    {
        auto laddr4 = reinterpret_cast<struct sockaddr_in*>(&localAddr);
        auto raddr4 = reinterpret_cast<struct sockaddr_in*>(&peerAddr);

        return laddr4->sin_port == raddr4->sin_port && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    }
    else if (localAddr.sin6_family == AF_INET6)
    {
        return localAddr.sin6_port == peerAddr.sin6_port
            && memcmp(&localAddr.sin6_addr, &peerAddr.sin6_addr, sizeof(localAddr.sin6_addr)) == 0;
    }
    return false;
}

sockaddr_in6 CEventLoop::SocketsOps::GetLocalAddr(int sockfd)
{
    struct sockaddr_in6 localAddr;
    bzero(&localAddr, sizeof(localAddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(localAddr));
    if (::getsockname(sockfd, reinterpret_cast<struct sockaddr*>(&localAddr), &addrlen))
    {
        LOG_SYSERR << "SocketsOps::GetLocalAddr";
    }

    return localAddr;
}

sockaddr_in6 CEventLoop::SocketsOps::GetPeerAddr(int sockfd)
{
    struct sockaddr_in6 peerAddr;
    bzero(&peerAddr, sizeof(peerAddr));
    socklen_t addrlen = static_cast<socklen_t>(sizeof(peerAddr));
    if (::getpeername(sockfd, reinterpret_cast<struct sockaddr*>(&peerAddr), &addrlen))
    {
        LOG_SYSERR << "SocketsOps::GetLocalAddr";
    }

    return peerAddr;
}

void CEventLoop::SocketsOps::ToIpPort(char* buf, size_t size, const sockaddr* addr)
{
    ToIp(buf, size, addr);
    size_t end = strlen(buf);
    auto addr4 = reinterpret_cast<const struct sockaddr_in*>(addr);

    uint16_t port = be16toh(addr4->sin_port);
    snprintf(buf + end, size - end, ":%u", port);
}

void CEventLoop::SocketsOps::ToIp(char* buf, size_t size, const sockaddr* addr)
{
    if (addr->sa_family == AF_INET)
    {
        auto addr4 = reinterpret_cast<const sockaddr_in*>(addr);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
    }
    else if(addr->sa_family == AF_INET6)
    {
        auto addr6 = reinterpret_cast<const sockaddr_in6*>(addr);
        ::inet_ntop(AF_INET6, &addr6->sin6_addr, buf, static_cast<socklen_t>(size));
    }
}

void SocketsOps::Listen(int fd)
{
    int ret = ::listen(fd, SOMAXCONN);
    if (ret < 0)
    {
        LOG_SYSERR << "SocketsOps::Listen";
    }
}