#include <unistd.h>

#include "../../net/eventLoop.h"

int flag = 0;

void assign()
{
        printf("thread %d\n", flag++);
}

int main()
{
        using namespace Explorer;

        // 创建工作队列-->启动线程池-->线程阻塞在工作队列上-->创建epoller、channel
        EventLoop loop(5, 9);

        Task func = assign;
        loop.RunAfter(1000, func); // 添加定时器、更新超时时间
        loop.RunAfter(2000, func); // 添加定时器、更新超时时间
        loop.RunEvery(1000, func); // 添加间隔定时器

        // 根据最小超时时间调用epoll_wait-->处理超时-->找出超时的定时器-->添加任务到工作队列
        // 阻塞的线程开始工作-->队列空、线程继续阻塞
        loop.Loop();
}
