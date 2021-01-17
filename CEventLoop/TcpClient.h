#ifndef TCP_CLIENT_H_INCLUDED
#define TCP_CLIENT_H_INCLUDED

#include "Callback.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Timestamp.h"
#include <memory>
#include <string>

namespace CEventLoop
{
    class Connector;
    using ConnectorPtr = std::shared_ptr<Connector>;

    class TcpClient
    {
    public:
        TcpClient(EventLoop* loop, const InetAddress& serverAddr, const std::string name);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        void newConnection(int sockfd);

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

        void defaultConnectionCallback(const TcpConnectionPtr connPtr);
        void defaultMessageCallback(const TcpConnectionPtr, Buffer*, Timestamp);

    private:

        EventLoop* loop_;
        ConnectorPtr connector_;
        const std::string name_;
        int nextConnId_ = 1;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        CloseCallback closeCallback_;

        TcpConnectionPtr connection_;
    };
}



#endif