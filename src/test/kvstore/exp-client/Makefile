objects = exp-client.o kvClient.o channel.o epoller.o socket.o buffer.o log.o sig.o\
        eventLoop.o connection.o server.o bPlusTree.o

VPATH = ../../../net : ../../../include : ../../../tree : ../../../kvstore

exp-cli : $(objects)
        g++ $(objects) -o exp-cli -lpthread

.PHONY : clean
clean :
        -rm exp-cli $(objects)
