#ifndef MUDUO_EPOLL_POLLER_H
#define MUDUO_EPOLL_POLLER_H

#include "muduo_poller.h"

#include <vector>
#include <sys/epoll.h>

class EpollPoller: public Poller
{
public:
    EpollPoller(EventLoop* loop);
    ~EpollPoller() override;

    Timestamp poll(int timeoutMs, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;
private:
    static const int kInitEventListSize = 16;
    /*填写活跃的连接*/
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    /*更新channel对应的事件*/
    void update(int operation, Channel* channel);
    using EventList = std::vector<epoll_event>;

    int epollfd_;/*epoll对应的fd*/
    EventList events_;/*当前发生的事件*/
};
#endif