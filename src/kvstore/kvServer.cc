#include "kvServer.h"

namespace Explorer {

KvServer::KvServer(int threadNums, int queueMaxSize, const char* filePath)
        : loop_(std::make_shared<EventLoop>(threadNums, queueMaxSize)),
          queue_(loop_->GetQueue()),
          db_(filePath)
{ }

void KvServer::GetArg(std::shared_ptr<Connection> con, Buffer& input)
{
        std::string in(input.BeginRead());
        std::stringstream ss(in);
        std::string arg;
        while (ss >> arg)
                arg_.push_back(arg);
        input.Clear();

        con_ = con;
}

std::shared_ptr<EventLoop> KvServer::GetLoop()
{
        return loop_;
}


void KvServer::RunDb()
{
        while (1)
        {
                if (arg_.empty())
                        continue ;

                if (!strcasecmp(arg_[0].c_str(), "get"))
                {
                        queue_->Push([this] {this->Search();});
                }
                else if (!strcasecmp(arg_[0].c_str(), "set"))
                {
                        queue_->Push([this] {this->Insert();});
                }
                else if (!strcasecmp(arg_[0].c_str(), "update"))
                {
                        queue_->Push([this] {this->Update();});
                }
                else if (!strcasecmp(arg_[0].c_str(), "del"))
                {
                        queue_->Push([this] {this->Del();});
                }
                else // "stat"
                {
                        queue_->Push([this] {this->Stat();});
                }
        }

}


void KvServer::Insert()
{
        char rep[512];

        if (db_.Insert(arg_[1].c_str(), atoi(arg_[2].c_str())) != 0)
                sprintf(rep, "   Key Already Exist.\n");
        else
                sprintf(rep, "   OK.\n");
        buf_.Append(rep, strlen(rep));
        con_->Send(buf_);
        arg_.clear();
}

void KvServer::Update()
{
        char rep[512];

        if (db_.Update(arg_[1].c_str(), atoi(arg_[2].c_str())) != 0)
                sprintf(rep, "   Key Not Exist.\n");
        else
                sprintf(rep, "   OK.\n");
        buf_.Append(rep, strlen(rep));
        con_->Send(buf_);
        arg_.clear();
}

void KvServer::Del()
{
        char rep[512];

        if (db_.Remove(arg_[1].c_str()) != 0)
                sprintf(rep, "   Key Not Found.\n");
        else
                sprintf(rep, "   OK.\n");
        buf_.Append(rep, strlen(rep));
        con_->Send(buf_);
        arg_.clear();
}

void KvServer::Search()
{
        char rep[512];

        if (arg_.size() == 2)
        {
                valueType value;
                if (db_.Search(arg_[1].c_str(), &value) != 0)
                        sprintf(rep, "   Key Not Found.\n");
                else
                        sprintf(rep, "   %d\n", value);
                buf_.Append(rep, strlen(rep));
                con_->Send(buf_);
        }
        else // argNum == 3
        {
                keyType start(arg_[1].c_str());
                valueType values[512];
                bool next = true;
                while (next)
                {
                        int ret = db_.SearchRange(&start,
                                                 arg_[2].c_str(),
                                                 values,
                                                 512,
                                                 &next);
                        if (ret <= 0)
                        {
                                sprintf(rep, "   Key Not Found.\n");
                                buf_.Append(rep, strlen(rep));
                                con_->Send(buf_);

                                return ;
                        }
                        for (int i = 0; i < ret; ++i)
                                sprintf(rep + strlen(rep), "   %d\n", values[i]);
                        buf_.Append(rep, strlen(rep));
                        con_->Send(buf_);
                }
        }
        arg_.clear();
}

void KvServer::Stat()
{
        char rep[512];

        meta metdt = db_.GetMeta();
        sprintf(rep, "   Order        : %d\n", metdt.order);
        sprintf(rep + strlen(rep), "   ValueSize    : %d\n", metdt.valueSize);
        sprintf(rep + strlen(rep), "   KeySize      : %d\n", metdt.keySize);
        sprintf(rep + strlen(rep), "   InternalNode : %d\n", metdt.internalNodeNum);
        sprintf(rep + strlen(rep), "   LeafNode     : %d\n", metdt.leafNodeNum);
        sprintf(rep + strlen(rep), "   Height       : %d\n", metdt.height);
        sprintf(rep + strlen(rep), "   OffSet       : %d\n", metdt.slot);

        buf_.Append(rep, strlen(rep));
        con_->Send(buf_);

        arg_.clear();
}

} // namespace Explorer
