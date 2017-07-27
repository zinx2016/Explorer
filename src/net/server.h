#ifndef _EXPLORER_SERVER_H_
#define _EXPLORER_SERVER_H_

#include <memory>

#include "../include/base.h"
#include "callback.h"
#include "socket.h"
#include "connection.h"

namespace Explorer {

class Channel;
class EventLoop;

class Server : private NoCopy
{
public:
        Server(std::shared_ptr<EventLoop> eventLoop, uint16_t port);
        ~Server();

        void Start();
        void Close();
        void HandleAccept(); // 客户端新连接到来时的服务器回调函数
        void SetConnectCallback(const ConnectCallback& cb);

        uint16_t Port() const;
        std::vector<std::shared_ptr<Connection>>& Connections();

protected:
        uint16_t port_;
        Socket socket_;
        std::shared_ptr<EventLoop> eventLoop_;
        std::shared_ptr<Channel> listen_; // 用于向Epoller注册监听套接字，等待连接到来
        std::vector<std::shared_ptr<Connection>> connections_; // 目前的已连接队列
        ConnectCallback connectCb_; // 可由用户设置的新连接到来时的回调函数
};

} // namespace Explorer

#endif // _EXPLORER_SERVER_H_
