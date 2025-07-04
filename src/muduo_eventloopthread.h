#ifndef MUDUO_EVENT_LOOP_THREAD_H
#define MUDUO_EVENT_LOOP_THREAD_H

#include "muduo_noncopyable.h"
#include "muduo_thread.h"
#include "muduo_eventloop.h"

#include <functional>
#include <mutex>
#include <condition_variable>
#include <string>

class EventLoopThread: noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(), const std::string &name = std::string());
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
    ThreadInitCallback callback_;
};

#endif