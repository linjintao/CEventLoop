#ifndef EVENT_LOOP_THREAD_INCLUDED
#define EVENT_LOOP_THREAD_INCLUDED

#include <functional>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "Callback.h"

namespace CEventLoop
{
    class EventLoop;

    class EventLoopThread
    {
    public:

        EventLoopThread(const ThreadInitCallback& cb,
                        const std::string& name = "EventLoopThread");
        ~EventLoopThread();

        EventLoop* startLoop();

        EventLoopThread(const EventLoopThread&) = delete;
        const EventLoopThread& operator=(const EventLoopThread&) = delete;

    private:
        void threadFunc();

        std::unique_ptr<std::thread> thread_;
        EventLoop* loop_;
        std::mutex mutex_;
        std::condition_variable cond_;
        ThreadInitCallback callback_;
    };
}


#endif