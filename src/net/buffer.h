#ifndef _EXPLORER_BUFFER_H_
#define _EXPLORER_BUFFER_H_

#include <vector>
#include <cstddef>

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

        size_t readableBytes() const;

        size_t writableBytes() const;

        size_t prependableBytes() const;

        char* beginRead() const;

        char* beginWrite() const;

        char* begin() const;

        void retriveMove(size_t len);

        // 从Buffer提取数据到data
        void retrive(char* data, size_t len);

        void retrive(void* data, size_t len);

        // 追加写数据到Buffer
        void append(const char* data, size_t len);

        void append(const void* data, size_t len);

        // 写数据到Buffer的beginRead()-len到beginRead()
        void prepend(const void* data, size_t len);

private:
        std::vector<char> buffer_;
        size_t readerIndex_;
        size_t writerIndex_;
};

} // namespace Explorer

#endif /*_EXPLORER_BUFFER_H_*/
