#include <memory>

#include "../../../net/eventLoop.h"
#include "../../../net/sig.h"
#include "../../../net/connection.h"
#include "../../../include/log.h"

using namespace Explorer;

int main()
{
        // 每个Channel管理一个描述符
        // 创建工作队列、线程池、线程池阻塞、创建loop的Channel、公共的Epoller、向Epoller注册Channel
        std::shared_ptr<EventLoop> loop = std::make_shared<EventLoop>(1, 8);
        // 创建连接套接字、创建连接的Channel、向Channel注册连接套接字、向公共的Epoller注册Channel
        Connection con(Address(8000, "127.0.0.1"), loop->GetEpoller());
        Signal::Register(SIGINT, [&]{loop->Exit();});

        ExitIf(!con.Success(), "");
        // 设置客户端套接字可读的回调函数
        con.SetReadCallback([&con]() {
                //printf("read %u : %s\n",
                //      con.GetInput().ReadableBytes(),
                //      con.GetInput().BeginRead());
                con.GetInput().Clear();
        });

        // 客户端运行5秒后终止
        loop->RunAfter(20000, [&loop](){loop->Exit();});

        // 每0.5秒发送一次
        TimerId id1 = loop->RunEvery(500, [&con]() {
                con.Send("hello world! ");
        });
        // 每0.7秒发送一次
        TimerId id2 = loop->RunEvery(700, [&con]() {
                con.Send("hello uestc!  ");
        });
        TimerId id3 = loop->RunAfter(9000, [&con]() {
                con.Send("hello Explorer!  ");
        });

        // 1.5秒后取消定时器,发送3次id1
        loop->RunAfter(1500, [&loop, id1]() {
                loop->Cancel(id1);
        });
        // 3.5秒后取消定时器，发送5次id2
        loop->RunAfter(3500, [&loop, id2]() {
                loop->Cancel(id2);
        });

        // 检测套接字上的可读事件
        loop->Loop();

        return 0;
}
