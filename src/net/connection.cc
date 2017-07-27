#include <cstring>
#include <cerrno>
#include <cassert>
#include <unistd.h>

#include "connection.h"
#include "epoller.h"
#include "channel.h"

namespace Explorer {

// 服务器端用
Connection::Connection(const Socket& socket, std::shared_ptr<Epoller> epoller)
        : socket_(socket),
          readCb_(nullptr),
          writeCb_(nullptr)
{
        assert(socket_.SetNonBlock()); // socket已连接
        channel_ = std::make_shared<Channel>(socket.Fd(), epoller,
                        [this]() {this->HandleRead();},
                        [this]() {this->HandleWrite();});
        connected_ = true;
}

// 客户端用
Connection::Connection(const Address& server, std::shared_ptr<Epoller> epoller)
        : readCb_(nullptr),
          writeCb_(nullptr)
{
        assert(socket_.Create());
        if (!socket_.Connect(server))
        {
                printf("socket connect server %s failed : %s\n",
                       server.ToString().c_str(), strerror(errno));
                return ;
        }
        assert(socket_.SetNonBlock());
        channel_ = std::make_shared<Channel>(socket_.Fd(), epoller,
                        [this] {this->HandleRead();},
                        [this] {this->HandleWrite();});
        connected_ = true;
}

Connection::~Connection()
{
        // 关闭注册在Connection上Channel的套接字
        // 这里关闭套接字后，会导致析构其成员Channel对象时，RemoveChannel没有套接字而出错
        // 所以，关闭套接字可以移到Channel的析构函数进行
        //Close();
}


Buffer& Connection::GetInput()
{
        return input_;
}

Buffer& Connection::GetOutput()
{
        return output_;
}


void Connection::HandleRead()
{
        if (!connected_)
        {
                printf("connection not open\n");
                return ;
        }
        bool blocked = false;
        Buffer buf;
        uint32_t read = socket_.Read(buf.BeginWrite(),
                                     buf.WritableBytes(),
                                     &blocked);
        input_.Clear();
        input_.Append(buf.BeginRead(), read);
        if (!read && !blocked)
        {
                Close();
                return ;
        }
        if (readCb_ && read)
                readCb_(); // 调用由用户设置的描述符可读的回调函数
}

void Connection::HandleWrite()
{
        if (!connected_)
        {
                printf("connection not open\n");
                return ;
        }
        output_.Clear();
        bool blocked = false;
        uint32_t write = socket_.Write(output_.BeginRead(),
                                       output_.ReadableBytes(),
                                       &blocked);
        if (!write && !blocked)
        {
                Close();
                return ;
        }
        output_.RetriveMove(write);
        if (output_.ReadableBytes() == 0)
        {
                if (writeCb_)
                        writeCb_();
                if (channel_->WriteReady())
                        channel_->EnableWrite(false);
        }
}

void Connection::SetReadCallback(const ReadCallback& cb)
{
        readCb_ = cb;
}

void Connection::SetWriteCallback(const ReadCallback& cb)
{
        writeCb_ = cb;
}


// 服务器在已连接套接字可读时，发送数据给客户端
void Connection::Send(Buffer& buffer)
{
        output_.Append(buffer.BeginRead(), buffer.ReadableBytes());
        buffer.Clear();
        SendOutput();
}

void Connection::Send(const char* buf)
{
        Send(buf, strlen(buf));
}

void Connection::Send(const char* buf, uint32_t len)
{
        output_.Append(buf, len);
        SendOutput();
}

void Connection::SendOutput()
{
        if (!connected_)
        {
                output_.Clear();
                return ;
        }
        bool blocked = false;
        uint32_t write = socket_.Write(output_.BeginRead(),
                                       output_.ReadableBytes(),
                                       &blocked);
        if (!write && !blocked)
        {
                Close();
                return ;
        }
        output_.RetriveMove(write);
}

bool Connection::Success() const
{
        return connected_;
}

bool Connection::Close()
{
        if (socket_.Valid())
        {
                connected_ = false;
                return socket_.Close();
        }
        return true;
}

} // namespace Explorer
