#ifndef _EXPLORER_EPOLL_H_
#define _EXPLORER_EPOLL_H_

#include <sys/epoll.h>
#include <vector>
#include "../base/base.h"

namespace Explorer {

class Channel;

class Epoller : private NoCopy
{
purlic:
        typedef std::vector<struct epoll_event> EventList;

        Epoller();

        ~Epoller();

        void addChannel(Channel* channel);

        void updateChannel(Channel* channel);

        void removeChannel(Channel* channel);


private:
        static const InitEventListSize = 1024;

private:
        int epollfd_;
        EventList events_;
};

} // namespace Explorer

#endif /*_EXPLORER_EPOLL_H_*/
