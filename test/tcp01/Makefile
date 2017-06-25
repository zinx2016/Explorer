VPATH = ../../net : ../../base

objects = test1.o eventLoop.o epoller.o channel.o currentThread.o \
        timerQueue.o timer.o time.o

test1 : $(objects)
        g++ $(objects) -o test1

.PHONY : clean
clean :
        -rm test1 $(objects)
