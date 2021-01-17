#ifndef HTTP_SERVER_H_INCLUDED
#define HTTP_SERVER_H_INCLUDED

#include "../TcpServer.h"
#include "HttpRequest.h"

namespace CEventLoop
{
    class EventLoop;
    class HttpServer
    {
    public:
        HttpServer(EventLoop *loop, const InetAddress& addr);
        void start();
        virtual void onConnection(const TcpConnectionPtr conn);
        virtual void onMessage(const TcpConnectionPtr, Buffer*, Timestamp);
        virtual void onRequest(const TcpConnectionPtr conn, HttpRequest* context);
    private:
        TcpServer server_;
    };
}

#endif