#include "muduo_poller.h"

#include <stdlib.h>

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    if(getenv("MUDUO_USER_POOL"))
    {
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}