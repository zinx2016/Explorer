#ifndef _EXPLORER_WORK_QUEUE_H_
#define _EXPLORER_WORK_QUEUE_H_

#include "condition.h"
#include <queue>

namespace Explorer {

template <typename T>
class BoundedQueue : private NoCopy
{
public:
        explicit BoundedQueue(int maxSize)
                : mutex_(),
                  notEmpty_(),
                  notFull_(),
                  queue_(),
                  maxSize_(maxSize),
                  clear_(false)
        { }

        void Push(const T& x)
        {
                MutexGuard mtx(mutex_);
                while (queue_.size() == maxSize_)
                {
                        notFull_.Wait(mutex_);
                }
                assert(!(queue_.size() == maxSize_));
                queue_.push(x);
                notEmpty_.Signal();
        }

        T Pop()
        {
                MutexGuard mtx(mutex_);
                while (queue_.empty() && !clear_)
                {
                        notEmpty_.Wait(mutex_);
                }
                if (clear_)
                        return T();
                assert(!queue_.empty());
                T ret(queue_.front());
                queue_.pop();
                notFull_.Signal();
                return ret;
        }

        void Clear()
        {
                MutexGuard mtx(mutex_);
                clear_ = true;
                notEmpty_.BroadCast();
        }

        int Size()
        {
                MutexGuard mtx(mutex_);
                return queue_.size();
        }

private:
        mutable Mutex           mutex_;
        Condition               notEmpty_;
        Condition               notFull_;
        std::queue<T>           queue_;
        int                     maxSize_;
        bool                    clear_;
};

} // namespace Explorer

#endif /*_EXPLORER_BOUNDED_QUEUE_H_*/
