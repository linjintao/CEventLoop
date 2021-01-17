
#include "../CEventLoop.h"
#include "../EventLoopThreadPool.h"

void print(CEventLoop::EventLoop* loop)
{
    LOG_INFO << "thread " << std::this_thread::get_id() << " is starting";
}


int main()
{
    CEventLoop::Logger::setLogLevel(CEventLoop::LogLevel::TRACE);
    CEventLoop::EventLoop baseLoop;
    CEventLoop::EventLoopThreadPool eventThreadPool(&baseLoop, "EventThreadPool");
    eventThreadPool.setThreadNum(3);
    eventThreadPool.start(print);
    sleep(5);
    eventThreadPool.quit();
    return 0;
}