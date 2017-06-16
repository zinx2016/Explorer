#ifndef _EXPLORER_THREAD_POOL_H_
#define _EXPLORER_THREAD_POOL_H_

#include "thread.h"
#include "workQueue.h"
#include <vector>
#include <memory>  // for shared_ptr

namespace Explorer {

template <typename T>
class ThreadPool : private NoCopy
{
public:
        ThreadPool(WorkQueue<T>& task, size_t maxThreadSize)
                : maxThreadSize_(maxThreadSize),
                  running_(false),
                  task_(task),
                  threads_()
        { }

        ~ThreadPool();

        void start();

        void stop();

private:
        void Run();

private:
        size_t          maxThreadSize_;
        bool            running_;
        WorkQueue<T>&   task_;
        std::vector<std::shared_ptr<Thread>> threads_;
};

template <typename T>
void
ThreadPool<T>::start()
{
        assert(threads_.empty());
        running_ = true;
        threads_.reserve(maxThreadSize_);
        for (int i = 0; i < maxThreadSize_; i++)
        {
                threads_.push_back( std::make_shared<Thread>([this] { this->Run();}) );
                threads_[i]->start();
        }
}

template <typename T>
void
ThreadPool<T>::Run()
{
        for (;;)
        {
                T task = task_.pop();
                task();
        }
}

template <typename T>
void
ThreadPool<T>::stop()
{
        running_ = 0;
        for (int i = 0; i < threads_.size(); i++)
        {
                threads_[i]->stop();
        }
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
        if (running_)
        {
                stop();
        }
}

}  // namespace Explorer

#endif /*_EXPLORER_THREAD_POOL_H_*/
