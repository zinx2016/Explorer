#ifndef _EXPLORER_EPOLLER_H_
#define _EXPLORER_EPOLLER_H_

#include <sys/epoll.h>
#include <vector>

#include "../include/base.h"

const uint32_t ReadEvent = EPOLLIN;
const uint32_t WriteEvent = EPOLLOUT;
const uint32_t MaxEvents = 1024;

namespace Explorer {

class Channel;

class Epoller : private NoCopy
{
public:
        Epoller();
        ~Epoller();

        void AddChannel(Channel* channel);
        void RemoveChannel(Channel* channel);
        void UpdateChannel(Channel* channel);

        void LoopOnce(int ms);

private:
        int fd_;
        std::vector<struct epoll_event> events_;
};

} // namespace Explorer

#endif // _EXPLORER_EPOLLER_H_
