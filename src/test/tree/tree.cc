#include <string.h>
#include <strings.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>

#include "../../tree/bPlusTree.h"

using namespace std;
using namespace Explorer;

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

void getInput(vector<string>& input)
{
        string in, arg;

        getline(cin, in);
        stringstream ss(in);

        while (ss >> arg)
                input.push_back(arg);
}

void stat(BPlusTree& db)
{
        meta metdt = db.GetMeta();
        printf("   Order        : %d\n", metdt.order);
        printf("   ValueSize    : %d\n", metdt.valueSize);
        printf("   KeySize      : %d\n", metdt.keySize);
        printf("   InternalNode : %d\n", metdt.internalNodeNum);
        printf("   LeafNode     : %d\n", metdt.leafNodeNum);
        printf("   Height       : %d\n", metdt.height);
        printf("   OffSet       : %d\n", metdt.slot);
}

void help()
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

int helper(BPlusTree& db, vector<string>& input, int argNum)
{
        if (!strcasecmp(input[0].c_str(), "q") ||
            !strcasecmp(input[0].c_str(), "quit"))
                return -1;
        else if (!strcasecmp(input[0].c_str(), "stat"))
                stat(db);
        else if (!strcasecmp(input[0].c_str(), "h"))
                help();
        else if (!strcasecmp(input[0].c_str(), "set") ||
                 !strcasecmp(input[0].c_str(), "get") ||
                 !strcasecmp(input[0].c_str(), "del") ||
                 !strcasecmp(input[0].c_str(), "update") ||
                 !strcasecmp(input[0].c_str(), "get"))
                printf("   Argument Error. Use \"h\" for help.\n");
        else
                printf("   Command Error. Use \"h\" for help.\n");
        return 0;
}

void insert(BPlusTree& db, vector<string>& input, int argNum)
{
        if (argNum != 3)
        {
                printf("   Argument Error. Use \"h\" for help.\n");
                return ;
        }
        if (db.Insert(input[1].c_str(), atoi(input[2].c_str())) != 0)
                printf("   Key Already Exist.\n");
}

void update(BPlusTree& db, vector<string>& input, int argNum)
{
        if (argNum != 3)
        {
                printf("   Argument Error. Use \"h\" for help.\n");
                return ;
        }
        if (db.Update(input[1].c_str(), atoi(input[2].c_str())) != 0)
                printf("   Key Not Exist.\n");
}

void del(BPlusTree& db, vector<string>& input, int argNum)
{
        if (argNum != 2)
        {
                printf("   Argument Error. Use \"h\" for help.\n");
                return ;
        }
        if (db.Remove(input[1].c_str()) != 0)
                printf("   Key Not Found.\n");
}

void search(BPlusTree& db, vector<string>& input, int argNum)
{
        if (argNum == 2)
        {
                valueType value;
                if (db.Search(input[1].c_str(), &value) != 0)
                        printf("   Key Not Found.\n");
                else
                        printf("   %d\n", value);
        }
        else if (argNum == 3)
        {
                keyType start(input[1].c_str());
                valueType values[512];
                bool next = true;
                while (next)
                {
                        int ret = db.SearchRange(&start,
                                                 input[2].c_str(),
                                                 values,
                                                 512,
                                                 &next);
                        if (ret <= 0)
                        {
                                printf("   Key Not Found.\n");
                                return ;
                        }
                        for (int i = 0; i < ret; ++i)
                                printf("   %d\n", values[i]);
                }
        }
        else
                printf("   Argument Error. Use \"h\" for help.\n");
}

void command(BPlusTree& db)
{
        while(1)
        {
                int argNum;
                vector<string> input;

                cout << " > ";
                getInput(input);
                argNum = input.size();

                if (argNum == 0)
                        continue;
                if (argNum == 1)
                {
                        int ret = helper(db, input, argNum);
                        if (ret == -1)
                                return ;
                        else
                                continue ;
                }

                if (!strcasecmp(input[0].c_str(), "get"))
                {
                        search(db, input, argNum);
                }
                else if (!strcasecmp(input[0].c_str(), "set"))
                {
                        insert(db, input, argNum);
                }
                else if (!strcasecmp(input[0].c_str(), "update"))
                {
                        update(db, input, argNum);
                }
                else if (!strcasecmp(input[0].c_str(), "del"))
                {
                        del(db, input, argNum);
                }
                else
                        printf("   Command Error. Use \"h\" for help.\n");
        }
}

void runDb()
{
        const char* filePath = "test.db";

        open(filePath, O_CREAT | O_EXCL, FILE_MODE);
        BPlusTree db(filePath);

        command(db);
}

int main()
{
        runDb();

        return 0;
}
