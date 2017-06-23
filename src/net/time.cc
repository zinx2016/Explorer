#include "time.h"
#include <sys/time.h>

namespace Explorer {

Time::Time()
        : microSeconds_(0)
{ }

Time::Time(int64_t micoSeconds)
        : microSeconds_(micoSeconds)
{
}

Time::self
Time::now()
{
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        int64_t seconds = tv.tv_sec;
        return Time(seconds * MicroSecondsPerSecond + tv.tv_usec);
}

int64_t
Time::microSeconds()
{
        return microSeconds_;
}


} // namespace Explorer
