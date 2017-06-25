#include <stdio.h>
#include <sys/timerfd.h>
#include <string.h>
#include <unistd.h>

#include "../../net/eventLoop.h"
#include "../../net/channel.h"

using Explorer::EventLoop;
using Explorer::Channel;

EventLoop* ploop;

void timeout()
{
        printf("time out !\n");
        ploop->quit();
}

int main()
{
        EventLoop loop;
        ploop = &loop;

        int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        Channel channel(&loop, timerfd); // 向事件分发注册描述符
        channel.setReadCallBack(timeout); // 注册描述符的事件回调函数
        // Channel::enable*()-->Channel::update()-->EventLoop::updateChannel()
        // -->Epoller::updateChannel()-->epoll_ctl()
        channel.enableRead(); // 设置描述符上关心的事件，调用update()向epoll注册事件

        struct itimerspec tm;
        bzero(&tm, sizeof(tm));
        tm.it_value.tv_sec = 2;
        ::timerfd_settime(timerfd, 0, &tm, NULL);

        // loop()-->Epoller::epoll()-->epoll_wait()
        // -->fillActiveChannels()-->Channel::handleEvent()-->timeout()
        loop.loop(); // EventLoop开始循环，等待超时，则timerfd描述符上的事件变为可读
        ::close(timerfd);
}
