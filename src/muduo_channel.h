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

    void setReadCallback(ReadEventCallback cb) 
    { 
        readCallback_ = std::move(cb);
    }

    void setWriteCallback(EventCallback cb)
    {
        writeCallback_ = std::move(cb);
    }

    void setCloseCallback(EventCallback cb)
    {
        closeCallback_ = std::move(cb);
    }

    void setErrorCallback(EventCallback cb)
    {
        errorCallback_ = std::move(cb);
    }

    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ =revt; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() {return index_; }
    void set_index(int idx) { index_ = idx; }

    EvenLoop* ownerLoop() { return loop_; }
    void remove();
private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EvenLoop* loop_; //事件循环
    const int fd_; //poller监听对象
    int events_; //监听的事件
    int revents_;//poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif