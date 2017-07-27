#ifndef _EXPLORER_KV_CLIENT_H_
#define _EXPLORER_KV_CLIENT_H_

#include <string.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "../tree/bPlusTree.h"
#include "../net/eventLoop.h"
#include "../net/sig.h"
#include "../net/connection.h"
#include "../include/log.h"
#include "../include/thread.h"
#include "../include/boundedQueue.h"
#include "../include/atomic.h"

namespace Explorer {

class KvClient
{
public:
        KvClient(int threadNums, int queueMaxSize,
                 uint16_t port = 8000, const char* ip = "127.0.0.1");

        void RunDb();

        Connection& GetConnection();

        std::shared_ptr<EventLoop> GetLoop();

        void IncCount();

        uint16_t GetCount();

private:
        void Work();

        void Command();

        void GetInput();

        bool CheckCommand();

        void Help();

        int Helper();


private:
        std::shared_ptr<EventLoop> loop_;
        std::shared_ptr<BoundedQueue<Task>> queue_;
        Connection con_;

        std::vector<std::string> input_;
        Buffer buf_;
        AtomicInt16 count_;
};

} // namespace Explorer

#endif // _EXPLORER_KV_CLIENT_H_
