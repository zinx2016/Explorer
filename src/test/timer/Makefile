objects = test.o channel.o epoller.o socket.o buffer.o log.o eventLoop.o

VPATH = ../../net : ../../include

test : $(objects)
        g++ $(objects) -o test -lpthread

.PHONY : clean
clean :
        -rm test $(objects)
