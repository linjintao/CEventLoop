#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include "InetAddress.h"
namespace CEventLoop
{
    class Socket
    {
    public:
        Socket(int sockfd) :sockfd_(sockfd) {}

        void listen();
        void bindAddress(const InetAddress& listenAddr);
        int fd() const { return sockfd_; }
        int accept(InetAddress* peerAddr);
    private:
        int sockfd_;

    };
}



#endif