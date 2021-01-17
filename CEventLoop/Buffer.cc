#include "Buffer.h"
#include <sys/uio.h>
#include <errno.h>

const char CEventLoop::Buffer::CRLF[] = "\r\n";
const char CEventLoop::Buffer::CRLF2[] = "\r\n";


ssize_t CEventLoop::Buffer::readFd(int fd, int* err)
{
    char extraBuf[65536];
    iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extraBuf;
    vec[1].iov_len = sizeof(extraBuf);

    const int iovcnt = (writable < sizeof(extraBuf)) ? 2 : 1;
    const int n = ::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *err = errno;
    }
    else if (n <= writable)
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buf_.size();
        append(extraBuf, n - writable);
    }

    return n;
}

