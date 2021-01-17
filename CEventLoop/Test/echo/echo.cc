
#include "echo.h"
#include <string>

EchoServer::EchoServer(CEventLoop::EventLoop* loop, const CEventLoop::InetAddress& listenAddr) : server_(loop, listenAddr, "EchoServer")
{
    server_.setConnectionCallback([&](const CEventLoop::TcpConnectionPtr& conn) {this->onConnection(conn); });
    server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void EchoServer::start()
{
    server_.start();
}

void EchoServer::onConnection(const CEventLoop::TcpConnectionPtr& conn)
{
    LOG_INFO << "EchoServer - onConnection[" << conn->name() << "]";
}

void EchoServer::onMessage(const CEventLoop::TcpConnectionPtr& conn, CEventLoop::Buffer* buf, CEventLoop::Timestamp time)
{
    LOG_INFO << "EchoServer - onMessage[" << conn->name() << "]: readableBytes " << buf->readableBytes() ;
    std::string message = buf->retrieveAllAsString();
    int num = std::atoi(message.c_str());
    if (num == 1000)
        server_.stop();
    else
        conn->sendInLoop(message.data(), message.length());
}

EchoClient::EchoClient(CEventLoop::EventLoop* loop, const CEventLoop::InetAddress& serverAddr)
    :client_(loop, serverAddr, "EchoClient")
{
    client_.setMessageCallback(std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    client_.setConnectionCallback([&](const CEventLoop::TcpConnectionPtr& conn) {this->onConnection(conn); });
    client_.setCloseCallback([&](const CEventLoop::TcpConnectionPtr) {client_.stop(); });
    client_.connect();
}

void EchoClient::onConnection(const CEventLoop::TcpConnectionPtr& conn)
{
    std::string str("0");
    conn->sendInLoop(str.data(), str.length());
}

void EchoClient::onMessage(const CEventLoop::TcpConnectionPtr& conn, CEventLoop::Buffer* buf, CEventLoop::Timestamp time)
{
    LOG_INFO << "EchoClient - onMessage[" << conn->name() << "]: readableBytes " << buf->readableBytes();
    std::string message = buf->retrieveAllAsString();
    int num = std::atoi(message.c_str());
    std::string str = std::to_string(++num);
    conn->sendInLoop(str.data(), str.length());
}
