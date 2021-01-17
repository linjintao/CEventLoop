#ifndef ACCEPTOR_H_INCLUDED
#define ACCEPTOR_H_INCLUDED

#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"

namespace CEventLoop
{
    class EventLoop;
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    class Acceptor
    {
    public:
        Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reusePort);
        ~Acceptor();
        void listen();

        void setNewConnectionCallback(NewConnectionCallback cb)
        {
            newConnectionCallback_ = std::move(cb);
        }
    private:
        void handleRead();

        EventLoop* loop_;
        Socket acceptSocket_;
        Channel acceptChannel_;
        NewConnectionCallback newConnectionCallback_;

        int idleFd_;
    };
}


#endif