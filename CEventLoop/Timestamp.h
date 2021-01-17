#ifndef TIMESTAMP_H_INCLUDED
#define TIMESTAMP_H_INCLUDED

#include <stdint.h>


namespace CEventLoop
{

    class Timestamp
    {
    public:
        Timestamp() : microSecondsSinceEpoch_(0)
        {

        }


    private:
        int64_t microSecondsSinceEpoch_;
    };

}



#endif