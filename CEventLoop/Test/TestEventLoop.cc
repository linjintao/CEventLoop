
#include "../EventLoop.h"
#include "TestAssert.h"
#include "../Logging.h"
#include <thread>

void ThreadFunc()
{
    CEventLoop::EventLoop loop;

    loop.loop();
}


int main()
{
    LOG_INFO << "Testing";
    std::thread t(ThreadFunc);
    CEventLoop::EventLoop loop;

    loop.loop();
    t.join();

    return 0;
}