objects = exp-server.o kvServer.o channel.o epoller.o socket.o buffer.o log.o sig.o\
        eventLoop.o connection.o server.o bPlusTree.o

VPATH = ../../../net : ../../../include : ../../../tree : ../../../kvstore

exp-srv : $(objects)
        g++ $(objects) -o exp-srv -lpthread

.PHONY : clean
clean :
        -rm exp-srv $(objects)
