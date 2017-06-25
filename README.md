# Explorer(探索者)
基于Raft算法，采用B+树索引实现的分布式Key-Value数据库
## 目前工作进度  
6.16 :   
完成互斥锁、条件变量、原子操作、工作队列、线程、线程池的代码实现，代码：src/base/...；  
并测试通过，测试代码：test/base01/threadPool.cc  
6.21 :     
完成Reactor事件处理框架，包括I/O和定时器功能：EventLoop、Epoller、Channel，TimerQueue，代码：src/net/...；     
并测试通过，测试代码：test/base02/eventLoop.cc、test/tcp01/test1.cc、test/tcp02/test2.cc、test/tcp03/test3.cc
