#ifndef _EXPLORER_THREAD_POOL_H_
#define _EXPLORER_THREAD_POOL_H_

#include <memory>
#include <vector>

#include "base.h"
#include "atomic.h"
#include "boundedQueue.h"
#include "thread.h"

/*
 * 设计思想
 * 让线程从阻塞队列中取出任务
 * 当队列为空时，线程阻塞，直到往队列中添加了新任务
 * 当队列为满时，线程阻塞，直到从队列中取走了任务
*/

namespace Explorer {

template <typename T>
class ThreadPool : private NoCopyTemplate<T>
{
public:
        ThreadPool(std::shared_ptr<BoundedQueue<T>> queue, int threadNum);

        void Clear(); // 销毁阻塞在空队列上的线程

private:
        void Run(); // 线程池中每个线程的执行函数

private:
        AtomicInt8 working_;
        int threadNum_;
        std::shared_ptr<BoundedQueue<T>> queue_; // 线程池取得工作任务的队列
        std::vector<std::shared_ptr<Thread>> threads_;
};

template <typename T>
ThreadPool<T>::ThreadPool(std::shared_ptr<BoundedQueue<T>> queue,
                          int threadNum)
        : working_(0),
          threadNum_(threadNum),
          queue_(queue)
{
        // 线程数最大设为8
        if (threadNum_ <= 0)
                threadNum_ = 1;
        if (threadNum_ > 8)
                threadNum_ = 8;
        threads_.resize(threadNum_);
        for (int i = 0; i < threads_.size(); ++i)
        {
                // 创建线程
                threads_[i] = std::make_shared<Thread>([this] {this->Run();});
                // 启动线程
                threads_[i]->Start();
        }
        working_ = 1;
}

template <typename T>
void ThreadPool<T>::Run()
{
        while (1)
        {
                // 从工作队列取出任务
                T task = queue_->Pop(); // 队列为空，则线程阻塞
                if (0 == working_.Get()) // 外部条件控制线程终止
                        break;
                task(); // 执行任务
        }
}

template <typename T>
void ThreadPool<T>::Clear()
{
        if (!working_.Get())
                return ;
        working_ = 0;
        queue_->Clear(); // 唤醒阻塞线程，并且使线程终止，销毁线程池
        for (int i = 0; i < threads_.size(); ++i)
                threads_[i]->Stop(); // 回收终止线程资源
}

} // namespace Explorer

#endif // _EXPLORER_THREAD_POOL_H_
