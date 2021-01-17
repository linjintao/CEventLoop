#include "Poller.h"
#include "Logging.h"
#include "Channel.h"
#include <assert.h>

using namespace CEventLoop;

Poller::Poller(EventLoop* loop) : loop_(loop)
{


}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
    int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);

    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        LOG_TRACE << numEvents << " events happended";

    }
    else if (numEvents == 0)
    {
        LOG_TRACE << "nothing happended";
    }
    else
    {
        LOG_SYSERR << "Poller::poll()";
    }

    return Timestamp();
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for (int i = 0; i < pollfds_.size()  && numEvents > 0; ++i)
    {
        if (pollfds_[i].revents > 0)
        {
            numEvents--;

            auto ch = channels_.find(pollfds_[i].fd);
            assert(ch != channels_.end());

            Channel* channel = ch->second;
            channel->setActiveEvents(pollfds_[i].revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel* channel)
{
    if (channel->index() < 0)
    {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size() - 1);
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    }
    else
    {
        int idx = channel->index();
        struct pollfd* pfd = &pollfds_[idx];
        pfd->fd = channel->fd();
        pfd->events = static_cast<short>(channel->events());
        pfd->revents = 0;
    }
}

void Poller::removeChannel(Channel* channel)
{
    if (channels_.find(channel->fd()) == channels_.end())
        return;
    int idx = channel->index();
    struct pollfd& pfd = pollfds_[idx];
    channels_.erase(pfd.fd);
    if (idx == pollfds_.size() - 1)
    {
        pollfds_.pop_back();
    }
    else
    {
        int channelAtEnd = pollfds_.back().fd;
        std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);

        // ??
        if (channelAtEnd < 0)
        {
            channelAtEnd = -channelAtEnd - 1;
        }

        channels_[channelAtEnd]->set_index(idx);
        pollfds_.pop_back();
    }

}