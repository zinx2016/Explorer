#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h> // for TCP_NODELAY

#include "socket.h"

namespace Explorer {

Socket::Socket()
{
        assert( -1 != (sockfd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP)) );
        setNonBlock(sockfd_);
}

void
Socket::connect(int sockfd, NetAddress& address)
{
        assert( 0 == ::connect(sockfd, (struct sockaddr* )&address.getAddr(), address.len()) );
}

void
Socket::bind(int sockfd, NetAddress& address)
{
        assert( 0 == ::bind(sockfd, (struct sockaddr* )&address.getAddr(), address.len()) );
}

void
Socket::listen(int sockfd)
{
        assert(0 == ::listen(sockfd, BACKLOG));
}

int
Socket::accept(int sockfd, struct sockaddr* address)
{
        int fd;
        socklen_t len = sizeof(struct sockaddr_in);
        assert( 0 < (fd = ::accept(sockfd, address, &len)) );
        setNonBlock(fd);
        return fd;
}

int
Socket::fd()
{
        return sockfd_;
}

void
Socket::close(int sockfd)
{
        assert(0 == ::close(sockfd));
}

void
Socket::shutdownWrite(int sockfd)
{
        assert(-1 != shutdown(sockfd, SHUT_WR));
}

void
Socket::setNonBlock(int sockfd)
{
        int flag = fcntl(sockfd, F_GETFL, 0);
        assert(flag != -1);
        flag |= O_NONBLOCK;
        assert( -1 != fcntl(sockfd, F_SETFL, flag));
}

void
Socket::setTcpNoDelay(bool on)
{
        int optval = on ? 1 : 0;
        setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval,
                   static_cast<socklen_t>(sizeof(optval)));
}

void
Socket::setReuseAddr(bool on)
{
        int optval = on ? 1 : 0;
        setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                   static_cast<socklen_t>(sizeof(optval)));
}

void
Socket::setReusePort(bool on)
{
        int optval = on ? 1 : 0;
        setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                   static_cast<socklen_t>(sizeof(optval)));
}

void
Socket::setKeepAlive(bool on)
{
        int optval = on ? 1 : 0;
        setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                   static_cast<socklen_t>(sizeof(optval)));
}

} // namespace Explorer
