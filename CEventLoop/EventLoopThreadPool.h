#ifndef EVENT_LOOP_THREAD_POOL_H_INCLUDED
#define EVENT_LOOP_THREAD_POOL_H_INCLUDED

#include <vector>
#include <memory>
#include <string>
#include "EventloopThread.h"
#include "Callback.h"

namespace CEventLoop
{
    class EventLoop;
    class EventLoopThread;

    class EventLoopThreadPool
    {
    public:
        EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);

        void setThreadNum(int num) { numThreads_ = num; }
        void start(const ThreadInitCallback cb = ThreadInitCallback());
        void quit();
        EventLoop* getNextLoop();
    private:
        EventLoop* baseLoop_;
        const std::string name_;
        bool started_;
        int numThreads_;
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;
    };

}

#endif