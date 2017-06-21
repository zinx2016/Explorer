#ifndef _EXPLORER_EVENT_LOOP_H_
#define _EXPLORER_EVENT_LOOP_H_

#include "../base/base.h"
#include "../base/mutex.h"
#include "../base/thread.h"
#include "../base/currentThread.h"
#include <vector>
#include <memory>

namespace Explorer {

class Channel;
class Epoller;

class EventLoop : public NoCopy
{
public:
        typedef std::vector<Channel*> ChannelList;

        EventLoop();
        ~EventLoop();

        // Epoller::epoll_wait()-->Channel::handleEvent();
        void loop();
        void quit();

        // Channel::update()-->EventLoop::updateChannel()-->Epoller::updateChannel()
        void updateChannel(Channel* channel);

private:
        const pid_t threadId_;
        bool looping_;
        bool quit_;
        std::unique_ptr<Epoller> epoller_;
        ChannelList activeChannels_;
};

} // namespace Explorer

#endif /*_EXPLORER_EVENT_LOOP_H_*/
