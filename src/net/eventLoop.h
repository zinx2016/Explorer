#ifndef _EXPLORER_EVENT_LOOP_H_
#define _EXPLORER_EVENT_LOOP_H_

#include <map>
#include <memory>

#include "../include/base.h"
#include "../include/mutex.h"

namespace Explorer {

class Epoller;
class Channel;

template <typename T>
class ThreadPool;

template <typename T>
class BoundedQueue;

// 定时器
typedef std::pair<int64_t, uint32_t> TimerId; // <超时的时间戳，定时器标识>

typedef std::pair<int64_t, int64_t> TimeRep; // <间隔时间，超时时间戳>

class EventLoop : private NoCopy
{
public:
        EventLoop(int threadNums, int queueMaxSize);
        ~EventLoop();

        void Loop();
        void Exit();
        std::shared_ptr<Epoller> GetEpoller();

        void RunNow(Task& task);

        TimerId RunAfter(int64_t milliSec, Task task);
        TimerId RunEvery(int64_t milliSec, Task task);

        void RescheduleAfter(TimerId* id, int64_t milliSec, Task& task);
        void RescheduleAfter(const TimerId& timerId, int64_t milliSec);

        void Cancel(const TimerId& timerId);

        std::shared_ptr<BoundedQueue<Task>>& GetQueue();

private:
        void HandleTimeout();
        void WakeUp();
        void Refresh();

private:
        static const int MaxTimeout = 0x7fffffff;

        std::shared_ptr<BoundedQueue<Task>> queue_; // 工作队列
        std::shared_ptr<ThreadPool<Task>> pool_; // 线程池
        std::shared_ptr<Channel> channel_; // 管理每个loop的描述符
        std::shared_ptr<Epoller> epoller_;

        Mutex mutex_;
        bool running_; // 用于控制loop的退出
        int wakeUp_[2]; // 和WakeUp()配合，使loop立即检测running_状态标识
        int nextTimeout_;
        uint64_t pid_;
        uint32_t seq_; // 每个定时器的身份标志

        std::map<uint32_t, TimeRep> repeat_; // 间隔定时器集合 <定时器标识，间隔定时器>
        std::map<TimerId, Task> pending_; // 定时器集合 <定时器，定时器任务>
};

} // namespace Explorer

#endif // _EXPLORER_EVENT_LOOP_H_
