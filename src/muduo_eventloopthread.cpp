#include "muduo_eventloopthread.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb, const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name)
    , mutex_()
    , cond_()
    , callback_(cb)
{

}

EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

/*开启事件循环*/
EventLoop* EventLoopThread::startLoop()
{
    thread_.start();

    EventLoop* loop = nullptr;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (( loop_ == nullptr))
        {
            cond_.wait(lock);
        }
        loop = loop_;
    }
    /*返回线程关联的loop*/
    return loop;
}

/*线程执行函数*/
void EventLoopThread::threadFunc()
{
    EventLoop loop;

    if(callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.loop();
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
}

