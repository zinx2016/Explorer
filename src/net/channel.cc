#include <unistd.h>

#include "channel.h"
#include "epoller.h"

namespace Explorer {

Channel::Channel(int fd, std::shared_ptr<Epoller> epoller,
                 const ReadCallback& readCb, const WriteCallback& writeCb)
        : fd_(fd),
          epoller_(epoller),
          readCb_(readCb),
          writeCb_(writeCb)
{
        events_ = ReadEvent;
        epoller_->AddChannel(this); // 创建Channel时，即向Epoller注册描述符
}

Channel::~Channel()
{
        // 从Epoller移除Channel
        epoller_->RemoveChannel(this);

        close(fd_); // 在这里关闭套接字，而不是在connection的析构函数关闭
}

int Channel::Fd() const
{
        return fd_;
}

uint32_t Channel::Events() const
{
        return events_;
}

bool Channel::ReadReady() const
{
        return events_ & ReadEvent;
}

bool Channel::WriteReady() const
{
        return events_ & WriteEvent;
}

void Channel::SetReadCallback(const ReadCallback& readCb)
{
        readCb_ = readCb;
}

void Channel::SetWriteCallback(const WriteCallback& writeCb)
{
        writeCb_ = writeCb;
}

void Channel::EnableRead(bool flag)
{
        if (flag)
                events_ |= ReadEvent;
        else
                events_ &= ~ReadEvent;
        epoller_->UpdateChannel(this);
}

void Channel::EnableWrite(bool flag)
{
        if (flag)
                events_ |= WriteEvent;
        else
                events_ &= ~WriteEvent;
        epoller_->UpdateChannel(this);
}

void Channel::HandleRead()
{
        readCb_();
}

void Channel::HandleWrite()
{
        writeCb_();
}

} // namespace Explorer
