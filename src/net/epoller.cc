#include "epoll.h"
#include <unistd.h>
#include <string.h>

namespace Explorer {

Epoller::Epoller()
        : epollfd_(epoll_createl(EPOLL_CLOEXEC)),
          events_(InitEventListSize)
{
        assert(epollfd_ > 0);
}

Epoller::~Epoller()
{
        close(epollfd_);
}

void
Epoller::addChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev))
}

void
Epoller::updateChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev))
}

void
Epoller::removeChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev))
}

} // namespace Explorer
