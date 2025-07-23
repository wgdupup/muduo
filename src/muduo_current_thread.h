#ifndef MUDUO_CURRENT_THREAD_H
#define MUDUO_CURRENT_THREAD_H

namespace CurrentThread
{
    extern __thread int t_cachedTid;/*线程变量*/

    void cacheTid();

    inline int tid()
    {
        /*分支优化预测*/
        if(__builtin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

#endif