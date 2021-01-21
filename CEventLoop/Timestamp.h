#ifndef TIMESTAMP_H_INCLUDED
#define TIMESTAMP_H_INCLUDED

#include <stdint.h>


namespace CEventLoop
{

    class Timestamp
    {
    public:
        Timestamp() = default;
        Timestamp(int64_t epoch) : microsecondsSinceEpoch_(epoch)
        {

        }

        int64_t microsecondsSinceEpoch() const {
            return microsecondsSinceEpoch_;
        }

        inline bool operator<(Timestamp rhs) const
        {
            return this->microsecondsSinceEpoch_ < rhs.microsecondsPerSecond;
        }

        static Timestamp now();
        static const int microsecondsPerSecond;
    private:


        int64_t microsecondsSinceEpoch_;
    };

}



#endif