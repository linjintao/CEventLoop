#include "TcpServer.h"
#include "EventLoop.h"
#include "SocketsOps.h"
#include "Logging.h"
#include <atomic>

using namespace std;
using namespace CEventLoop;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const string& name) :
    loop_(loop),
    ipPort_(listenAddr.toIpPort()),
    name_(name),
    threadPool_(new EventLoopThreadPool(loop, name + "ThreadPool")),
    acceptor_(new Acceptor(loop, listenAddr, true))
{
    acceptor_->setNewConnectionCallback([&](int sockfd, const InetAddress& addr) { this->newConnection(sockfd, addr); });
    messageCb_ = [&](const TcpConnectionPtr connPtr, Buffer* buffer, Timestamp time) {this->defaultMessageCallback(connPtr, buffer, time); };
    connectionCb_ = [&](const TcpConnectionPtr connPtr) {this->defaultConnectionCallback(connPtr); };
}

CEventLoop::TcpServer::~TcpServer()
{
    LOG_INFO << "TcpServer::~TcpServer[" << name_ << "] - Remove " << connections_.size() << " connections";
}

void TcpServer::start()
{
    if (!started_.test_and_set())
    {
        threadPool_->start();
        loop_->runInLoop([&]() {acceptor_->listen(); });
    }
}

void TcpServer::stop()
{
    LOG_INFO << "TcpServer::stop[" << name_ << "]";
    loop_->queueInLoop([&]() {loop_->quit(); });
}

void CEventLoop::TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    loop_->assertInLoopThread();
    EventLoop* ioLoop = threadPool_->getNextLoop();

    char buf[64];
    snprintf(buf, sizeof(buf), "-%s#%d", ipPort_.c_str(), nextConnId_++);
    string name = name_ + buf;

    LOG_INFO << "TcpServer::newConnection [" << name_ << "] - new connection [" << name << "] from " << peerAddr.toIpPort();

    InetAddress localAddr(SocketsOps::GetLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(ioLoop, name, sockfd, localAddr, peerAddr));

    // all the callback functions will be called by ioloop, they must be thread safe
    conn->setConnectionCallback(connectionCb_);
    conn->setMessageCallback(messageCb_);
    conn->setCloseCallback([&](TcpConnectionPtr connPtr) {this->removeConnectionInLoop(connPtr); });
    ioLoop->runInLoop([=]() {conn->connectEstablished(); });
    connections_[name] = conn;
}

void CEventLoop::TcpServer::removeConnectionInLoop(const TcpConnectionPtr conn)
{
    loop_->queueInLoop([=]() {this->removeConnection(conn); });
}

void TcpServer::removeConnection(const TcpConnectionPtr conn)
{
    // called by ioloop ?
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnection[" << name_ << "] - connection " << conn->name();
    EventLoop* ioLoop = conn->getIOLoop();
    ioLoop->queueInLoop([=]() {conn->connectDestroyed(); });
    connections_.erase(conn->name());
}

void CEventLoop::TcpServer::defaultConnectionCallback(const TcpConnectionPtr connPtr)
{
    LOG_INFO << "TcpServer::defaultConnectionCalback[" << name_ << "] - connection " << connPtr->name();
}

void CEventLoop::TcpServer::defaultMessageCallback(const TcpConnectionPtr connPtr, Buffer* buf, Timestamp)
{
    LOG_INFO << "TcpServer::defaultMessageCallback[" << name_ << "] - connection " << connPtr->name();
    buf->retrieveAll();
}
