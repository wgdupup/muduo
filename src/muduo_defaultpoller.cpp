#include "muduo_poller.h"
#include "muduo_epollpoller.h"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    if(getenv("MUDUO_USER_POOL"))
    {
        return nullptr;/*生成poll的实例*/
    }
    else
    {
        return new EpollPoller(loop);/*生成epoll的实例*/
    }
}