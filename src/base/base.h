#ifndef _EXPLORER_BASE_H_
#define _EXPLORER_BASE_H_

namespace Explorer {

class NoCopy
{
public:
        NoCopy() = default;

private:
        NoCopy(const NoCopy& ) = delete;
        NoCopy& (const NoCopy& ) = delete;
};

typedef std::function<void ()> Task;
typedef std::function<void ()> Func;

} // namespace Explorer

#endif  /*_EXPLORER_BASE_H_*/
