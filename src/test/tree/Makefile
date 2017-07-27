objects = tree.o bPlusTree.o

VPATH = ../../net : ../../include : ../../tree

tree : $(objects)
        g++ $(objects) -o tree -lpthread

.PHONY : clean
clean :
        -rm tree $(objects)
