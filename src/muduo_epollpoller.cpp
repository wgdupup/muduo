#include "muduo_epollpoller.h"
#include "muduo_logger.h"
#include "muduo_channel.h"

#include <errno.h>
#include <unistd.h>
#include <string.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller(EventLoop* loop)
    : Poller(loop)
    , epollfd_(epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error:%d \n",errno);
    }
}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

/*获取活跃的channel*/
Timestamp EpollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    LOG_INFO("Poll starting\n");

    int numEvents = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now());

    if(numEvents > 0)
    {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activeChannels);
        if(numEvents == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if(numEvents == 0)
    {
        LOG_DEBUG("timeout!\n");
    }
    else
    {
        if(saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_ERROR("poll err!\n");
        }
    }
    return now;
}

/*更新channel，主要包括添加或者删除*/
void EpollPoller::updateChannel(Channel* channel)
{
    const int index = channel->index();

    LOG_INFO("fd=%d events=%d index=%d \n", channel->fd(), channel->events(), index);
    if(index == kNew || index == kDeleted)
    {
        if(index == kNew)
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        if(channel->isNoneEvent())/*当前监听事件为空，则进行删除*/
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else/*当前监听事件不为空，则进行修改*/
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

/*从poller中删除channel*/
void EpollPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("fd=%d events=%d \n", channel->fd(), channel->events());
    int index = channel->index();
    if(index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

/*设置事件对应的channel中发生的事件，然后将这些activechannel返回*/
void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for(int i = 0; i < numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

/*将channel监听的事件进行epoll_ctl中的add/mod/del操作*/
void EpollPoller::update(int operation, Channel* channel)
{
    epoll_event event;
    memset(&event, 0 , sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();

    if(epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll del error:%d\n", errno);
        }
        else
        {
            LOG_FATAL("epoll add or modify error:%d\n", errno);
        }
    }
}