
#include "TimerQueue.h"
#include "Logging.h"
#include "EventLoop.h"
#include <sys/timerfd.h>
#include <string.h>
using namespace CEventLoop;

static int CreateTimerFd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_SYSERR << "Failed in timerfd_create";
    }
    return timerfd;
}

static timespec HowMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microsecondsSinceEpoch() - Timestamp::now().microsecondsSinceEpoch();
    if (microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::microsecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::microsecondsPerSecond) * 1000);
    return ts;
}


static void ResetTimerFd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));

    newValue.it_value = HowMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
}



TimerQueue::TimerQueue(EventLoop* loop) : timerfd_(CreateTimerFd()), timerfdChannel_(loop, timerfd_)
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{

}

TimerId TimerQueue::addTimer(TimerCallback cb, Timestamp when, double interval)
{
    auto timerPtr = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timerPtr));

    return TimerId(timerPtr, timerPtr->numCreated());
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged)
        ResetTimerFd(timerfd_, timer->expiration());
}

bool TimerQueue::insert(Timer* timer)
{
    loop_->assertInLoopThread();
    Timestamp when = timer->expiration();
    auto it = std::begin(timers_);

    bool earliestChanged = false;
    if (it == timers_.end() || when < it->first)
    {
        earliestChanged = true;
    }

    timers_.insert({ when, std::unique_ptr<Timer>(timer) });

    return earliestChanged;
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    Timestamp now = Timestamp::now();
    ResetTimerFd(timerfd_, now);

    auto end = timers_.lower_bound(now);
    if (end == timers_.end())
    {
        LOG_SYSERR << "No timer";
    }

    for (auto it = timers_.begin(); it != end; it++)
    {
        it->second->run();
    }

    timers_.erase(timers_.begin(), end);
}
