objects = webServer.o channel.o epoller.o socket.o buffer.o log.o eventLoop.o connection.o \
        server.o sig.o

VPATH = ../../../net : ../../../include

webServer : $(objects)
        g++ $(objects) -o webServer -lpthread

.PHONY : clean
clean :
        -rm webServer $(objects)
