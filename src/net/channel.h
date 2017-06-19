#ifndef _EXPLORER_CHANNEL_H_
#define _EXPLORER_CHANNEL_H_

#include "../base/base.h"

namespace Explorer {

class EventLoop;

class Channel : private NoCopy
{
public:
        typedef Func EventCallBack;

        Channel(EventLoop* loop, int fd);
        ~Channel();

        void handleEvent(); // 最核心函数
        void setReadCallBack(const EventCallBack& cb);
        void setWriteCallBack(const EventCallBack& cb);

        void enableRead();
        void enableWrite();
        void disableRead();
        void disableWrite();
        void update();
        bool readReady();
        bool writeReady();

        int fd() const { return fd_; }
        int events() const { return events_; }
        // 在Epoller对象内被调用
        void set_revents(int revents) { revents_ = revents; }

private:
        static const uint32_t ReadEvent;
        static const uint32_t WriteEvent;

private:
        EventLoop* loop_;
        const int fd_;
        int events_;
        int revents_; //目前的活跃事件

        EventCallBack readCallBack_;
        EventCallBack writeCallBack_;

};

} // namepsace Explorer

#endif /*_EXPLORER_CHANNEL_H_*/
