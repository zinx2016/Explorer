#include <unistd.h>
#include <string.h>
#include "epoller.h"
#include "channel.h"

namespace Explorer {

Epoller::Epoller(EventLoop* loop)
        : epollfd_(epoll_create1(EPOLL_CLOEXEC)),
          events_(InitEventListSize),
          ownerLoop_(loop)
{
        assert(epollfd_ > 0);
}

Epoller::~Epoller()
{
        close(epollfd_);
}

void
Epoller::epoll(int epollTimeOut, ChannelList& activeChannels)
{
        int numActives = epoll_wait(epollfd_,
                                    &*events_.begin(),
                                    events_.size(),
                                    epollTimeOut);
        assert(numActives >= 0 || (numActives == -1 && errno == EINTR));
        if (numActives > 0)
        {
                fillActiveChannels(numActives, activeChannels);
                if (static_cast<size_t>(numActives) == events_.size())
                {
                        events_.resize(events_.size() * 2);
                }
        }
        if (numActives == 0)
        {
                // nothing to do.
        }
}

void
Epoller::fillActiveChannels(int numActives, ChannelList& activeChannels)
{
        assert(static_cast<size_t>(numActives) <= events_.size());

        for (int i = 0; i <= numActives; i++)
        {
                Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
                channel->set_revents(events_[i].events);
                activeChannels.push_back(channel);
        }
}

EventLoop*
Epoller::ownerLoop() const
{
        return ownerLoop_;
}

void
Epoller::addChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev));
}

void
Epoller::updateChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev));
}

void
Epoller::removeChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();
        assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev));
}

} // namespace Explorer
