#include "timer.h"

namespace Explorer {

Timer::Timer(const TimerCallBack& cb, Time when, double interval)
        : callback_(cb),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.00)
{ }

void
Timer::run() const
{
        callback_();
}

bool
Timer::repeat() const
{
        return repeat_;
}

Time
Timer::expiration() const
{
        return expiration_;
}

void
Timer::restart(Time now)
{
        if (repeat_)
        {
                expiration_ = addTime(now, interval_);
        }
        else
                expiration_ = Time();
}

} // namespace Explorer
