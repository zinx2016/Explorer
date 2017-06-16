#ifndef _EXPLORER_WORK_QUEUE_H_
#define _EXPLORER_WORK_QUEUE_H_

#include "condition.h"
#include <queue>

namespace Explorer {

template <typename T>
class WorkQueue : private NoCopy
{
public:
        explicit WorkQueue(int maxSize)
                : mutex_(),
                  notEmpty_(mutex_),
                  notFull_(mutex_),
                  queue_(),
                  maxSize_(maxSize)
        { }

        void push(const T& x)
        {
                MutexLockGuard mtx(mutex_);
                while (queue_.size() == maxSize_)
                {
                        notFull_.wait();
                }
                assert(!(queue_.size() == maxSize_));
                queue_.push(x);
                notEmpty_.notify();
        }

        T pop()
        {
                MutexLockGuard mtx(mutex_);
                while (queue_.empty())
                {
                        notEmpty_.wait();
                }
                assert(!queue_.empty());
                T ret(queue_.front());
                queue_.pop();
                notFull_.notify();
                return ret;
        }

        bool empty()
        {
                MutexLockGuard mtx(mutex_);
                return queue_.empty();
        }

        bool full()
        {
                MutexLockGuard mtx(mutex_);
                return queue_.size() == maxSize_;
        }

        size_t size()
        {
                MutexLockGuard mtx(mutex_);
                return queue_.size();
        }

        size_t capacity()
        {
                MutexLockGuard mtx(mutex_);
                return maxSize_;
        }

private:
        mutable Mutex           mutex_;
        Condition               notEmpty_;
        Condition               notFull_;
        std::queue<T>           queue_;
        int                     maxSize_;
};

} // namespace Explorer

#endif /*_EXPLORER_WORK_QUEUE_H_*/
