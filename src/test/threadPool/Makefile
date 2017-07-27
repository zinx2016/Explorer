objects = test.o

VPATH = ../../net : ../../include

test : $(objects)
        g++ -g $(objects) -o test -lpthread

.PHONY : clean
clean :
        -rm test $(objects)
