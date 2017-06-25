#ifndef _EXPLORER_SOCKET_H_
#define _EXPLORER_SOCKET_H_

#include "../base/base.h"
#include "netAddress.h"
#include <sys/socket.h>

namespace Explorer {

#define BACKLOG 4096

class Socket : private NoCopy
{
public:
        Socket();
        void connect(int sockfd, NetAddress& address);
        void bind(int sockfd, NetAddress& address);
        void listen(int sockfd);
        int accept(int sockfd, struct sockaddr* address);
        void close(int sockfd);
        int fd();

        // 半关闭
        void shutdownWrite(int fd);
        // 设置套接字非阻塞
        void setNonBlock(int fd);
        // 启用/禁止 Nagle算法 TCP_NODELAY
        void setTcpNoDelay(bool on);
        // SO_REUSEADDR
        void setReuseAddr(bool on);
        // SO_REUSEPORT
        void setReusePort(bool on);
        // SO_KEEPALIVE
        void setKeepAlive(bool on);

private:
        int sockfd_; // 监听套接字
};

} // namespace Explorer

#endif /*_EXPLORER_SOCKET_H_*/
