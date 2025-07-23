#ifndef MUDUO_CHANNEL_H
#define MUDUO_CHANNEL_H

#include <functional>
#include <memory>

#include "muduo_noncopyable.h"
#include "muduo_timestamp.h"

class EventLoop;

/*
channel通信通道，封装了sockfd和注册的event，一个fd对应一个channel，
channel里面包括其关联的eventloop，感兴趣的事件，以及相应的事件回调函数
*/
class Channel: noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receiveTimestamp);
    /*设置回调函数*/
    void setReadCallback(ReadEventCallback cb) 
    { 
        readCallback_ = std::move(cb);//左值转换为右值引用，触发移动语义，避免深拷贝
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
    /*防止当channel被手动remove掉，channel还在执行回调操作*/
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ =revt; }

    /*设置fd相应的事件状态*/
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }
    /*返回fd当前的事件状态*/
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() {return index_; }
    void set_index(int idx) { index_ = idx; }
    /*one loop per thread*/
    EventLoop* ownerLoop() { return loop_; }
    void remove();
private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_; //关联的事件循环
    const int fd_; //poller监听对象
    int events_; //监听的事件
    int revents_;//poller返回的具体发生的事件
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;
    //负责fd中相应的事件的回调函数
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

#endif