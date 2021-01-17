
#include "llhttp.h"
#include "../CEventLoop.h"
#include "HttpServer.h"

using namespace CEventLoop;

int main()
{
#ifdef _DEBUG
    CEventLoop::Logger::setLogLevel(CEventLoop::LogLevel::TRACE);
#endif

    CEventLoop::EventLoop loop;

    HttpServer server(&loop, InetAddress(2002));
    server.start();
    loop.loop();
    return 0;
}