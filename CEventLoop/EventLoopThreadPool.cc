
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include <string>
#include <assert.h>

using namespace std;
using namespace CEventLoop;

CEventLoop::EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const string& name)
    : baseLoop_(baseLoop),
      name_(name),
      started_(false),
      numThreads_(0),
      next_(0)
{
}

void CEventLoop::EventLoopThreadPool::start(const ThreadInitCallback cb)
{
    assert(!started_);
    baseLoop_->assertInLoopThread();
    started_ = true;

    for (int i = 0; i < numThreads_; i++)
    {
        char buf[512+ 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        auto t = std::make_unique<EventLoopThread>(cb, std::string(buf));
        loops_.push_back(t->startLoop());
        threads_.emplace_back(std::move(t));
    }
}

void CEventLoop::EventLoopThreadPool::quit()
{
    for (auto& loop : loops_)
        loop->quit();
}


EventLoop* CEventLoop::EventLoopThreadPool::getNextLoop()
{
    baseLoop_->assertInLoopThread();
    EventLoop* loop = baseLoop_;

    if (!loops_.empty())
    {
        loop = loops_[next_++];
        if (next_ >= loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}
