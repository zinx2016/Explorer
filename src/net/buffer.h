#ifndef _EXPLORER_BUFFER_H_
#define _EXPLORER_BUFFER_H_

#include <vector>
#include <cstddef>
#include <sys/types.h>
#include <errno.h>

namespace Explorer {

class Buffer
{
public:
        static const size_t InitialSize = 1024;
        static const size_t PrependSize = 8;

        explicit Buffer(size_t initialSize = InitialSize)
            : buffer_(PrependSize + initialSize),
              readerIndex_(PrependSize),
              writerIndex_(PrependSize)
        { }

        ssize_t Read(int fd, int* savedErrno);

        size_t ReadableBytes() const;

        size_t WritableBytes() const;

        size_t PrependableBytes() const;

        char* BeginRead() const;

        char* BeginWrite() const;

        char* Begin() const;

        void RetriveMove(size_t len);

        // 从Buffer提取数据到data
        void Retrive(char* data, size_t len);

        void Retrive(void* data, size_t len);

        // 追加写数据到Buffer
        void Append(const char* data, size_t len);

        void Append(const void* data, size_t len);

        // 写数据到Buffer的beginRead()-len到beginRead()
        void Prepend(const void* data, size_t len);

        void Clear();

        void Unget(size_t len);

        void MoveWriterIndex(ssize_t len);

        void MoveReaderIndex(ssize_t len);

private:
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;
};

} // namespace Explorer

#endif /*_EXPLORER_BUFFER_H_*/
