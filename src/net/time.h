#ifndef _EXPLORER_TIME_H_
#define _EXPLORER_TIME_H_

#include <sys/time.h>

namespace Explorer {

class Time
{
public:
        static int64_t Now() { return NowMicro() / 1000; }

        static int64_t NowMicro()
        {
                struct timeval tv;
                gettimeofday(&tv, 0);
                return int64_t(tv.tv_sec) * 1000000 + tv.tv_usec;
        }

};

} // namespace Explorer

#endif // _EXPLORER_TIME_H_
