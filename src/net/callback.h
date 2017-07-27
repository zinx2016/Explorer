#ifndef _EXPLORER_CALLBACK_H_
#define _EXPLORER_CALLBACK_H_

#include <functional>
#include <memory>

namespace Explorer {

class Connection;

typedef std::function<void()> ReadCallback;
typedef std::function<void()> WriteCallback;
typedef std::function<void(std::shared_ptr<Connection>)> ConnectCallback;

} // namespace Explorer

#endif // _EXPLORER_CALLBACK_H_
