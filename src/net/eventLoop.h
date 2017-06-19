#ifndef _EXPLORER_EVENT_LOOP_H_
#define _EXPLORER_EVENT_LOOP_H_

#include "../base/base.h"
#include "callback.h"
#include "../base/mutex.h"

namespace Explorer {

class Channel;
class Epoller;

class EventLoop
{
public:


private:
        bool looping_;
        bool quit_;
        Mutex mutex_;

};

} // namespace Explorer

#endif /*_EXPLORER_EVENT_LOOP_H_*/
