#include <cassert>

#include "server.h"
#include "eventLoop.h"
#include "channel.h"
#include "epoller.h"

namespace Explorer {

Server::Server(std::shared_ptr<EventLoop> eventLoop, uint16_t port)
        : port_(port),
          eventLoop_(eventLoop),
          listen_(nullptr),
          connectCb_(nullptr)
{ }

Server::~Server()
{
        if (listen_)
                Close();
}

void Server::Start()
{
        // 设置服务器监听套接字
        assert(socket_.Create() && socket_.SetReuseAddress() &&
               socket_.Bind(port_) && socket_.Listen());

        // 向epoll注册监听套接字
        listen_ = std::make_shared<Channel>(socket_.Fd(), eventLoop_->GetEpoller(),
                                            [this]() {HandleAccept();}, nullptr);
}

void Server::Close()
{
        socket_.Close(); // 关闭监听套接字
        for (auto con : connections_)
                con->Close(); // 关闭已连接套接字
}

void Server::HandleAccept()
{
        int fd; // 已连接套接字

        fd = socket_.Accept(); // 连接到来，创建已连接套接字
        assert(fd > 0);
        // 注册已连接套接字，及其可读、可写的回调函数
        std::shared_ptr<Connection> con =
        std::make_shared<Connection>(Socket(fd), eventLoop_->GetEpoller());
        if (connectCb_)
                connectCb_(con);
        connections_.push_back(con); // 新连接加入已连接队列
}

void Server::SetConnectCallback(const ConnectCallback& cb)
{
        connectCb_ = cb;
}

uint16_t Server::Port() const
{
        return port_;
}

std::vector<std::shared_ptr<Connection>>& Server::Connections()
{
        return connections_;
}

} // namespace Explorer
