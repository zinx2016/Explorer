#include "kvClient.h"

namespace Explorer {

KvClient::KvClient(int threadNums, int queueMaxSize, uint16_t port, const char* ip)
        : loop_(std::make_shared<EventLoop>(threadNums, queueMaxSize)),
          queue_(loop_->GetQueue()),
          con_(Address(port, ip), loop_->GetEpoller()),
          count_(0)
{}

void KvClient::RunDb()
{
        Command();

        // 客户端立即终止
        loop_->RunAfter(0, [this] {this->GetLoop()->Exit();});
}

Connection& KvClient::GetConnection()
{
        return con_;
}

std::shared_ptr<EventLoop> KvClient::GetLoop()
{
        return loop_;
}

void KvClient::Work()
{
        con_.Send(buf_);
}

void KvClient::Command()
{
        int time = 0;

        while(1)
        {
                int argNum;

                if (time == GetCount())
                {
                        std::cout << " > ";
                        time++;
                }
                else
                        continue ;

                GetInput();
                argNum = input_.size();

                if (argNum == 0)
                {
                        --time;
                        continue;
                }
                if (argNum == 1)
                {
                        int ret = Helper();
                        if (ret == -1)
                                return ;
                        else if (ret == 1)
                                continue ;
                        else
                        {
                                --time;
                                continue ;
                        }
                }

                if (!strcasecmp(input_[0].c_str(), "set") ||
                    !strcasecmp(input_[0].c_str(), "get") ||
                    !strcasecmp(input_[0].c_str(), "del") ||
                    !strcasecmp(input_[0].c_str(), "update"))
                {
                        if (CheckCommand())
                                queue_->Push([this] {this->Work();});
                        else
                        {
                                --time;
                                continue ;
                        }
                }
                else
                {
                        --time;
                        printf("   Command Error. Use \"h\" for help.\n");
                }
        }
}

void KvClient::GetInput()
{
        std::string in, arg;

        getline(std::cin, in);
        std::stringstream ss(in);

        const char* tmp = in.c_str();
        buf_.Clear();
        buf_.Append(tmp, strlen(tmp));

        input_.clear();
        while (ss >> arg)
                input_.push_back(arg);
}

void KvClient::Help()
{
        printf("   -----------Help Document-----------\n");
        printf("   commands below are case insensitive\n\n");
        printf("   set key value\n");
        printf("   get key\n");
        printf("   get lkey rkey\n");
        printf("   update key value\n");
        printf("   del key\n");
        printf("   stat\n");
        printf("   q\n");
        printf("   quit\n");
        printf("   h\n");
        printf("   ----------------END----------------\n");
        printf("\n");
}

int KvClient::Helper()
{
        if (!strcasecmp(input_[0].c_str(), "q") ||
            !strcasecmp(input_[0].c_str(), "quit"))
                return -1;
        else if (!strcasecmp(input_[0].c_str(), "stat"))
        {
                queue_->Push([this] {this->Work();});
                return 1;
        }
        else if (!strcasecmp(input_[0].c_str(), "h"))
                Help();
        else if (!strcasecmp(input_[0].c_str(), "set") ||
                 !strcasecmp(input_[0].c_str(), "get") ||
                 !strcasecmp(input_[0].c_str(), "del") ||
                 !strcasecmp(input_[0].c_str(), "update") ||
                 !strcasecmp(input_[0].c_str(), "get"))
                printf("   Argument Error. Use \"h\" for help.\n");
        else
                printf("   Command Error. Use \"h\" for help.\n");
        return 0;
}

bool KvClient::CheckCommand()
{
        int argNum = input_.size();
        if (!strcasecmp(input_[0].c_str(), "set"))
        {
                if (argNum == 3)
                        return true;
        }
        else if (!strcasecmp(input_[0].c_str(), "get"))
        {
                if (argNum == 2 || argNum == 3)
                        return true;
        }
        else if (!strcasecmp(input_[0].c_str(), "del"))
        {
                if (argNum == 2)
                        return true;

        }
        else // "update"
        {
                if (argNum == 3)
                        return true;
        }
        printf("   Argument Error. Use \"h\" for help.\n");
        return false;
}

void KvClient::IncCount()
{
        ++count_;
}

uint16_t KvClient::GetCount()
{
        return count_.Get();
}

} // namespace Explorer
