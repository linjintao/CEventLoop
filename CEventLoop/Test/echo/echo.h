#ifndef ECHO_H_INCLUDED
#define ECHO_H_INCLUDED

#include "../../CEventLoop.h"


class EchoServer
{
public:
    EchoServer(CEventLoop::EventLoop* loop, const CEventLoop::InetAddress& listenAddr);

    void start();
    
private:
    void onConnection(const CEventLoop::TcpConnectionPtr& conn);

    void onMessage
    (
        const CEventLoop::TcpConnectionPtr& conn,
        CEventLoop::Buffer* buf,
        CEventLoop::Timestamp time
    );

    CEventLoop::TcpServer server_;
};

class EchoClient
{
public:
    EchoClient(CEventLoop::EventLoop* loop, const CEventLoop::InetAddress& serverAddr);

private:
    void onConnection(const CEventLoop::TcpConnectionPtr& conn);

    void onMessage
    (
        const CEventLoop::TcpConnectionPtr& conn,
        CEventLoop::Buffer* buf,
        CEventLoop::Timestamp time
    );

    CEventLoop::TcpClient client_;
};

#endif