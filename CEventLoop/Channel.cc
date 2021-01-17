
#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"
#include <sstream>

using namespace CEventLoop;

Channel::Channel(EventLoop* loop, int fd) : loop_(loop), fd_(fd)
{

}

void Channel::handleEvent(Timestamp pollTime)
{
    LOG_INFO << "Channel::handleEvent - " << eventsToString(fd_, activeEvents_);
    if ((activeEvents_ & POLLHUP) && !(activeEvents_ & POLLIN))
    {
        if (closeCallback_)
            closeCallback_();
    }

    if (activeEvents_ & POLLNVAL)
    {
        LOG_WARN << "fd = " << fd_ << " Channel:handleEvent - POLLNAL";
    }

    if (activeEvents_ & (POLLERR | POLLNVAL))
    {
        if (errorCallback_)
            errorCallback_();
    }

    if (activeEvents_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
        if (readCallback_)
            readCallback_(pollTime);
    }

    if (activeEvents_ & POLLOUT)
    {
        if (writeCallback_)
            writeCallback_();
    }
    activeEvents_ = 0;

}

void Channel::enableWriting()
{
    events_ = static_cast<Event>(static_cast<short>(Event::WriteEvent) | static_cast<short>(events_));
    loop_->updateChannel(this);
}

void CEventLoop::Channel::enableReading()
{
    events_ = static_cast<Event>(static_cast<short>(Event::ReadEvent) | static_cast<short>(events_));
    loop_->updateChannel(this);
}

void CEventLoop::Channel::disableAll()
{
    events_ = Event::NoneEvent;
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void> obj)
{
    tie_ = obj;
}

std::string CEventLoop::Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & POLLIN)
        oss << "IN ";
    if (ev & POLLPRI)
        oss << "PRI ";
    if (ev & POLLOUT)
        oss << "OUT ";
    if (ev & POLLHUP)
        oss << "HUP ";
    if (ev & POLLRDHUP)
        oss << "RDHUP ";
    if (ev & POLLERR)
        oss << "ERR ";
    if (ev & POLLNVAL)
        oss << "NVAL";
    
    return oss.str();
}
