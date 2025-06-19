#ifndef MUDUO_CALLBACK_H
#define MUDUO_CALLBACK_H

#include "muduo_timestamp.h"

#include <memory>
#include <functional>

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnection&)>;
using CloseCallback = std::function<void (const TcpConnection&)>;
using WriteCompleteCallback = std::function<void (const TcpConnection&)>;

using MessageCallback = std::function<void (const TcpConnection&, Buffer*, Timestamp)>;
#endif