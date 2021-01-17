
#include "../CEventLoop.h"

using namespace CEventLoop;

int main()
{
    EventLoop loop;
   
    TcpClient client(&loop, InetAddress(2007), "TcpClient");

    client.connect();
    loop.loop();

}