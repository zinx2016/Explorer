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
                fillActiveChannels(numActives, activeChannels); // 将活跃事件添加到待处理的队列中
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

        for (int i = 0; i < numActives; i++)
        {
                Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
                channel->set_revents(events_[i].events); //设置channel的活跃事件
                activeChannels.push_back(channel);
        }
}

EventLoop*
Epoller::ownerLoop() const
{
        return ownerLoop_;
}

bool
Epoller::existChannel(Channel* channel) const
{
        channel_.find(channel->fd()) != channel_.end();
}

void
Epoller::updateChannel(Channel* channel)
{
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.events = channel->events();
        ev.data.ptr = channel;
        int fd = channel->fd();

        if (existChannel(channel)) // 更新
                assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &ev));
        else // 添加
                assert(0 == epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev));
        channel_[fd] = channel;
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
        channel_.erase(fd);
}

} // namespace Explorer
