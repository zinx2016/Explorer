#include <memory>

#include "../../../net/server.h"
#include "../../../net/sig.h"
#include "../../../net/eventLoop.h"

using namespace Explorer;

int main()
{
        // 创建工作队列、线程池-->线程启动阻塞在空队列
        // -->创建Epoller、Channel-->向Epoller注册Channel
        std::shared_ptr<EventLoop> loop = std::make_shared<EventLoop>(2, 8);

        Signal::Register(SIGINT, [&]{loop->Exit();}); // 注册SIGINT信号，使loop退出Loop()循环
        Server server(loop, 8000); // 创建Server
        server.Start(); // 创建、绑定、监听套接字、向Epoller注册监听套接字
        // Server.HandleAccept()调用connectCb_()
        server.SetConnectCallback([](std::shared_ptr<Connection> con) {
        // Server的connectCb_()作用是注册connection的readCb_
                        con->SetReadCallback([con]() {
                                printf("read %u : %s\n",
                                       con->GetInput().ReadableBytes(),
                                       con->GetInput().BeginRead());
                                con->Send(con->GetInput());
                        });
                });
        // server.HandleAccept-->Connection()-->Connection.ReadCallback()
        loop->Loop(); // 循环调用Epoller->LoopOnce()等待注册的描述符可读
        return 0;
}
