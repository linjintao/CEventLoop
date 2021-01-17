#include "Acceptor.h"
#include "SocketsOps.h"
#include "EventLoop.h"
#include "Logging.h"
#include <unistd.h>
#include <fcntl.h>

using namespace CEventLoop;

Acceptor::Acceptor(EventLoop *loop, const InetAddress& listenAddr, bool reusePort)
    : loop_(loop),
      acceptSocket_(SocketsOps::CreateNonblocking(listenAddr.family())),
      acceptChannel_(loop, acceptSocket_.fd()),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback([&](Timestamp time) {this->handleRead(); });
}

CEventLoop::Acceptor::~Acceptor()
{
    acceptChannel_.disableAll();
    acceptChannel_.remove();
    ::close(idleFd_);
}

void Acceptor::listen()
{
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}

void CEventLoop::Acceptor::handleRead()
{
    loop_->assertInLoopThread();
    InetAddress peerAddr;

    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0)
    {
        if (newConnectionCallback_)
            newConnectionCallback_(connfd, peerAddr);
        else
            SocketsOps::Close(connfd);
    }
    else
    {
        LOG_SYSERR << "in Acceptor::handleRead";
        if (errno == EMFILE)
        {
            ::close(idleFd_);
            idleFd_ = ::accept(acceptSocket_.fd(), nullptr, nullptr);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }

}
