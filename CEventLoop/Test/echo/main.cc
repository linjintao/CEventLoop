
#include "echo.h"
#include <unistd.h>

void StartEchoServer()
{
    CEventLoop::EventLoop loop;
    CEventLoop::InetAddress listenAddr(2007);
    EchoServer server(&loop, listenAddr);
    server.start();
    LOG_INFO << "EchoServer: " << loop.getEventLoopOfCurrentThread();
    loop.loop();
}

void StartEchoClient()
{
    CEventLoop::EventLoop loop;
    CEventLoop::InetAddress serverAddr(2007);
    EchoClient client(&loop, serverAddr);
    LOG_INFO << "EchoClient: " << loop.getEventLoopOfCurrentThread();
    loop.loop();
}



int main()
{
    CEventLoop::Logger::setLogLevel(CEventLoop::LogLevel::TRACE);
    LOG_INFO << "pid = " << (int)getpid();
    std::thread serverThread(StartEchoServer);
    std::thread clientThread(StartEchoClient);

    serverThread.join();
    clientThread.join();
    return 0;
}