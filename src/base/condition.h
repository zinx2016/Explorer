#ifndef _EXPLORER_CONDITION_H_
#define _EXPLORER_CONDITION_H_

#include "mutex.h"
#include <errno.h> //ETIMEDOUT
#include <sys/time.h> //timespec,...

namespace Explorer {

class Condition : private NoCopy
{
public:
        explicit Condition(Mutex& mutex)
          : mutex_(mutex)
        {
                CHECK(pthread_cond_init(&cond_, NULL));
        }

        ~Condition()
        {
                CHECK(pthread_cond_destroy(&cond_));
        }

        void wait()
        {
                CHECK(pthread_cond_wait(&cond_, mutex_.getMutex()));
        }

        bool waitForSeconds(double seconds)
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

                return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.getMutex(), &abstime);
        }

        void notify()
        {
                CHECK(pthread_cond_signal(&cond_));
        }

        void notifyAll()
        {
                CHECK(pthread_cond_broadcast(&cond_));
        }

private:
        pthread_cond_t cond_;
        Mutex&  mutex_;
};

} // namespace Explorer

#endif /*_EXPLORER_CONDITION_H_*/
