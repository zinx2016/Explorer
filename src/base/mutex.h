#ifndef _EXPLORER_MUTEX_H_
#define _EXPLORER_MUTEX_H_

#include "config.h"

#define CHECK(ret) ({__typeof__ (ret) errnum = (ret);           \
                     assert(ret == 0); (void)errnum;})

namespace Explorer {


class Mutex
{
public:
        Mutex() : holder_(0)
        {
                pthread_mutex_init(&mutex_, NULL);
        }

        ~Mutex()
        {
                assert(holder_ == 0);
                CHECK(pthread_mutex_destroy(&mutex_));
        }

        void lock(const pid_t& tid)
        {
                CHECK(pthread_mutex_lock(&mutex_));
                assignHolder(tid);  //必须在CHECK之后
        }

        void unLock()
        {
                unassignHolder();  //必须在CHECK之前
                CHECK(pthread_mutex_unlock(&mutex_));
        }

        void assignHolder(const pid_t& tid) { holder_ = tid; }

        void unassignHolder() { holder_ = 0; }

        bool locked() const { return holder_ != 0; }

private:
        pthread_mutex_t mutex_;
        pid_t holder_;  //持有锁的线程ID
};



}


#endif
