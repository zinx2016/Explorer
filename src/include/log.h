#ifndef _EXPLORER_LOG_H_
#define _EXPLORER_LOG_H_

#include <cassert>
#include <cerrno>
#include <cstring>

namespace Explorer {

#define Info(...) Logv::GetLogger().logv(__VA_ARGS__)
#define Warn(...) Logv::GetLogger().logv(__VA_ARGS__)
#define Error(...) Logv::GetLogger().logv(__VA_ARGS__)
#define Fatal(...) Logv::GetLogger().logv(__VA_ARGS__)
#define ExitIf(b, ...) do {if (b) {Logv::GetLogger().logv(__VA_ARGS__); exit(0);}} while (0)
#define FatalIf(b, ...) do {if (b) {Logv::GetLogger().logv(__VA_ARGS__); assert(0);}} while (0)

class Logv
{
public:
        static Logv& GetLogger();
        void logv(const char* fmt ...);

private:
        Logv();
};

} // namespace Explorer

#endif // _EXPLORER_LOG_H_
