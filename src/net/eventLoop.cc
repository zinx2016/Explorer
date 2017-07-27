#include <unistd.h>

#include "eventLoop.h"
#include "epoller.h"
#include "channel.h"
#include "../include/threadPool.h"
#include "../include/boundedQueue.h"
#include "socket.h"
#include "time.h"

namespace Explorer {

EventLoop::EventLoop(int threadNums, int queueMaxSize)
        : epoller_(std::make_shared<Epoller>()),
          queue_(std::make_shared<BoundedQueue<Task>>(queueMaxSize)),
          pool_(std::make_shared<ThreadPool<Task>>(queue_, threadNums)),
          running_(true),
          nextTimeout_(MaxTimeout),
          seq_(0)
{
        assert(!pipe(wakeUp_));
        assert(Socket(wakeUp_[0]).SetNonBlock());
        channel_ = std::make_shared<Channel>(wakeUp_[0], epoller_, nullptr, nullptr);
        channel_->SetReadCallback([this]() {
                        char buf;
                        ssize_t r = read(channel_->Fd(), &buf, sizeof(buf));
                        assert(r == sizeof(buf));
                  });
        pid_ = pthread_self();
}

EventLoop::~EventLoop()
{
        close(wakeUp_[1]);
}

void EventLoop::Loop()
{
        while (running_)
        {
                epoller_->LoopOnce(std::min(5000, nextTimeout_));
                HandleTimeout();
        }
        if (nextTimeout_ != MaxTimeout)
        {
                epoller_->LoopOnce(nextTimeout_);
                HandleTimeout();
        }
        else
                epoller_->LoopOnce(0);
        repeat_.clear();
        pending_.clear();
        pool_->Clear();
}

void EventLoop::Exit()
{
        MutexGuard mtx(mutex_);
        if (running_)
        {
                running_ = false;
                WakeUp(); // 使管道可读，检测running_，退出Loop
        }
}

void EventLoop::WakeUp()
{
        char buf;
        ssize_t r = write(wakeUp_[1], &buf, sizeof(buf));
        assert(r > 0);
}


std::shared_ptr<Epoller> EventLoop::GetEpoller()
{
        return epoller_;
}

void EventLoop::RunNow(Task& task)
{
        RunAfter(0, task);
}

//TimerId EventLoop::RunAfter(int64_t milliSec, Task& task)
TimerId EventLoop::RunAfter(int64_t milliSec, Task task)
{
        MutexGuard mtx(mutex_);
        if (!running_)
                return TimerId();
        TimerId id {Time::Now() + milliSec, seq_++}; // 构造定时器
        pending_.insert({id, task}); // 添加到定时器集合
        if (pthread_self() != pid_)
                WakeUp();
        else
                Refresh(); // 更新最近超时时间
        return id;
}

//TimerId EventLoop::RunEvery(int64_t milliSec, Task& task)
TimerId EventLoop::RunEvery(int64_t milliSec, Task task)
{
        MutexGuard mtx(mutex_);
        if (!running_)
                return TimerId();
        uint32_t seq = seq_++;
        TimeRep rep {milliSec, Time::Now() + milliSec}; // 构造间隔定时器
        TimerId id {rep.second, seq};
        repeat_.insert({seq, rep}); // 添加到间隔定时器集合
        pending_.insert({id, task}); // 添加到定时器集合
        if (pthread_self() != pid_)
                WakeUp();
        else
                Refresh(); // 更新最近超时时间
        return {milliSec, seq};
}

void EventLoop::RescheduleAfter(TimerId* id, int64_t milliSec, Task& task)
{
        MutexGuard mtx(mutex_);
        if (!running_)
                return ;
        TimerId nid {Time::Now() + milliSec, seq_++};
        auto item = pending_.find(*id);
        if (item != pending_.end())
                pending_.erase(*id);
        pending_.insert({nid, task});
        *id = nid;
        if (pthread_self() != pid_)
                WakeUp();
        else
                Refresh();
}

void EventLoop::RescheduleAfter(const TimerId& timerId, int64_t milliSec)
{
        MutexGuard mtx(mutex_);
        if (!running_)
                return ;
        TimerId id {Time::Now() + milliSec, seq_++};
        auto item = pending_.find(timerId);
        if (item == pending_.end())
                return ;
        Task task = item->second;
        pending_.erase(item);
        pending_.insert({id, task});
        if (pthread_self() != pid_)
                WakeUp();
        else
                Refresh();
}

// 从定时器集合中去除定时器
void EventLoop::Cancel(const TimerId& Timer)
{
        MutexGuard mtx(mutex_);

        auto item = repeat_.find(Timer.second);
        if (item != repeat_.end())
                repeat_.erase(item);
        auto it = pending_.find(Timer);
        if (it != pending_.end())
                pending_.erase(it);
}

// epoll_wait超时，处理定时器集合中定时器
void EventLoop::HandleTimeout()
{
        TimerId now {Time::Now(), 0xffffffff};
        std::vector<Task> expired;
        // 从定时器集合中找出超时定时器，将其任务添加到超时队列中
        while (running_ &&
               !pending_.empty() &&
               pending_.begin()->first <= now)
        {
                expired.push_back(pending_.begin()->second);
                const TimerId& id = pending_.begin()->first;
                auto item = repeat_.find(id.second); // 查看该定时器是否是间隔定时器
                if (item != repeat_.end()) // 重新添加到定时器集合中
                {
                        TimerId nid {now.first + item->second.first, id.second};
                        item->second.second = nid.first;
                        pending_.insert({nid, pending_.begin()->second});
                }
                pending_.erase(pending_.begin());
        }
        Refresh();
        for (int i = 0; i < expired.size(); ++i)
                queue_->Push(expired[i]); // 将超时任务添加到工作队列中
}

// 更新下次最近超时时间
void EventLoop::Refresh()
{
        if (pending_.empty())
                nextTimeout_ = MaxTimeout;
        else
        {
                auto it = pending_.begin()->first;
                int64_t tmp = it.first - Time::Now();
                nextTimeout_ = tmp < 0 ? 0 : int(tmp);
        }
}

std::shared_ptr<BoundedQueue<Task>>& EventLoop::GetQueue()
{
        return queue_;
}

} // namespace Explorer
