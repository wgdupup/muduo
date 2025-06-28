#include "muduo_buffer.h"

#include <errno.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t Buffer::readFd(int fd, int* saveErrno)
{
    char exrtabuf[65536] = {0};
    struct iovec vec[2];

    const size_t writable = writeableBytes();

    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;

    vec[1].iov_base = exrtabuf;
    vec[1].iov_len = sizeof(exrtabuf);

    const int iovcnt = (writable < sizeof(exrtabuf) ? 2 : 1);
    const ssize_t n = ::readv(fd, vec, iovcnt);
    
    if(n < 0)
    {
        *saveErrno = errno;
    }
    else if(n <= writable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(exrtabuf, n - writable);
    }

    return n;
}

ssize_t Buffer::writeFd(int fd, int* saveErrno)
{
    ssize_t n = ::write(fd, peek(), readableBytes());
    if(n < 0)
    {
        *saveErrno = errno;
    }
    return n;
}