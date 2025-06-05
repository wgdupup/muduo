#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <functional>
#include <memory>

#include "muduo_noncopyable.h"
#include "muduo_timestamp.h"

class EvenLoop;

//channel通信通道，封装了sockfd和注册的event，记录了poller返回的具体事件
class Channel: noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EvenLoop *loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTimestamp);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EvenLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif