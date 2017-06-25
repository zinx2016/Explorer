#include <stdio.h>
#include <unistd.h>

#include "../../net/timerQueue.h"
#include "../../base/currentThread.h"
#include "../../net/eventLoop.h"

using Explorer::EventLoop;

int cnt = 0;
EventLoop* pl;

void timeout()
{
        ++cnt;
        printf("time out , cnt : %d\n", cnt);
        if (cnt == 20)
                pl->quit();
}

int main()
{
        printf("main: pid = %d , tid = %d\n", getpid(), Explorer::CurrentThread::tid());

        EventLoop loop;
        pl = &loop;

        for(int i = 1; i < 10; i++)
                loop.runAfter(i, timeout);

        loop.runEvery(2.5, timeout);

        loop.loop();
}
