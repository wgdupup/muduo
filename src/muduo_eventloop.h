#ifndef MUDUO_EVENTLOOP_H
#define MUDUO_EVENTLOOP_H

#include "muduo_noncopyable.h"
#include "muduo_channel.h"
#include "muduo_poller.h"
#include "muduo_current_thread.h"

#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
//时间循环类，主要包括channel和poller两个类
class EventLoop: noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }
    //在当前loop中执行cb
    void runInLoop(Functor cb);
    //把cb放入队列中，唤醒loop所在的线程，执行cb
    void queueInLoop(Functor cb);
    //唤醒loop所在的线程的
    void wakeup();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    //判断eventloop对象是否在自己的线程里面
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    void handleRead();//wake up
    void doPendingFunctors();

    using ChannelList = std::vector<Channel*>;

    std::atomic_bool looping_; 
    std::atomic_bool quit_;    /*标识退出loop循环*/
    const pid_t threadId_;  /*所属线程id*/
    Timestamp pollReturnTime_; /*poller返回发生事件的channels的时间点*/
    std::unique_ptr<Poller> poller_; /*关联的poller*/

    int wakeupFd_; /*用于唤醒loop*/
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    std::atomic_bool callingPendingFunctors_; /*标识当前loop是否有需要执行的回调操作*/
    std::vector<Functor> pendingFunctors_; /*存储loop需要执行的所有的回调操作*/
    std::mutex mutex_; /*互斥锁，用来保护上面vector容器的线程安全操作*/
};

#endif