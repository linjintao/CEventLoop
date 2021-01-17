
#include "../EventLoop.h"
#include "TestAssert.h"
#include "../Logging.h"
#include <thread>

void ThreadFunc(CEventLoop::EventLoop** loopOut)
{
    CEventLoop::EventLoop loop;
    *loopOut = &loop;
    loop.loop();
}


int main()
{
    LOG_INFO << "Testing";
    return 0;
    CEventLoop::EventLoop* loopThread = nullptr;
    std::thread t(ThreadFunc, &loopThread);
    CEventLoop::EventLoop loop;

    loop.loop();
    t.join();

    return 0;
}