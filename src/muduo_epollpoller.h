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

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void update(int operation, Channel* channel);
    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};
#endif