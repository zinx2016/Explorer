#include <unistd.h>
#include <cstring>

#include "epoller.h"
#include "channel.h"
#include "../include/log.h"

namespace Explorer {

Epoller::Epoller()
        : events_(MaxEvents)
{
        FatalIf((fd_ = epoll_create1(EPOLL_CLOEXEC)) < 0,
                "epoll create failed, %s", strerror(errno));
}

Epoller::~Epoller()
{
        close(fd_);
}

void Epoller::AddChannel(Channel* channel)
{
        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = channel->Events();
        event.data.ptr = channel;
        FatalIf(epoll_ctl(fd_, EPOLL_CTL_ADD, channel->Fd(), &event),
                "epoll add event failed, %s", strerror(errno));
}

void Epoller::RemoveChannel(Channel* channel)
{
        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = channel->Events();
        event.data.ptr = channel;
        FatalIf(epoll_ctl(fd_, EPOLL_CTL_DEL, channel->Fd(), &event),
                "epoll remove event failed, %s fd: %d",
                strerror(errno),
                channel->Fd());
}

void Epoller::UpdateChannel(Channel* channel)
{
        struct epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = channel->Events();
        event.data.ptr = channel;
        FatalIf(epoll_ctl(fd_, EPOLL_CTL_MOD, channel->Fd(), &event),
                "epoll update event failed, %s ", strerror(errno));
}

void Epoller::LoopOnce(int ms)
{
        int ready = epoll_wait(fd_, &*events_.begin(), events_.size(), ms);
        FatalIf(ready == -1 && errno != EINTR,
                "epoll wait failed, %s", strerror(errno));
        for (; --ready >= 0; ) {
                Channel *channel = (Channel *)events_[ready].data.ptr;
                uint32_t event = events_[ready].events;
                if (event & ReadEvent) {
                        channel->HandleRead();
                } else if (event & WriteEvent) {
                        channel->HandleWrite();
                } else {
                        Fatal("unexpected epoll event");
                }
        }
}

} // namespace Explorer
