#ifndef SOCKETS_OPS_H_INCLUDED
#define SOCKETS_OPS_H_INCLUDED

#include <arpa/inet.h>

namespace CEventLoop
{
    namespace SocketsOps
    {
        int CreateNonblocking(sa_family_t family);

        int Connect(int sockfd, const struct sockaddr* addr);
        void Listen(int sockfd);
        void Close(int fd);
        void ShutdownWrite(int sockfd);
        size_t Write(int fd, const void* data, size_t len);
        ssize_t Read(int fd, void* buf, size_t len);

        int Accept(int fd, struct sockaddr_in6* addr);
        int getSocketError(int sockfd);
        bool IsSelfConnect(int sockfd);

        struct sockaddr_in6 GetLocalAddr(int sockfd);
        struct sockaddr_in6 GetPeerAddr(int sockfd);

        void ToIpPort(char* buf, size_t size, const struct sockaddr* addr);
        void ToIp(char* buf, size_t size, const struct sockaddr* addr);
    }
}


#endif