#ifndef _EXPLORER_KV_SERVER_H_
#define _EXPLORER_KV_SERVER_H_

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <fcntl.h>
#include <sys/stat.h>

#include "../net/eventLoop.h"
#include "../net/buffer.h"
#include "../net/connection.h"
#include "../include/boundedQueue.h"
#include "../tree/bPlusTree.h"

namespace Explorer {

class KvServer
{
public:
        KvServer(int threadNums, int queueMaxSize, const char* filePath);

        void GetArg(std::shared_ptr<Connection> con, Buffer& input);

        std::shared_ptr<EventLoop> GetLoop();

        void RunDb();

private:
        void Insert();
        void Update();
        void Del();
        void Search();
        void Stat();

private:
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<Connection> con_;
        std::shared_ptr<BoundedQueue<Task>> queue_;

        BPlusTree db_;
        Buffer buf_;
        std::vector<std::string> arg_;
};

} // namespace Explorer

#endif // _EXPLORER_KV_SERVER_H_
