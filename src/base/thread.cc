#include "thread.h"
#include <sys/syscall.h>  //for syscall()
#include <unistd.h>
#include <cassert>

namespace Explorer {

struct ThreadData
{
        ThreadData(pid_t tid, Func func)
            : tid_(tid),
              func_(func)
        { }

        pid_t tid_;
        Func func_;
};

void* startThread(void* arg)  //不能作为类成员函数
{
        ThreadData *data = (ThreadData*)arg;
        data->func_();  //运行线程函数
}

Thread::Thread(const Func& func)
        : pthreadId_(0),
          tid_(0),
          func_(func),
          started_(false),
          stoped_(false)
{ }

Thread::~Thread()
{
        if (started_ && !stoped_)
        {
                pthread_detach(pthreadId_);
        }
}

pid_t Thread::gettid()
{
        return static_cast<pid_t>(syscall(SYS_gettid));
}

void Thread::start()
{
        assert(!started_);
        tid_ = gettid();
        started_ = true;
        ThreadData *data = new ThreadData(tid_, func_);
        if( 0 != pthread_create(&pthreadId_, NULL, startThread, (void*)data))
        {
                delete data;
                tid_ = 0;
                started_ = false;
                pthreadId_ = 0;
        }
}

int Thread::stop()
{
        assert(started_);
        assert(!stoped_);
        stoped_ = true;
        return pthread_join(pthreadId_, NULL);
}


}
