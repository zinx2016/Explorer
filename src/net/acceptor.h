#ifndef _EXPLORER_ACCEPTOR_H_
#define _EXPLORER_ACCEPTOR_H_

#include "eventLoop.h"
#include "channel.h"
#include "socket.h"
#include "callback.h"
#include "netAddress.h"
#include "socket.h"

namespace Explorer {

class Acceptor
{
public:
        Acceptor(EventLoop* loop, NetAddress& address);

        void setConnectionCallBack(const ConnectionCallBack& cb);

        void handleRead();
        void listen();

private:
        EventLoop* loop_;
        Channel acceptChannel_;
        Socket socket_;
        ConnectionCallBack connectionCb_;
};

} // namespace Explorer

#endif /*_EXPLORER_ACCEPTOR_H_*/
