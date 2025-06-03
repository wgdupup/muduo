#ifndef MUDUO_BASE_NONCOPYABLE_H
#define MUDUO_BASE_NONCOPYABLE_H

/*
    将拷贝构造函数和赋值运算符定义为删除的，后续的派生类继承该类后，派生类是无法拷贝构造和通过=进行赋值的
*/
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) =delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

#endif