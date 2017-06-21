#include "currentThread.h"
#include <sys/syscall.h>
#include <unistd.h>

namespace Explorer {

namespace CurrentThread {

// 线程特定数据
__thread pid_t cachedTid = 0; // 线程ID

inline pid_t gettid()
{
        return static_cast<pid_t>(syscall(SYS_gettid));
}

pid_t tid()
{
        if (cachedTid == 0)
        {
                cachedTid = gettid();
        }
        return cachedTid;
}

} // namespace CurrentThread

} // namespace Explorer
