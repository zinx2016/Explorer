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
        Socket() : fd_(-1) { }

        bool create();
        bool connect(NetAddress& address);
        bool bind(NetAddress& address);
        bool listen();
        int accept(struct sockaddr* address);

private:
        int fd_;
};

} // namespace Explorer

#endif /*_EXPLORER_SOCKET_H_*/
