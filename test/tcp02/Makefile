VPATH = ../../net : ../../base

objects = test2.o timerQueue.o timer.o time.o eventLoop.o epoller.o channel.o \
        currentThread.o

test2 : $(objects)
        g++ $(objects) -o test2

.PHONY : clean
clean :
        -rm test2 $(objects)
