#include <sys/timerfd.h>
#include <string.h>
#include <cassert>
#include <unistd.h>

#include "timerQueue.h"
#include "timer.h"

namespace Explorer {

TimerQueue::TimerQueue(EventLoop* loop)
        : loop_(loop),
          timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
          timerfdChannel_(loop_, timerfd_),
          timersList_()
{
        timerfdChannel_.setReadCallBack([this] { this->handleRead();}); //一个Channel负责所有定时器
        timerfdChannel_.enableRead(); // 向epoll注册定时器事件
}

TimerQueue::~TimerQueue()
{
        for (TimerList::iterator it = timersList_.begin();
             it != timersList_.end(); ++it)
                delete it->second;
}

void
TimerQueue::addTimer(const TimerCallBack& cb, Time when, double interval)
{
        bool status = false;
        Timer *timer = new Timer(cb, when, interval); // 新的定时器
        Time tv = timer->expiration();

        // 若没有这步，则timerfd_settime会设置最大的为超时时间
        TimerList::iterator it = timersList_.begin();
        if (it == timersList_.end() || tv < it->first)
                status = true;

        timersList_.insert(std::make_pair(tv, timer)); // 将定时器插入定时器集合

        // 新添加的定时器超时时间小于目前最小的超时时间才会设置超时
        if (status)
        {

                // 微秒级别的定时器
                int64_t difference = timer->expiration().microSeconds()
                                     - Time::now().microSeconds();

                struct itimerspec value;
                bzero(&value, sizeof(value));

                value.it_value.tv_sec = static_cast<time_t>(difference
                                        / Time::MicroSecondsPerSecond);
                value.it_value.tv_nsec = static_cast<long>((difference
                                         % Time::MicroSecondsPerSecond) * 1000);

                ::timerfd_settime(timerfd_, 0, &value, 0); // 一个定时器描述符管理所有定时器
        }

}

// 若定时器从epoll返回，应读取描述符上的内容，否则epoll以后会持续返回该定时器
// 表现出来的就是会一直循环调用handleRead()。
// 解决方法就是将epoll设为ET模式，或者读取定时器描述符上的事件。
void
TimerQueue::handleRead()
{
        uint64_t buf;
        read(timerfd_, &buf, sizeof(buf));
        Time now(Time::now());
        std::vector<Entry> ret = getExpired(now); // 返回超时的定时器
        for (std::vector<Entry>::iterator it = ret.begin();
             it != ret.end(); ++it)
                (*it).second->run(); // 调用定时器的回调函数
        reset(ret, now);
}

std::vector<typename TimerQueue::Entry>
TimerQueue::getExpired(Time now)
{
        std::vector<Entry> ret;
        Entry cmp = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
        TimerList::iterator it = timersList_.lower_bound(cmp);
        assert(it == timersList_.end() || now < it->first);
        std::copy(timersList_.begin(), it, back_inserter(ret));
        timersList_.erase(timersList_.begin(), it);
        return ret;
}

void
TimerQueue::reset(std::vector<Entry>& expired, Time now)
{
        for (std::vector<Entry>::iterator it = expired.begin();
             it != expired.end(); ++it)
        {
                if (it->second->repeat())
                {
                        it->second->restart(now);
                        timersList_.insert(std::make_pair(it->second->expiration(), it->second));
                }
                else
                        delete it->second;
                if (!timersList_.empty())
                {
                        Time next = timersList_.begin()->second->expiration();
                        int64_t difference = next.microSeconds()
                                     - Time::now().microSeconds();

                        struct itimerspec value;
                        bzero(&value, sizeof(value));

                        value.it_value.tv_sec = static_cast<time_t>(difference
                                                / Time::MicroSecondsPerSecond);
                        value.it_value.tv_nsec = static_cast<long>(difference
                                                 % Time::MicroSecondsPerSecond) * 1000;

                        ::timerfd_settime(timerfd_, 0, &value, 0);
                }
        }
}

} // namespace Explorer
