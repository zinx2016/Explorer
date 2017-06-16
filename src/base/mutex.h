#ifndef _EXPLORER_MUTEX_H_
#define _EXPLORER_MUTEX_H_

#include <pthread.h>
#include <cassert> //assert()
#include "base.h"

#define CHECK(ret) ({__typeof__ (ret) errnum = (ret);           \
                     assert(errnum == 0); (void) errnum;})

namespace Explorer {

class Mutex : private NoCopy
{
public:
        Mutex()
        {
                pthread_mutex_init(&mutex_, NULL);
        }

        ~Mutex()
        {
                CHECK(pthread_mutex_destroy(&mutex_));
        }

        void lock()
        {
                CHECK(pthread_mutex_lock(&mutex_));
        }

        void unlock()
        {
                CHECK(pthread_mutex_unlock(&mutex_));
        }

        pthread_mutex_t* getMutex()
        {
                return &mutex_;
        }

private:
        pthread_mutex_t mutex_;
};

class MutexLockGuard : private NoCopy
{
public:
        explicit MutexLockGuard(Mutex& mutex)
                : mutex_(mutex)
        {
                mutex_.lock();
        }

        ~MutexLockGuard()
        {
                mutex_.unlock();
        }

private:
        Mutex& mutex_;
};

} //namespace Explorer

#endif /*_EXPLORER_MUTEX_H_*/
