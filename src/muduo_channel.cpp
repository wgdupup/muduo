#include "muduo_channel.h"
#include "muduo_logger.h"
#include "muduo_eventloop.h"
#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1), tied_(false)
{

}

Channel::~Channel()
{

}

/*
使用弱引用跟踪tcpconnection的生命周期，防止在资源已被释放的情况下执行回调，
因为其使用的回调函数与tcpconnection有关
*/
void Channel::tie(const std::shared_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

/*更新channel事件*/
void Channel::update()
{
    /*通过channel所属的EventLoop，调用poller的相应方法，注册fd的events事件*/
    loop_->updateChannel(this);
}

/*在channel所属的EventLoop中， 把当前的channel删除掉*/
void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if(tied_)
    {   /*将弱引用提升为shared_ptr，当资源引用计数归零后返回空指针*/
        guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

/*根据poller通知的channel发生的具体事件， 由channel负责调用具体的回调操作*/
void Channel::handleEventWithGuard(Timestamp receiveTime)
{
    LOG_INFO("channel handleEvent revents:%d\n", revents_);

    if((revents_ & EPOLLHUP) &&!(revents_ & EPOLLIN))
    {
        if(closeCallback_)
        {
            closeCallback_();
        }
    }

    if(revents_ & EPOLLERR)
    {
        if(errorCallback_)
        {
            errorCallback_();
        }
    }

    if(revents_ & (EPOLLIN | EPOLLPRI))
    {
        if(readCallback_)
        {
            readCallback_(receiveTime);
        }
    }

    if(revents_ & EPOLLOUT)
    {
        if(writeCallback_)
        {
            writeCallback_();
        }
    }
}