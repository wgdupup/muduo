#include "muduo_eventloop.h"
#include "muduo_logger.h"
#include "muduo_poller.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory>

/*线程局部变量*/
__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0)
    {
        LOG_FATAL("eventfd error:%d \n", errno);
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupFd_(createEventfd())
    , wakeupChannel_(new Channel(this, wakeupFd_))
    , currentActiveChannel_(nullptr)
{
    LOG_DEBUG("EventLoop created %p int thread %d \n", this, threadId_);
    if(t_loopInThisThread)
    {
        LOG_FATAL("another eventloop %p exists", t_loopInThisThread);
    }
    else
    {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

/*开启事件循环*/
void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);
    while(!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for(Channel* channel: activeChannels_)
        {
            channel->handleEvent(pollReturnTime_);
        }

        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping. \n", this);
    looping_ = false;
}

/*
如果是eventloop所在线程调用quit，那么只会置quit
否则需要唤醒另外一个eventloop，这样才能让其在loop循环中退出
*/
void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
    {
        wakeup();
    }
}

/*eventloop执行回调*/
void EventLoop::runInLoop(Functor cb)
{
    if(isInLoopThread())/*在当前的loop线程中，执行cb*/
    {
        cb();
    }
    else
    {
        queueInLoop(cb);/*在非当前loop线程中执行cb，就需要唤醒loop所在线程，执行cb*/
    }
}

/*把cb放入队列中，唤醒loop所在的线程，执行cb*/
void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    /*唤醒对应的线程执行回调函数，callingPendingFunctors_考虑的是对应线程正在执行回调函数，需要再次唤醒*/
    if(!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR("eventloop read %ld bytes instead of 8", n);
    }
}

void EventLoop::removeChannel(Channel* channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel)
{
    return poller_->hasChannel(channel);
}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

/*唤醒loop所在的线程*/
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof(one));
    if(n != sizeof(one))
    {
        LOG_ERROR("eventloop wakeup writes %lu bytes instead of 8\n", n);
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(const Functor& functor : functors)
    {
        functor;/*执行当前loop需要执行的回调操作*/
    }

    callingPendingFunctors_ = false;
}