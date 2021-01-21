#ifndef TIME_QUEUE_H_INCLUDED
#define TIME_QUEUE_H_INCLUDED

#include "Channel.h"
#include "Timer.h"
#include <map>

namespace CEventLoop
{
    class EventLoop;

    class TimerId
    {
    public:
        friend class TimerQueue;

        TimerId(Timer* timer, int64_t seq) : timer_(timer), sequence_(seq)
        {

        }

    private:
        Timer* timer_;
        int64_t sequence_;
    };


    class TimerQueue
    {
    public:
        TimerQueue(EventLoop* loop);
        ~TimerQueue();
        TimerId addTimer(TimerCallback cb, Timestamp when, double interval);

    private:
        void handleRead();
        void addTimerInLoop(Timer* timer);
        bool insert(Timer* timer);

        std::map<Timestamp, std::unique_ptr<Timer>> timers_;
        const int timerfd_;
        EventLoop* loop_;
        Channel timerfdChannel_;
    };

}



#endif