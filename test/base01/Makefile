VPATH = ../../net : ../../base

objects = eventloop.o timerQueue.o timer.o time.o eventLoop.o epoller.o channel.o \
        currentThread.o thread.o

eventloop : $(objects)
        g++ $(objects) -o eventloop -lpthread

.PHONY : clean
clean :
        -rm eventloop $(objects)
