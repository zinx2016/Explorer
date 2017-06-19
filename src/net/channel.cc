#include "channel.h"
#include "eventLoop.h"

namespace Explorer {

const uint32_t ReadEvent = EPOLLIN;
const uint32_t WriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
        : loop_(loop),
          fd_(fd)
          events_(0),
          revents_(0)
{ }

Channel::~Channel()
{
        if (loop_->isInLoopThread())
        {
                assert(!loop_->hasChannel(this));
        }
}

void
Channel::handleEvent()
{
        if (revents_ & ReadEvent)
        {
                if (readCallBack_)
                        readCallBack_();
        }
        if (revents_ & WriteEvent)
        {
                if (writeCallBack_)
                        writeCallBack_();
        }
}

void
Channel::setReadCallBack(const EventCallBack& cb)
{
        readCallBack_ = cb;
}

void
Channel::setWriteCallBack(const EventCallBack& cb)
{
        writeCallBack_ = cb;
}

void
Channel::enableRead()
{
        events_ |= ReadEvent;
        update();
}

void
Channel::enableWrite()
{
        events_ |= WriteEvent;
        update();
}

void
Channel::disableRead()
{
        events_ &= ~ReadEvent;
        update();
}

void
Channel::disableWrite()
{
        events_ &= ~WriteEvent;
        update();
}

void
Channel::update()
{
        loop_->updateChannel(this);
}

bool
Channel::readReady()
{
        return events_ & ReadEvent;
}

bool
Channel::writeReady()
{
        return events_ & writeEvent;
}

} // namespace Explorer
