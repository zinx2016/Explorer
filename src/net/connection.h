#ifndef _EXPLORER_CONNECTION_H_
#define _EXPLORER_CONNECTION_H_

#include <memory>

#include "../include/base.h"
#include "callback.h"
#include "address.h"
#include "socket.h"
#include "buffer.h"

namespace Explorer {

class Channel;
class Epoller;

class Connection : private NoCopy
{
public:
        Connection(const Socket& socket, std::shared_ptr<Epoller> epoller);
        Connection(const Address& server, std::shared_ptr<Epoller> epoller);
        ~Connection();

        Buffer& GetInput();
        Buffer& GetOutput();

        void HandleRead();
        void HandleWrite();
        void SetReadCallback(const ReadCallback& cb);
        void SetWriteCallback(const ReadCallback& cb);

        void Send(Buffer& buffer);
        void Send(const char* buf);
        void Send(const char* buf, uint32_t len);
        void SendOutput();

        bool Success() const;
        bool Close();


protected:
        bool connected_;

        Buffer input_;
        Buffer output_;

        Socket socket_; // 已连接套接字
        std::shared_ptr<Channel> channel_;

        ReadCallback readCb_;
        WriteCallback writeCb_;
};

} // namespace Explorer

#endif // _EXPLORER_CONNECTION_H_
