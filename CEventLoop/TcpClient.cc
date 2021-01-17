#include "TcpClient.h"
#include "Connector.h"
#include "TcpConnection.h"
#include "SocketsOps.h"
#include "Logging.h"

using namespace std;

CEventLoop::TcpClient::TcpClient
(
    EventLoop* loop,
    const InetAddress& serverAddr,
    const std::string name
) : loop_(loop), connector_(new Connector(loop, serverAddr)), name_(name)
{
    messageCallback_ = [&](const TcpConnectionPtr conn, Buffer* buf, Timestamp t) { this->defaultMessageCallback(conn, buf, t); };
    connectionCallback_ = [&](const TcpConnectionPtr conn) { this->defaultConnectionCallback(conn); };
    connector_->setNewConnectionCallback([&](int sockfd) {this->newConnection(sockfd); });
}

CEventLoop::TcpClient::~TcpClient()
{
}

void CEventLoop::TcpClient::connect()
{
    connector_->start();
}

void CEventLoop::TcpClient::disconnect()
{
}

void CEventLoop::TcpClient::stop()
{
    LOG_INFO << "TcpClient::stop[" << name_ << "]";
    loop_->queueInLoop([&]() {loop_->quit(); });
}

void CEventLoop::TcpClient::newConnection(int sockfd)
{
    loop_->assertInLoopThread();
    InetAddress peerAddr(SocketsOps::GetPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof(buf), "%s#%d", peerAddr.toIpPort().c_str(), nextConnId_++);
    string connName = name_ + buf;

    LOG_INFO << "TcpClient::newConnection[" << name_ << "] - " << connName;

    InetAddress localAddr(SocketsOps::GetLocalAddr(sockfd));
    TcpConnectionPtr connPtr(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));

    connPtr->setMessageCallback(messageCallback_);
    connPtr->setConnectionCallback(connectionCallback_);
    connPtr->setCloseCallback(closeCallback_);

    connection_ = connPtr;
    connPtr->connectEstablished();
}

void CEventLoop::TcpClient::defaultConnectionCallback(const TcpConnectionPtr connPtr)
{
    LOG_INFO << "TcpClient::defaultConnectionCallback[" << name_ << "] - " << connPtr->name();
    Buffer buf;
    buf.append("Hello World", 11);
    connPtr->send(&buf);
}

void CEventLoop::TcpClient::defaultMessageCallback(const TcpConnectionPtr connPtr, Buffer*, Timestamp)
{
    LOG_INFO << "TcpClient::defaultMessageCallback[" << name_ << "] - " << connPtr->name();
}
