#ifndef _EXPLORER_EPOLL_H_
#define _EXPLORER_EPOLL_H_

#include <sys/epoll.h>
#include <vector>
#include "../base/base.h"
#include "eventLoop.h"

namespace Explorer {

class Channel;

class Epoller : private NoCopy
{
public:
        typedef std::vector<struct epoll_event> EventList;
        typedef std::vector<Channel*> ChannelList;

        Epoller(EventLoop* loop);
        ~Epoller();

        void epoll(int epollTimeOut, ChannelList& activeChannels);
        void fillActiveChannels(int numActives, ChannelList& activeChannels);

        void addChannel(Channel* channel);
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

        EventLoop* ownerLoop() const;

private:
        static const int InitEventListSize = 1024;

private:
        EventLoop* ownerLoop_;
        int epollfd_;
        EventList events_;
};

} // namespace Explorer

#endif /*_EXPLORER_EPOLL_H_*/
