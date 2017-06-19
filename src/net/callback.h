#ifndef _EXPLORER_CALLBACK_H_
#define _EXPLORER_CALLBACK_H_

#include <functional>
#include <memory>

namespace Explorer {

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void ()> ReadCallBack;
typedef std::function<void ()> WriteCallBack;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallBack;

} // namespace Explorer

#endif /*_EXPLORER_CALLBACK_H_*/
