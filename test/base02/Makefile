VPATH = ../../net : ../../base

objects = threadPool.o timerQueue.o timer.o time.o eventLoop.o epoller.o channel.o \
        currentThread.o thread.o

threadPool : $(objects)
        g++ $(objects) -o threadPool -lpthread

.PHONY : clean
clean :
        -rm threadPool $(objects)
