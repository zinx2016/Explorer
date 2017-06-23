#ifndef _EXPLORER_TIME_H_
#define _EXPLORER_TIME_H_

#include <stdint.h>

namespace Explorer {

class Time
{
public:
        typedef Time self;

        Time();

        explicit Time(int64_t microSeconds);

        static self now();

        int64_t microSeconds();

        static const int MicroSecondsPerSecond = 1000000;

private:
        int64_t microSeconds_;
};

inline Time
addTime(Time tv, double plus)
{
        int64_t add = static_cast<int64_t>(plus * Time::MicroSecondsPerSecond);
        return Time(tv.microSeconds() + add);
}

inline bool
operator<(Time lhs, Time rhs)
{
        return lhs.microSeconds() < rhs.microSeconds();
}

} // namespace Time

#endif /*_EXPLORER_TIME_H_*/
