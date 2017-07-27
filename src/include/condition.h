#ifndef _EXPLORER_CONDITION_H_
#define _EXPLORER_CONDITION_H_

#include <errno.h> // ETIMEDOUT
#include <sys/time.h> // timespec

#include "base.h"
#include "mutex.h"

namespace Explorer {

class Condition : private NoCopy
{
public:
        Condition() { assert(0 == pthread_cond_init(&cond_, 0)); }

        ~Condition() { assert(0 == pthread_cond_destroy(&cond_)); }

        void Wait(Mutex& mutex) { assert(0 == pthread_cond_wait(&cond_,
                                                                mutex.GetMutex())); }

        void Signal() { assert(0 == pthread_cond_signal(&cond_)); }

        void BroadCast() { assert(0 == pthread_cond_broadcast(&cond_));}

        bool TimeWait(Mutex& mutex, double seconds)
        {
                struct timespec abstime;
                const int64_t nanoTimePerSecond = 1e9;
                int64_t nanoSeconds = static_cast<int64_t>(seconds * nanoTimePerSecond);

                struct timeval now;
                gettimeofday(&now, NULL);  //系统不支持clock_gettime()函数
                abstime.tv_sec = now.tv_sec;
                abstime.tv_nsec = now.tv_usec * 1000;
                abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoSeconds)
                                                        / nanoTimePerSecond);
                abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoSeconds)
                                                        % nanoTimePerSecond);

                return ETIMEDOUT ==
                       pthread_cond_timedwait(&cond_, mutex.GetMutex(), &abstime);

        }

private:
        pthread_cond_t cond_;

};

} // namespace Explorer

#endif // _EXPLORER_CONDITION_H_
