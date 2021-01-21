
#include "Timestamp.h"


#include "Timestamp.h"
#include <sys/time.h>
using namespace CEventLoop;

const int Timestamp::microsecondsPerSecond = 1000 * 1000;

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return Timestamp(tv.tv_sec * Timestamp::microsecondsPerSecond + tv.tv_usec);
}