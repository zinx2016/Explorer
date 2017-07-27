#ifndef _EXPLORER_CHANNEL_H_
#define _EXPLORER_CHANNEL_H_

#include <memory>

#include "../include/base.h"
#include "callback.h"

namespace Explorer {

class Epoller;

class Channel : private NoCopy
{
public:
        friend class Connection;

        Channel(int fd, std::shared_ptr<Epoller> epoller,
                const ReadCallback& readCb, const WriteCallback& writeCb);
        ~Channel();

        int Fd() const;
        uint32_t Events() const;
        bool ReadReady() const;
        bool WriteReady() const;

        void SetReadCallback(const ReadCallback& readCb);
        void SetWriteCallback(const WriteCallback& writeCb);
        void EnableRead(bool flag);
        void EnableWrite(bool flag);

        void HandleRead();
        void HandleWrite();

private:
        int fd_;
        uint32_t events_;
        std::shared_ptr<Epoller> epoller_;
        ReadCallback readCb_;
        WriteCallback writeCb_;
};

} // namespace Explorer

#endif // _EXPLORER_CHANNEL_H_
