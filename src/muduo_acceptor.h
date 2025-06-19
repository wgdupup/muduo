#ifndef MUDUO_ACCEPTOR_H
#define MUDUO_ACCEPTOR_H

#include "muduo_noncopyable.h"
#include "muduo_socket.h"
#include "muduo_channel.h"

class EventLoop;
class InetAddress;

class Acceptor : noncopyable
{
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;
    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    {
        NewConnectionCallback_ = cb;
    }

    bool listening() const { return listening_; }
    void listen();
    void handleRead();
    
private:
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback NewConnectionCallback_;
    bool listening_;
};

#endif 
