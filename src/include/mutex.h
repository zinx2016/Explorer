#ifndef _EXPLORER_MUTEX_H_
#define _EXPLORER_MUTEX_H_

#include <pthread.h>
#include <cassert>

#include "base.h"

namespace Explorer {

class Condition;

class Mutex : private NoCopy
{
public:
        friend class Condition;

        Mutex() { assert(0 == pthread_mutex_init(&mutex_, 0)); }

        ~Mutex() { assert(0 == pthread_mutex_destroy(&mutex_)); }

        void Lock() { assert(0 == pthread_mutex_lock(&mutex_)); }

        void Unlock() { assert(0 == pthread_mutex_unlock(&mutex_)); }

        pthread_mutex_t* GetMutex() { return &mutex_; }

private:
        pthread_mutex_t mutex_;
};

class MutexGuard : private NoCopy
{
public:
        MutexGuard(Mutex& mutex)
                : mutex_(mutex)
        {
                mutex_.Lock();
        }

        ~MutexGuard() { mutex_.Unlock(); }

private:
        Mutex& mutex_;
};

} // namespace Explorer

#endif // _EXPLORER_MUTEX_H_
