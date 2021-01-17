

#include "Connector.h"
#include "EventLoop.h"
#include "Logging.h"
#include "SocketsOps.h"
#include <string.h>

using namespace CEventLoop;


Connector::Connector(EventLoop* loop, const InetAddress& serverAddr) : loop_(loop), serverAddr_(serverAddr)
{

}

Connector::~Connector()
{

}


void Connector::start()
{
    connect_ = true;
    loop_->runInLoop([&]() {this->startInLoop(); });
}

void Connector::restart()
{

}

void Connector::stop()
{

}

int CEventLoop::Connector::removeAndResetChannel()
{
    channel_->disableAll();
    channel_->remove();
    int sockfd = channel_->fd();

    loop_->queueInLoop([&]() {channel_.reset(); });
    return sockfd;
}

void Connector::startInLoop()
{
    loop_->assertInLoopThread();
    if (connect_)
    {
        connect();
    }
    else
    {
        LOG_DEBUG << "do not connect";
    }
}

void Connector::handleWrite()
{
    LOG_TRACE << "Connector::handleWrite " << (int)state_;
    if (state_ == States::Connecting)
    {
        int sockfd = removeAndResetChannel();
        int err = SocketsOps::getSocketError(sockfd);
        if (err)
        {
            char errornobuf[512] = "";
            strerror_r(err, errornobuf, sizeof(errornobuf));
            LOG_INFO << "Connector::handleWrite - SO_ERROR = " << err << " " << (const char*)errornobuf;
            retry(sockfd);
        }
        else if (SocketsOps::IsSelfConnect(sockfd))
        {
            LOG_INFO << "Connector::handleWrite - Self connect";
            retry(sockfd);
        }
        else
        {
            state_ = States::Connected;
            if (connect_)
            {
                newConnectionCallback_(sockfd);
            }
            else
            {
                SocketsOps::Close(sockfd);
            }
        }
    }
}

void Connector::handleError()
{

}

void Connector::connecting(int fd)
{
    channel_.reset(new Channel(loop_, fd));
    channel_->setWriteCallback([&]() { return this->handleWrite(); });
    channel_->setErrorCallback([&]() { return this->handleError(); });

    state_ = States::Connecting;
    channel_->enableWriting();

}

void Connector::retry(int fd)
{
    SocketsOps::Close(fd);
    state_ = States::Disconnected;
    if (connect_)
    {

    }
}

void Connector::connect()
{
    int sockfd = SocketsOps::CreateNonblocking(serverAddr_.family());
    int ret = SocketsOps::Connect(sockfd, serverAddr_.getSockAddr());
    int savedErrno = (ret == 0) ? 0 : errno;
    switch (savedErrno)
    {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
        connecting(sockfd);
        break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
        retry(sockfd);
        break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
        LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
        SocketsOps::Close(sockfd);
        break;

    default:
        LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
        SocketsOps::Close(sockfd);
        // connectErrorCallback_();
        break;
    }
}