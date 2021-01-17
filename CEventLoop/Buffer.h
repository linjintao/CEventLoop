#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <unistd.h>
#include <vector>
#include <assert.h>
#include <string>
#include <algorithm>

namespace CEventLoop
{
    class Buffer
    {
    public:
        static const size_t kCheapPrepend = 8;
        static const size_t kInitialSize = 1024;

        Buffer(size_t initialSize = kInitialSize) :buf_(kCheapPrepend + kInitialSize), readerIndex_(kCheapPrepend), writerIndex_(kCheapPrepend)
        {

        }

        void hasWritten(size_t len)
        {
            assert(len <= writableBytes());
            writerIndex_ += len;
        }

        char* begin() const
        {
            return const_cast<char*>(buf_.data());
        }

        size_t readableBytes() const
        {
            return writerIndex_ - readerIndex_;
        }

        size_t writableBytes() const
        {
            return buf_.size() - writerIndex_;
        }

        void append(const char* data, size_t len)
        {
            std::copy(data, data + len, begin() + writerIndex_);
            hasWritten(len);
        }

        const char* peek() const
        {
            return begin() + readerIndex_;
        }

        const char* beginWrite() const
        {
            return begin() + writerIndex_;
        }

        const char* findCRLF() const
        {
            const char* crlf = std::search(peek(), beginWrite(), CRLF, CRLF + 2);
            return crlf == beginWrite() ? nullptr : crlf;
        }

        const char* findCRLF2() const
        {
            const char* crlf = std::search(peek(), beginWrite(), CRLF2, CRLF2 + 2);
            return crlf == beginWrite() ? nullptr : crlf;
        }

        void retrieveUtil(const char* end)
        {
            assert(peek() <= end);
            retrieve(end - peek());
        }

        void retrieveAll()
        {
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }

        void retrieve(size_t len)
        {
            assert(len <= readableBytes());
            if (len < readableBytes())
                readerIndex_ += len;
            else
                retrieveAll();
        }

        std::string retrieveAllAsString()
        {
            return retrieveAsString(readableBytes());
        }

        std::string retrieveAsString(size_t len)
        {
            assert(len <= readableBytes());
            std::string result(peek(), len);
            retrieve(len);
            return result;
        }

        ssize_t readFd(int fd, int* err);
    private:
        static const char CRLF[];
        static const char CRLF2[];

        std::vector<char> buf_;
        size_t readerIndex_;
        size_t writerIndex_;
    };
}

#endif