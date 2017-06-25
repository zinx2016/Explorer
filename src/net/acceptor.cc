#include "acceptor.h"

namespace Explorer {

Acceptor::Acceptor(EventLoop* loop, NetAddress& address)
        : loop_(loop),
          socket_(),
          acceptChannel_(loop, socket_.fd())
{
        socket_.setReuseAddr(true);
        socket_.bind(socket_.fd(), address);
        acceptChannel_.setReadCallBack([this] {this->handleRead();});
}

void
Acceptor::setConnectionCallBack(const ConnectionCallBack& cb)
{
        connectionCb_ = cb;
}

void
Acceptor::listen()
{
        socket_.listen(socket_.fd());
        acceptChannel_.enableRead();
}

void
Acceptor::handleRead()
{
        NetAddress cliAddress;
        int confd = socket_.accept(socket_.fd(), (struct sockaddr*)&cliAddress.getAddr());
        connectionCb_(confd, cliAddress);
}


} // namespace Explorer
