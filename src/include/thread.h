#ifndef _EXPLORER_THREAD_H_
#define _EXPLORER_THREAD_H_

#include <thread>

#include "base.h"

namespace Explorer {

class Thread : private NoCopy
{
public:
        Thread(const Func& func)
                : func_(func)
        {}

        void Start()
        {
                std::thread tmp(func_);
                thread_.swap(tmp);
        }

        void Stop() { thread_.join(); }

private:
        Func func_;
        std::thread thread_;
};

} // namespace Explorer

#endif // _EXPLORER_THREAD_H_
