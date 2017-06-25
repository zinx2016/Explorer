#include <stdio.h>

#include "../../net/acceptor.h"

void callback(int fd, const Explorer::NetAddress& cb)
{
        printf("callback is called\n");
}

int main()
{
        Explorer::NetAddress srvaddr(1234);
        Explorer::EventLoop loop;
        Explorer::Acceptor accept(&loop, srvaddr);

        accept.setConnectionCallBack(callback);
        accept.listen();

        loop.loop();
}
