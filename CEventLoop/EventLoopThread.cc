#include "EventloopThread.h"
#include "EventLoop.h"

CEventLoop::EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name) : callback_(cb), thread_(nullptr)
{
}

CEventLoop::EventLoopThread::~EventLoopThread()
{
    thread_->join();
}

CEventLoop::EventLoop* CEventLoop::EventLoopThread::startLoop()
{
    thread_.reset(new std::thread{ std::bind(&EventLoopThread::threadFunc,this) });
    EventLoop* loop = nullptr;
    {
        std::unique_lock lock(mutex_);
        while (loop_ == nullptr)
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }

    return loop;
}

void CEventLoop::EventLoopThread::threadFunc()
{
    EventLoop loop;
    if (callback_)
        callback_(&loop);

    {
        std::lock_guard lock(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }

    loop.loop();

    {
        std::scoped_lock lock(mutex_);
        loop_ = nullptr;
    }
}
