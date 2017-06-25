#ifndef _EXPLORER_CALLBACK_H_
#define _EXPLORER_CALLBACK_H_

#include <functional>
#include <memory>
#include "netAddress.h"

namespace Explorer {

class TcpConnection;

typedef std::function<void ()> ReadCallBack;
typedef std::function<void ()> WriteCallBack;
typedef std::function<void ()> TimerCallBack;
typedef std::function<void (int sockfd, const NetAddress&)> ConnectionCallBack;

} // namespace Explorer

#endif /*_EXPLORER_CALLBACK_H_*/
