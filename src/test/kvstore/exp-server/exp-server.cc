#include "../../../net/sig.h"
#include "../../../net/server.h"
#include "../../../include/thread.h"
#include "../../../kvstore/kvServer.h"

using namespace Explorer;

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

int main()
{
        const char* filePath = "test.db";
        open(filePath, O_CREAT | O_EXCL, FILE_MODE);

        KvServer kvsrv(2, 8, "test.db");
        std::shared_ptr<EventLoop> loop = kvsrv.GetLoop();

        Signal::Register(SIGINT, [&]{loop->Exit();});
        Server server(loop, 8000);
        server.Start();

        server.SetConnectCallback([&kvsrv](std::shared_ptr<Connection> con) {
                        con->SetReadCallback([con, &kvsrv]() {
                                kvsrv.GetArg(con, con->GetInput());
                        });
                });

        Thread sendReplyThread([&kvsrv]() {
                kvsrv.RunDb();
        });
        sendReplyThread.Start();

        loop->Loop();

        sendReplyThread.Stop();

        return 0;
}
