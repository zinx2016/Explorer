#include <unistd.h>

#include "../../include/threadPool.h"

int flag = 0;

void assign()
{
        printf("thread %d\n", flag++);
}

int main()
{
        using namespace Explorer;

        std::shared_ptr<BoundedQueue<Task>> pqueue = std::make_shared<BoundedQueue<Task>>(20);
        for (int i = 0; i < 15; i++)
                pqueue->Push(assign);

        ThreadPool<Task> pool(pqueue, 6); // 创建线程池，线程开始运行
        sleep(1); // 等待线程执行任务

        pool.Clear(); // 立即销毁线程池
        printf("end\n");
}
