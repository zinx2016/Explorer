objects = test.o channel.o epoller.o socket.o buffer.o log.o eventLoop.o connection.o \
        server.o bPlusTree.o sig.o kvClient.o kvServer.o

VPATH = ../../net : ../../include : ../../tree : ../../kvstore

test : $(objects)
        g++ $(objects) -o test -lpthread

.PHONY : clean
clean :
        -rm test $(objects)
