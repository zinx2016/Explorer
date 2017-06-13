#ifndef _EXPLORER_THREAD_H_
#define _EXPLORER_THREAD_H_

#include <functional> //function<>
#include <pthread.h>

namespace Explorer {


class Thread
{
public:
        typedef std::function<void()> ThreadFunc;

        explicit Thread(const ThreadFunc&);

        ~Thread();

        void start();

        int stop();

        pid_t tid() const { return tid_;}

        bool started() const { return started_; }

private:
        pid_t gettid();

private:
        pthread_t pthreadId_;
        pid_t tid_;
        ThreadFunc func_;
        bool started_;
        bool stoped_;
};


}


#endif
