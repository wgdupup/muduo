#ifndef MUDUO_POLLER_H
#define MUDUO_POLLER_H

#include "muduo_noncopyable.h"
#include "muduo_timestamp.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

/*多路事件分发器的核心IO复用模块，基类*/
class Poller: noncopyable
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;
    /*判断参数channel是否在当前Poller当中*/
    bool hasChannel(Channel* channel) const;
    /*EventLoop可以通过该接口获取默认的IO复用的具体实现*/
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    //fd->channel，实现fd和channel的映射
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop* ownerLoop_;/*所关联的eventloop*/
};
#endif