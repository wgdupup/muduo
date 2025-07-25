#include "muduo_event_loop_thread_pool.h"
#include "muduo_eventloopthread.h"
#include "muduo_eventloop.h"

#include <memory>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& nameArg)
    : baseLoop_(baseLoop)
    , name_(nameArg)
    , started_(false)
    , numThreads_(0)
    , next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

/*启动所有eventloop*/
void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    started_ = true;

    for(int i = 0;i < numThreads_; i++)
    {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof(buf), "%s%d", name_.c_str(), i);
        EventLoopThread* t = new EventLoopThread(cb, buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }

    if(numThreads_ == 0 && cb)
    {
        cb(baseLoop_);
    }
}

/*轮询方式获取下一个loop*/
EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;

    if(!loops_.empty())
    {
        loop = loops_[next_];
        ++next_;
        if(next_ >= loops_.size())
        {
            next_ = 0;
        }
    }

    return loop;
}

/*返回所有的loop*/
std::vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(loops_.empty())
    {
        return std::vector<EventLoop*>(1, baseLoop_);
    }
    return loops_;
}