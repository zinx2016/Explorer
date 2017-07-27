#include "../../../kvstore/kvClient.h"

using namespace Explorer;

// 3种线程
int main()
{
        const char* ip = "127.0.0.1";
        KvClient client(5, 8, 8000, ip);
        std::shared_ptr<EventLoop> loop = client.GetLoop();
        Connection& con = client.GetConnection();

        Signal::Register(SIGINT, [&]{loop->Exit();});
        ExitIf(!con.Success(), "");

        // 设置客户端套接字可读的回调函数
        con.SetReadCallback([&con, &client]() {
                printf("%s",
                        con.GetInput().BeginRead());
                con.GetInput().Clear();
                client.IncCount();
        });

        // 创建线程处理客户端命令
        Thread getInputThread([&client]() {
                client.RunDb();
        });

        getInputThread.Start();

        // 检测套接字上的可读事件
        loop->Loop();

        getInputThread.Stop();

        return 0;
}
