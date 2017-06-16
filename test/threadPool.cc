// 编译方式：
// g++ threadPool.cc ../thread.cc -o threadPool -lpthread
// ./threadPool
// 结果：
// f() called
// f() called
// g() called
//

#include "../threadPool.h"
#include <iostream>

using std::cout;
using std::endl;

void f()
{
        cout << "f() called" << endl;
}

void g()
{
        cout << "g() called" << endl;
}

int main()
{
        using Explorer::ThreadPool;
        using Explorer::WorkQueue;
        using Explorer::Task;

        WorkQueue<Task> work(3);
        work.push(f);
        work.push(f);
        work.push(g);

        ThreadPool<Task> tpool(work, 2);
        tpool.start();
}
