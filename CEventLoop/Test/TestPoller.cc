#include "../Poller.h"
#include <stdio.h>
#include "TestAssert.h"

void DoTesting()
{
    Ttttttttt();
}
int main()
{
    CEventLoop::Poller poller(nullptr);
    try
    {
        DoTesting();
    }
    catch (CEventLoop::Exception e)
    {
        printf("%s\n", e.what());
        printf("%s\n", e.stackTrace());
        return 1;
    }
    return 0;
}