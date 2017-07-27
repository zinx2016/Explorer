objects = webClient.o channel.o epoller.o socket.o buffer.o log.o eventLoop.o connection.o \
        server.o sig.o

VPATH = ../../../net : ../../../include

webClient : $(objects)
        g++ $(objects) -o webClient -lpthread

.PHONY : clean
clean :
        -rm webClient $(objects)
