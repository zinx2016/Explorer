#include <iostream>

#include "../../base/threadPool.h"

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
