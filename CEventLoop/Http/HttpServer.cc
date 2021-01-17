#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpRequest.h"


using namespace CEventLoop;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& addr) : server_(loop, addr, "HttpServer")
{
    server_.setConnectionCallback([&](const TcpConnectionPtr conn) {this->onConnection(conn); });
    server_.setMessageCallback([&](const TcpConnectionPtr conn, Buffer* buf, Timestamp time) {this->onMessage(conn, buf, time); });
}

void CEventLoop::HttpServer::start()
{
    server_.setThreadNum(1);
    server_.start();
}

void CEventLoop::HttpServer::onConnection(const TcpConnectionPtr conn)
{
    if (conn->state() == States::Connected)
    {
        auto httpContext = std::make_shared<HttpContext>();
        conn->setContext(httpContext);
    }
}

void CEventLoop::HttpServer::onMessage(const TcpConnectionPtr conn, Buffer* buf, Timestamp time)
{
    auto httpContext = reinterpret_cast<HttpContext*>(conn->getContext().get());
    if (!httpContext->parseRequest(buf, time))
    {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (httpContext->gotAll())
    {
        onRequest(conn, httpContext->getRequest());
    }

}

void CEventLoop::HttpServer::onRequest(const TcpConnectionPtr conn, HttpRequest* request)
{
    conn->send("HTTP/1.1 200 OK\r\n\r\n<html>Hello World!</html>");
    conn->shutdown();
}
