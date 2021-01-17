#ifndef CHANNEL_H_INCLUDED
#define CHANNEL_H_INCLUDED

#include <functional>
#include <poll.h>
#include <memory>
#include "Timestamp.h"
#include "Callback.h"

namespace CEventLoop
{
    class EventLoop;


    using ReadEventCallback = std::function<void(Timestamp)>;

    class Channel
    {
    public:

        enum class Event : short {
            NoneEvent = 0,
            ReadEvent = POLLIN | POLLPRI,
            WriteEvent = POLLOUT,
            ReadWriteEvent
        };

        Channel(EventLoop* loop, int fd);

        void setReadCallback(const ReadEventCallback cb)
        {
            readCallback_ = std::move(cb);
        }

        void setWriteCallback(const EventCallback cb)
        {
            writeCallback_ = std::move(cb);
        }

        void setCloseCallback(const EventCallback cb)
        {
            closeCallback_ = std::move(cb);
        }

        void setErrorCallback(const EventCallback cb)
        {
            errorCallback_ = std::move(cb);
        }

        void enableWriting();
        void enableReading();
        void disableAll();
        void remove();

        bool isWriting()
        {
            return static_cast<short>(events_) & POLLOUT;
        }

        int fd() const { return fd_; }

        void handleEvent(Timestamp pollTime);
        void setActiveEvents(int activeEvents)
        {
            activeEvents_ = activeEvents;
        }

        void set_index(int idx) { index_ = idx; }
        int index() const { return index_; }

        Event events() const { return events_; }
        EventLoop* ownerLoop() { return loop_; }
        void tie(const std::shared_ptr<void>);
        std::string eventsToString(int fd, int ev);
    private:
        bool isAddToLoop_ = false;
        EventLoop*      loop_;
        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback errorCallback_;
        EventCallback closeCallback_;
        int        index_ = -1;
        int        fd_;
        int        activeEvents_;
        Event      events_;

        std::weak_ptr<void> tie_;
    };

}


#endif

