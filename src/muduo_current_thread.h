#ifndef MUDUO_CURRENT_THREAD_H
#define MUDUO_CURRENT_THREAD_H

namespace CurrentThread
{
    extern __thread int t_cachedTid;

    void cacheTid();

    inline int tid()
    {
        if(__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif