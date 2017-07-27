#ifndef _EXPLORER_SIGNAL_H_
#define _EXPLORER_SIGNAL_H_

#include <csignal>
#include <functional>
#include <unordered_map>

namespace Explorer {

class Signal
{
public:
        static void Register(int signo, const std::function<void()>& handler)
        {
                handlers_[signo] = handler;
                signal(signo, signal_handler);
        }

        static void signal_handler(int signo)
        {
                handlers_[signo]();
        }

private:
        static std::unordered_map<int, std::function<void()>> handlers_;
};

} // namespace Explorer

#endif // _EXPLORER_SIGNAL_H_
