


#ifndef POLLER_H_INCLUDED
#define POLLER_H_INCLUDED

#include <map>
#include <vector>
#include <poll.h>
#include "Timestamp.h"

namespace CEventLoop
{
    class Channel;
    class EventLoop;

    class Poller
    {
    public:
        using ChannelList = std::vector<Channel*>;
        Poller(EventLoop* eventLoop);
        virtual ~Poller() {};
        virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);

        virtual void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);
        void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;


    private:
        using PollFdList = std::vector<struct pollfd>;
        using ChannelMap = std::map<int, Channel*>;

        EventLoop* loop_;
        PollFdList pollfds_;
        ChannelMap channels_;
    };
}



#endif

