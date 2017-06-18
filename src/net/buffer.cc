#include "buffer.h"
#include <algorithm>
#include <cassert>

namespace Explorer {

size_t
Buffer::readableBytes() const
{
        return writerIndex_ - readerIndex_;
}

size_t
Buffer::writableBytes() const
{
        return buffer_.size() - writerIndex_;
}

size_t
Buffer::prependableBytes() const
{
        return readerIndex_;
}

char*
Buffer::beginRead() const
{
        return const_cast<char*>(&*buffer_.begin()) + readerIndex_;
}

char*
Buffer::beginWrite() const
{
        return const_cast<char*>(&*buffer_.begin()) + writerIndex_;
}

char*
Buffer::begin() const
{
        return const_cast<char*>(&*buffer_.begin()) + PrependSize;
}

void
Buffer::retriveMove(size_t len)
{
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
                readerIndex_ += len;
        }
        else
        {
                readerIndex_ = PrependSize;
                writerIndex_ = PrependSize;
        }
}

void
Buffer::retrive(char* data, size_t len)
{
        assert(len <= readableBytes());
        std::copy(beginRead(), beginRead() + len, data);
        retriveMove(len);
}

void
Buffer::retrive(void* data, size_t len)
{
        retrive(static_cast<char*>(data), len);
}

void
Buffer::append(const char* data, size_t len)
{
        if (len > writableBytes())
        {
                if (writableBytes() + prependableBytes() < len + PrependSize)
                {
                        // 扩充Buffer
                        buffer_.resize(writerIndex_ + len);
                }
                else // 利用头部预留空间
                {
                        // 移动已使用内存起始位置到PrependSize
                        size_t len = readableBytes();
                        std::copy(beginRead(), beginWrite(), begin());
                        readerIndex_ = PrependSize;
                        writerIndex_ = readerIndex_ + len;
                }
                assert(writableBytes() >= len);
        }
        std::copy(data, data + len, beginWrite());
        writerIndex_ += len;
}

void
Buffer::append(const void* data, size_t len)
{
        append(static_cast<const char*>(data), len);
}

void
Buffer::prepend(const void* data, size_t len)
{
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* position = static_cast<const char*>(data);
        std::copy(position, position + len, beginRead());
}

} // namespace Explorer
