objects = channel.o epoller.o socket.o buffer.o log.o sig.o\
        eventLoop.o connection.o server.o bPlusTree.o

objects1 = exp-client.o kvClient.o $(objects)

objects2 = exp-server.o kvServer.o $(objects)

vpath %.cc net include tree kvstore
vpath %exp-client.cc test/kvstore/exp-client
vpath %exp-server.cc test/kvstore/exp-server

all : exp-cli exp-srv
.PHONY : all

exp-cli : $(objects1)
        g++ $(objects1) -o exp-cli -lpthread

exp-srv : $(objects2)
        g++ $(objects2) -o exp-srv -lpthread

.PHONY : clean
clean :
        -rm exp-cli exp-srv exp-client.o kvClient.o exp-server.o kvServer.o $(objects)
