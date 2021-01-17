
#include "../CEventLoop.h"

using namespace CEventLoop;

int main()
{
    return 0;
    EventLoop loop;
   
    TcpClient client(&loop, InetAddress(2007), "TcpClient");

    client.connect();
    loop.loop();

}