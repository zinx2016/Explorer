#ifndef _EXPLORER_NET_ADDRESS_H_
#define _EXPLORER_NET_ADDRESS_H_

#include <netinet/in.h>
#include <cassert>
#include <stdint.h>

namespace Explorer {

class NetAddress
{
public:
        NetAddress() { }

        explicit NetAddress(uint16_t port); //sin_addr.s_addr = INADDR_ANY

        NetAddress(const char* ip, uint16_t port);

        sa_family_t family() const;
        uint32_t ip() const;
        uint16_t port() const;
        struct sockaddr_in& getAddr();
        socklen_t len() const;

private:
        struct sockaddr_in addr_;

};

} // namespace Explorer

#endif /*_EXPLORER_NET_ADDRESS_H_*/
