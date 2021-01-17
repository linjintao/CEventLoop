#ifndef CONNECTOR_H_INCLUDED
#define CONNECTOR_H_INCLUDED

#include <memory>
#include <functional>
#include "InetAddress.h"
#include "Channel.h"
#include "Callback.h"

namespace CEventLoop
{
    class EventLoop;


    class Connector : public std::enable_shared_from_this<Connector>
    {
    public:
        using NewConnectionCallback = std::function<void(int)>;

        Connector(EventLoop* loop, const InetAddress& serverAddr);
        ~Connector();

        void setNewConnectionCallback(const NewConnectionCallback& cb)
        {
            newConnectionCallback_ = cb;
        }

        void start();  // can be called in any thread
        void restart();  // must be called in loop thread
        void stop();  // can be called in any thread

    private:
        int removeAndResetChannel();
        void startInLoop();
        void handleWrite();
        void handleError();
        void connect();

        void connecting(int fd);
        void retry(int fd);


        EventLoop* loop_;
        InetAddress serverAddr_;
        bool connect_ = false;
        States state_ = States::Disconnected;
        std::unique_ptr<Channel> channel_;
        NewConnectionCallback newConnectionCallback_;
        int retryDelayMs_;
    };
}



#endif