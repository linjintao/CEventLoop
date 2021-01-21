#ifndef CALLBACK_H_INCLUDED
#define CALLBACK_H_INCLUDED

#include <stddef.h>
#include <memory>
#include <functional>


namespace CEventLoop
{
    class Buffer;
    class Timestamp;
    class TcpConnection;
    class EventLoop;
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    using ConnectionCallback = std::function<void(const TcpConnectionPtr)>;
    using MessageCallback = std::function<void(const TcpConnectionPtr, Buffer*, Timestamp)>;
    using CloseCallback = std::function<void(const TcpConnectionPtr)>;
    using WriteCompleteCallback = std::function<void(const TcpConnectionPtr)>;
    using EventCallback = std::function<void()>;
    using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr, size_t)>;
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    using TimerCallback = std::function<void()>;

    enum class States
    {
        Disconnected,
        Connecting,
        Connected
    };
}


#endif