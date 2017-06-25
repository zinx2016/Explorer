#include "../../net/eventLoop.h"
#include "../../net/epoller.h"
#include "../../net/channel.h"
#include <unistd.h>

using Explorer::EventLoop;
using Explorer::Thread;

EventLoop *lp;

void func()
{
        //lp->loop(); // loop对象被非创建它的线程调用，则出错
        EventLoop loop;
        loop.loop();
}

int main()
{
        EventLoop loop;
        lp = &loop;
        Thread thread(func);
        thread.start();
        sleep(3);
}
