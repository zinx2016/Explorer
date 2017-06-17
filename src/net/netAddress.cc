#include "netAddress.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace Explorer {

NetAddress::NetAddress(uint16_t port)
{
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        addr_.sin_addr.s_addr = htonl(INADDR_ANY);
}

NetAddress::NetAddress(const char* ip, uint16_t port)
{
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        assert(inet_pton(AF_INET, ip, &addr_.sin_addr) == 1);
}

sa_family_t
NetAddress::family() const
{
        return addr_.sin_family;
}

uint32_t
NetAddress::ip() const
{
        return addr_.sin_addr.s_addr;
}

uint16_t
NetAddress::port() const
{
        return addr_.sin_port;
}

struct sockaddr_in&
NetAddress::getAddr()
{
        return addr_;
}

socklen_t
NetAddress::len() const
{
        return sizeof(addr_);
}

} // namespace Explorer
