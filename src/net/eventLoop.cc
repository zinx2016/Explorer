#include "eventLoop.h"
#include "channel.h"
#include "epoller.h"
#include <cassert>
#include "../base/currentThread.h"

namespace Explorer {

// 指向每个线程内唯一的EventLoop
__thread EventLoop* loopInThisThread = 0;

const int epollTimeOut = 10000; // 10 seconds

EventLoop::EventLoop()
        : threadId_(CurrentThread::tid()),
          looping_(false),
          quit_(false),
          epoller_(new Epoller(this)),  // 创建EventLoop的Epoller
          timerQueue_(new TimerQueue(this))
{
        // 1个线程只能创建1个EventLoop
        assert(loopInThisThread == 0);
        loopInThisThread = this;
}

EventLoop::~EventLoop()
{
        assert(!looping_);
        loopInThisThread = 0;
}

void
EventLoop::loop()
{
        assert(!looping_);
        // 只能在所属线程内调用
        assert(threadId_ == CurrentThread::tid());

        looping_ = true;
        quit_ = false;

        while (!quit_)
        {
                activeChannels_.clear(); // 清除事件
                epoller_->epoll(epollTimeOut, activeChannels_);
                for (ChannelList::iterator iter = activeChannels_.begin();
                     iter != activeChannels_.end(); ++iter)
                {
                        (*iter)->handleEvent();  // 调用每个Channel的事件处理机制
                }
        }
        looping_ = false;
}

void
EventLoop::updateChannel(Channel* channel)
{
        assert(epoller_->ownerLoop() == this);
        // 只能在所属线程内调用
        assert(threadId_ == CurrentThread::tid());

        epoller_->updateChannel(channel);
}

void
EventLoop::quit()
{
        quit_ = true;
}

void
EventLoop::runAfter(double seconds, const TimerCallBack& cb)
{
        Time time(addTime(Time::now(), seconds));
        timerQueue_->addTimer(cb, time, 0.00); // 回调函数，超时时间戳，间隔
}

void
EventLoop::runEvery(double seconds, const TimerCallBack& cb)
{
        Time time(addTime(Time::now(), seconds));
        timerQueue_->addTimer(cb, time, seconds);
}

} // namespace Explorer
