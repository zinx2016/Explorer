#ifndef _EXPLORER_EVENT_LOOP_H_
#define _EXPLORER_EVENT_LOOP_H_

#include <vector>
#include <memory>

#include "../base/base.h"
#include "../base/mutex.h"
#include "../base/thread.h"
#include "../base/currentThread.h"
#include "callback.h"
#include "timerQueue.h"

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

        // 定时器功能
        void runAfter(double seconds, const TimerCallBack& cb);
        void runEvery(double seconds, const TimerCallBack& cb);

private:
        const pid_t threadId_;
        bool looping_;
        bool quit_;
        std::unique_ptr<Epoller> epoller_;
        ChannelList activeChannels_;
        std::unique_ptr<TimerQueue> timerQueue_;
};

} // namespace Explorer

#endif /*_EXPLORER_EVENT_LOOP_H_*/
