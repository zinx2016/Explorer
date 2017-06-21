#ifndef _EXPLORER_CURRENT_THREAD_H_
#define _EXPLORER_CURRENT_THREAD_H_

#include <pthread.h>

namespace Explorer {

namespace CurrentThread {

// 线程特定数据
extern __thread pid_t cachedTid; // 线程ID

pid_t tid();

} // namespace CurrentThread

} // namespace Explorer

#endif
