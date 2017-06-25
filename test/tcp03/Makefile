objects = test3.o acceptor.o eventLoop.o channel.o epoller.o currentThread.o \
        socket.o time.o timer.o timerQueue.o netAddress.o

VPATH = ../../net : ../../base

test3 : $(objects)
        g++ $(objects) -o test3

.PHONY : clean
clean :
        -rm test3 $(objects)
