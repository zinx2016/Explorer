#ifndef _EXPLORER_SOCKET_H_
#define _EXPLORER_SOCKET_H_

#include "address.h"

namespace Explorer {

class Socket
{
public:
        Socket();
        Socket(int fd);

        int Fd() const;
        bool Valid() const;

        bool Create();
        bool Bind(uint16_t port);
        bool Connect(const Address& addr);
        bool Listen();
        int Accept();
        bool Close();

        bool SetOption(int value, int optval);
        bool GetOption(int value, int* optval);
        bool SetReuseAddress();
        bool GetPeerName(Address* addr);
        bool GetSockName(Address* addr);
        bool AddFlag(int flag);
        bool SetNonBlock();

        uint32_t Write(const char* data, uint32_t len, bool* blocked);
        uint32_t Read(char* data, uint32_t len, bool* blocked);

private:
        static const int BACKLOG = 10240;
        int fd_;
};

} // namespace Explorer

#endif // _EXPLORER_SOCKET_H_
