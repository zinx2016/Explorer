#ifndef _EXPLORER_TIMER_QUEUE_H_
#define _EXPLORER_TIMER_QUEUE_H_

#include <set>
#include <vector>
#include <memory>

#include "channel.h"
#include "time.h"
#include "callback.h"
#include "../base/base.h"

namespace Explorer {

class EventLoop;
class Timer;

class TimerQueue : private NoCopy
{
public:
        TimerQueue(EventLoop* loop);
        ~TimerQueue();

        void addTimer(const TimerCallBack& cb, Time when, double interval);

private:
        typedef std::pair<Time, Timer*> Entry;
        typedef std::set<Entry> TimerList; // 定时器集合

        void handleRead(); //超时回调
        bool insert(Timer* timer);

        std::vector<Entry> getExpired(Time now); // 获取超时定时器
        void reset(std::vector<Entry>& expired, Time now);

private:
        EventLoop* loop_;
        const int timerfd_;
        Channel timerfdChannel_;
        TimerList timersList_;
};

} // namespace Explorer

#endif /*_EXPLORER_TIMER_QUEUE_H_*/
