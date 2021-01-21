// EventLoop.cpp : Defines the entry point for the application.
//

#include <cassert>
#include <sys/eventfd.h>
#include "Poller.h"
#include "EventLoop.h"
#include "Logging.h"
#include "Channel.h"
#include "SocketsOps.h"

using namespace CEventLoop;
using namespace std;

thread_local EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop() :
    threadId_(std::this_thread::get_id()),
    poller_(new Poller(this)),
    wakeupFd_(EventLoop::createWakeupEventFd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    timerQueue_(this)
{
    LOG_TRACE << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread != nullptr)
    {
        LOG_FATAL << "Another EvenLoop " << t_loopInThisThread << " exists in this thread " << threadId_;
    }
    t_loopInThisThread = this;
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    assert(!looping_);
    t_loopInThisThread = nullptr;
    LOG_TRACE << "EventLoop::~EventLoop";
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);

    looping_ = true;

    while (!quit_)
    {
        activeChannels_.clear();
        auto pollReturnTime = poller_->poll(5 * 1000, &activeChannels_);

        for (Channel* channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime);
        }
        currentActiveChannel_ = nullptr;
        doPendingFunctors();
    }

    looping_ = false;
}

void EventLoop::assertInLoopThread()
{
    if (!isInLoopThread())
    {
        abortNotInLoopThread();
    }
}

void CEventLoop::EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = SocketsOps::Read(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_SYSERR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

int CEventLoop::EventLoop::createWakeupEventFd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }

    return evtfd;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
        << " was created in threadId_ = " << threadId_
        << ", current thread id = " << std::this_thread::get_id();
    abort();
}

void CEventLoop::EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    {
        std::scoped_lock lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    callingPendingFunctors_ = true;
    for (auto& functor : functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pendingFunctors_.push_back(std::move(cb));
    if(!isInLoopThread() || callingPendingFunctors_)
        wakeup();
}


void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = SocketsOps::Write(wakeupFd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG_SYSERR << "EventLoop::wakeup() writes " << n << " bytes instead of " << sizeof(one);
    }
}

void CEventLoop::EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}

void EventLoop::updateChannel(Channel* channel)
{
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}