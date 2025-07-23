#include "muduo_thread.h"
#include "muduo_current_thread.h"

#include <semaphore.h>

std::atomic_int32_t Thread::numCreated_ {0};

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false)
    , joined_(false)
    , tid_(0)
    , func_(std::move(func))
    , name_(name)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);

    thread_ = std::shared_ptr<std::thread>(new std::thread([&](){
        tid_ = CurrentThread::tid();
        sem_post(&sem);
        /*线程执行函数*/
        func_();
    }));

    sem_wait(&sem);/*等待获取上面新创建的线程的tid值*/
}

void Thread::join()/*回收线程*/
{
    joined_ = true;
    thread_->join();
}

/*设置线程名*/
void Thread::setDefaultName()
{
    int num = ++numCreated_;

    if(name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "Thread%d", num);
        name_ = buf;
    }
}