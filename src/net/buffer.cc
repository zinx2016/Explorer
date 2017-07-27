#include "buffer.h"
#include <algorithm>
#include <cassert>
#include <sys/uio.h>

namespace Explorer {

ssize_t
Buffer::Read(int fd, int* savedErrno)
{
        char extrabuf[65536]; // 64KB
        struct iovec vec[2];
        vec[0].iov_base = Begin() + writerIndex_;
        vec[0].iov_len = WritableBytes();
        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof(extrabuf);

        ssize_t n = readv(fd, vec, 2); // 分散读
        if ( n < 0)
                *savedErrno = errno;
        else if ( n < WritableBytes()) // 优先读到buffer_里
                writerIndex_ += n;
        else
        {
                writerIndex_ = buffer_.size(); // buffer_写满
                Append(extrabuf, n - WritableBytes()); // 剩下的追加到buffer_后
        }
        return n;
}

size_t
Buffer::ReadableBytes() const
{
        return writerIndex_ - readerIndex_;
}

size_t
Buffer::WritableBytes() const
{
        return buffer_.size() - writerIndex_;
}

size_t
Buffer::PrependableBytes() const
{
        return readerIndex_;
}

char*
Buffer::BeginRead() const
{
        return const_cast<char*>(&*buffer_.begin()) + readerIndex_;
}

char*
Buffer::BeginWrite() const
{
        return const_cast<char*>(&*buffer_.begin()) + writerIndex_;
}

char*
Buffer::Begin() const
{
        return const_cast<char*>(&*buffer_.begin()) + PrependSize;
}

void
Buffer::RetriveMove(size_t len)
{
        assert(len <= ReadableBytes());
        if (len < ReadableBytes())
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
Buffer::Retrive(char* data, size_t len)
{
        assert(len <= ReadableBytes());
        std::copy(BeginRead(), BeginRead() + len, data);
        RetriveMove(len);
}

void
Buffer::Retrive(void* data, size_t len)
{
        Retrive(static_cast<char*>(data), len);
}

void
Buffer::Append(const char* data, size_t len)
{
        if (len > WritableBytes())
        {
                if (WritableBytes() + PrependableBytes() < len + PrependSize)
                {
                        // 扩充Buffer
                        buffer_.resize(writerIndex_ + len);
                }
                else // 利用头部预留空间
                {
                        // 移动已使用内存起始位置到PrependSize
                        size_t len = ReadableBytes();
                        std::copy(BeginRead(), BeginWrite(), Begin());
                        readerIndex_ = PrependSize;
                        writerIndex_ = readerIndex_ + len;
                }
                assert(WritableBytes() >= len);
        }
        std::copy(data, data + len, BeginWrite());
        writerIndex_ += len;
}

void
Buffer::Append(const void* data, size_t len)
{
        Append(static_cast<const char*>(data), len);
}

void
Buffer::Prepend(const void* data, size_t len)
{
        assert(len <= PrependableBytes());
        readerIndex_ -= len;
        const char* position = static_cast<const char*>(data);
        std::copy(position, position + len, BeginRead());
}

void
Buffer::Clear()
{
        readerIndex_ = writerIndex_ = PrependSize;
        buffer_.clear();
        buffer_.resize(PrependSize + InitialSize);
}

void
Buffer::Unget(size_t len)
{
        MoveReaderIndex(-len);
}

void Buffer::MoveWriterIndex(ssize_t len)
{
        if (len > 0)
        {
                size_t size = len < WritableBytes() ? len : WritableBytes();
                writerIndex_ += size;
        }
        else
        {
                size_t size = (-len) < (ReadableBytes() + PrependableBytes()) ?
                              (-len) : (ReadableBytes() + PrependableBytes());
                writerIndex_ -= size;
                if (writerIndex_ < readerIndex_)
                        readerIndex_ = writerIndex_;
        }
}

void Buffer::MoveReaderIndex(ssize_t len)
{
        if (len > 0)
        {
                size_t size = (len) < (ReadableBytes() + WritableBytes()) ?
                              (len) : (ReadableBytes() + WritableBytes());
                readerIndex_ += size;
        }
        else
        {
                size_t size = (-len) < PrependableBytes() ? (-len) : PrependableBytes();
                readerIndex_ -= size;
        }

}

} // namespace Explorer
