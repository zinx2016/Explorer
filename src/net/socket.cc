#include "socket.h"

namespace Explorer {

bool
Socket::create()
{
        fd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        return fd_ != -1;
}

bool
Socket::connect(NetAddress& address)
{
        return 0 == ::connect(fd_, (struct sockaddr* )&address.getAddr(), address.len());
}

bool
Socket::bind(NetAddress& address)
{
        return 0 == ::bind(fd_, (struct sockaddr* )&address.getAddr(), address.len());
}

bool
Socket::listen()
{
        return 0 == ::listen(fd_, BACKLOG);
}

int
Socket::accept(struct sockaddr* address)
{
        int fd;
        socklen_t len = sizeof(struct sockaddr_in);
        assert( 0 < (fd = ::accept(fd_, address, &len)) );
        return fd;
}

} // namespace Explorer
