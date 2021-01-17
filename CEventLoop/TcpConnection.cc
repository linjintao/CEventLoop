#include "TcpConnection.h"
#include "Logging.h"
#include "SocketsOps.h"
#include <string.h>

using namespace CEventLoop;
using namespace std;


TcpConnection::TcpConnection
(
    CEventLoop::EventLoop* ioLoop,
    const string& name,
    int sockfd,
    const CEventLoop::InetAddress& localAddr,
    const CEventLoop::InetAddress& peerAddr
) : ioLoop_(ioLoop), name_(name), channel_(new Channel(ioLoop, sockfd))
{
    channel_->setReadCallback([&](Timestamp t) {this->handleRead(t); });
    channel_->setCloseCallback([&]() {this->handleClose(); });
    channel_->setErrorCallback([&]() {this->handleError(); });
    channel_->setWriteCallback([&]() {this->handleWrite(); });
}

TcpConnection::~TcpConnection()
{
    if(state_ == States::Connected)
        shutdown();
    // connection is destructed by acceptor thread, but channel_ should not be remove by it ?
    //channel_->remove();
    LOG_INFO << "~TcpConnection()";
}

void TcpConnection::send(const char* msg)
{
    sendInLoop(msg, strlen(msg));
}

void TcpConnection::send(Buffer* buf)
{
    if (state_ == States::Connected)
    {
        if (ioLoop_->isInLoopThread())
        {
            sendInLoop(buf->peek(), buf->readableBytes());
        }
        else
        {

        }
    }

}

void TcpConnection::sendInLoop(const void* data, size_t len)
{
    ioLoop_->assertInLoopThread();
    if (state_ == States::Disconnected)
    {
        LOG_SYSERR << "disconnected, give up writing";
        return;
    }
    ssize_t nwrote = 0;
    ssize_t remaining = len;
    if (outputBuffer_.readableBytes() == 0)
    {
        nwrote = SocketsOps::Write(channel_->fd(), data, len);
        if (nwrote > 0)
        {
            remaining -= nwrote;
            if (remaining == 0 && writeCompleteCallback_)
                writeCompleteCallback_(shared_from_this());
        }
    }

    if (remaining > 0)
    {
        size_t oldLen = outputBuffer_.readableBytes();
        if (oldLen + remaining >= highWaterMark_ && oldLen < highWaterMark_)
        {
            highWaterMarkCallback_(shared_from_this(), oldLen + remaining);
        }

        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if(!channel_->isWriting())
            channel_->enableWriting();
    }
}

void TcpConnection::connectEstablished()
{
    ioLoop_->assertInLoopThread();
    channel_->tie(shared_from_this());
    channel_->enableReading();
    state_ = States::Connected;
    if (connectionCallback_)
        connectionCallback_(shared_from_this());
}

void CEventLoop::TcpConnection::connectDestroyed()
{
    ioLoop_->assertInLoopThread();
    if (state_ == States::Connected)
    {
        state_ = States::Disconnected;
        channel_->disableAll();
        connectionCallback_(shared_from_this());
        SocketsOps::Close(channel_->fd());
    }
    channel_->remove();
}

void CEventLoop::TcpConnection::shutdown()
{
    ioLoop_->assertInLoopThread();
    SocketsOps::ShutdownWrite(channel_->fd());
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
    ioLoop_->assertInLoopThread();
    int err = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &err);
    if (n > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
    }
    else if (n == 0)
    {
        handleClose();
    }
    else
    {

    }

    
}

void TcpConnection::handleClose()
{
    ioLoop_->assertInLoopThread();
    LOG_INFO << "TcpConnection::handleClose  fd = " << channel_->fd() << " state = ";

    state_ = States::Disconnected;
    channel_->disableAll();
    TcpConnectionPtr self(shared_from_this());
    connectionCallback_(self);
    closeCallback_(self);

    LOG_INFO << "TcpConnection::handleClose() RefCount=" << self.use_count();
}

void TcpConnection::handleError()
{

}

void CEventLoop::TcpConnection::handleWrite()
{
}
