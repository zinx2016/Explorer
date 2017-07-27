#ifndef _EXPLORER_BASE_H_
#define _EXPLORER_BASE_H_

#include <functional>

namespace Explorer {

class NoCopy
{
public:
        NoCopy() = default;

private:
        NoCopy(const NoCopy&) = delete;
        NoCopy& operator=(const NoCopy&) = delete;
};

template <typename T>
class NoCopyTemplate
{
public:
        NoCopyTemplate() = default;

private:
        NoCopyTemplate(const NoCopyTemplate& ) = delete;
        NoCopyTemplate& operator=(const NoCopyTemplate& ) = delete;
};

typedef std::function<void()> Task;
typedef std::function<void()> Func;

} // namespace Explorer

#endif // _EXPLORER_BASE_H_
