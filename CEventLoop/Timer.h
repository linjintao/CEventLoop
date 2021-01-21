#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include "Callback.h"
#include "Timestamp.h"
#include <atomic>

namespace CEventLoop
{
    class Timer
    {
    public:
        Timer(TimerCallback cb, Timestamp when, double interval)
            : callback_(cb),
              expiration_(when),
              interval_(interval),
              repeat_(interval_ > 0),
              sequence_(s_numCreated.fetch_add(1))
        {
        }

        void run() const
        {
            callback_();
        }

        void restart(Timestamp now);

        Timestamp expiration() const { return expiration_; }

        static int64_t numCreated() { return s_numCreated.load(); }
    private:
        const TimerCallback callback_;
        Timestamp expiration_;
        const double interval_;
        const bool   repeat_;
        const int64_t sequence_;
        static std::atomic<int64_t> s_numCreated;
    };
}



#endif