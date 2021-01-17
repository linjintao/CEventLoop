#ifndef TCP_CONNECTION_H_INCLUDED
#define TCP_CONNECTION_H_INCLUDED

#include "EventLoop.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Callback.h"
#include "Buffer.h"
#include <memory>

namespace CEventLoop
{

    class TcpConnection : public std::enable_shared_from_this<TcpConnection>
    {
    public:
        // Created by acceptor loop
        TcpConnection(CEventLoop::EventLoop* ioLoop, const std::string& name, int sockfd, const CEventLoop::InetAddress& localAddr, const CEventLoop::InetAddress& peerAddr);
        // Desconstructed by acceptor loop
        ~TcpConnection();

        void setConnectionCallback(ConnectionCallback cb)
        {
            connectionCallback_ = std::move(cb);
        }

        void setMessageCallback(MessageCallback cb)
        {
            messageCallback_ = std::move(cb);
        }

        void setCloseCallback(CloseCallback cb)
        {
            closeCallback_ = std::move(cb);
        }

        void setWriteCallback(EventCallback cb)
        {
            writeCallback_ = std::move(cb);
        }

        void setHighWaterMarkCallback(HighWaterMarkCallback cb)
        {
            highWaterMarkCallback_ = std::move(cb);
        }

        // ioloop strict functions 
        void send(const char* msg);
        void send(Buffer* buf);
        void sendInLoop(const void* data, size_t len);
        void connectEstablished();
        void connectDestroyed();
        const std::string& name() const { return name_; }
        void shutdown();
        ///////////////////////

        EventLoop* getIOLoop()
        {
            return ioLoop_;
        }

        std::shared_ptr<void> getContext()
        {
            return context_;
        }

        void setContext(std::shared_ptr<void> context)
        {
            context_ = context;
        }

        States state() const
        {
            return state_;
        }

    private:
        void handleRead(Timestamp receiveTime);
        void handleClose();
        void handleError();
        void handleWrite();


        States state_ = States::Disconnected;
        EventLoop* ioLoop_;
        const std::string name_;
        std::unique_ptr<Channel> channel_;
        ConnectionCallback connectionCallback_;
        MessageCallback    messageCallback_;
        CloseCallback      closeCallback_;
        EventCallback      writeCallback_;
        WriteCompleteCallback writeCompleteCallback_;

        size_t highWaterMark_ = 64*1024*1024;
        HighWaterMarkCallback highWaterMarkCallback_;
        Buffer inputBuffer_;
        Buffer outputBuffer_;

        std::shared_ptr<void> context_;
    };

}

#endif