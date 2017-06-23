#ifndef _EXPLORER_TIMER_H_
#define _EXPLORER_TIMER_H_

#include <stdint.h>

#include "callback.h"
#include "time.h"
#include "../base/base.h"

namespace Explorer {

class Timer : private NoCopy
{
public:
        Timer(const TimerCallBack& cb, Time when, double interval);

        void run() const;

        bool repeat() const;

        Time expiration() const;

        void restart(Time now);

private:
        const TimerCallBack callback_; // 该定时器的回调函数
        const double interval_; // 定时器间隔
        const bool repeat_; // 超时后是否重设
        Time expiration_; // 多少微秒后超时
};

} // namespace Explorer

#endif /*_EXPLORER_TIMER_H_*/
