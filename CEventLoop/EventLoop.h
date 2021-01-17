// EventLoop.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>

// TODO: Reference additional headers your program requires here.
namespace CEventLoop
{
    class Poller;
    class Channel;

    class EventLoop
    {
    public:
        using Functor = std::function<void()>;

        EventLoop();
        ~EventLoop();
        void loop();

        bool isInLoopThread()
        {
            return std::this_thread::get_id() == threadId_;
        }
        void runInLoop(Functor cb);
        void queueInLoop(Functor cb);
        void assertInLoopThread();
        void wakeup();
        void quit();
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

        static EventLoop* getEventLoopOfCurrentThread();

        EventLoop(const EventLoop&) = delete;
        EventLoop& operator=(const EventLoop&) = delete;

    private:
        void handleRead();
        static int createWakeupEventFd();
        void abortNotInLoopThread();

        void doPendingFunctors();

        using ChannelList = std::vector<Channel*>;

        ChannelList activeChannels_;
        Channel* currentActiveChannel_ = nullptr;

        std::shared_ptr<Poller> poller_;
        bool quit_ = false;
        bool looping_ = false;
        std::thread::id threadId_;

        mutable std::mutex mutex_;
        std::vector<Functor> pendingFunctors_;
        bool callingPendingFunctors_ = false;

        int wakeupFd_;
        std::unique_ptr<Channel> wakeupChannel_;
    };
}