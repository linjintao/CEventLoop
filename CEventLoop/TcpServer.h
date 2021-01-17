#ifndef TCP_SERVER_H_INCLUDED
#define TCP_SERVER_H_INCLUDED

#include "InetAddress.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "Callback.h"
#include "EventLoopThreadPool.h"

#include <string>
#include <memory>
#include <atomic>
#include <map>

namespace CEventLoop
{
    class EventLoop;

    class TcpServer
    {
    public:
        TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name);
        ~TcpServer();
        void start();
        void stop();
        void setThreadNum(int num) { if (num > 0) threadPool_->setThreadNum(num); }

        void setConnectionCallback(ConnectionCallback cb)
        {
            connectionCb_ = cb;
        }

        void setMessageCallback(MessageCallback cb)
        {
            messageCb_ = cb;
        }

        virtual void defaultConnectionCallback(const TcpConnectionPtr);
        virtual void defaultMessageCallback(const TcpConnectionPtr, Buffer*, Timestamp);

    private:
        void newConnection(int sockfd, const InetAddress& peerAddr);
        void removeConnectionInLoop(const TcpConnectionPtr conn);
        void removeConnection(const TcpConnectionPtr conn);

        EventLoop* loop_; // acceptor loop
        const std::string ipPort_;
        const std::string name_;
        std::unique_ptr<Acceptor> acceptor_;
        std::atomic_flag started_ = false;

        ConnectionCallback connectionCb_;
        MessageCallback messageCb_;

        int nextConnId_ = 1;

        using ConnectionMap = std::map<std::string, TcpConnectionPtr>;
        ConnectionMap connections_;
        std::shared_ptr<EventLoopThreadPool> threadPool_;
    };

}


#endif