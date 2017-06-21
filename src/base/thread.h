#ifndef _EXPLORER_THREAD_H_
#define _EXPLORER_THREAD_H_

#include <functional> //function<>
#include <pthread.h>
#include "base.h"

namespace Explorer {

class Thread : private NoCopy
{
public:
        explicit Thread(const Func&);

        ~Thread();

        void start();

        int stop();

        pid_t tid();

        bool started() const { return started_; }

        bool stoped() const { return stoped_; }

private:
        pthread_t pthreadId_;
        pid_t tid_;
        Func func_;
        bool started_;
        bool stoped_;
};

} // namespace Explorer

#endif /*_EXPLOER_THREAD_H_*/
