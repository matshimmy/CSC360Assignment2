all: MFS

MFS: main.o sortQueue.o
	gcc main.o sortQueue.o -o MFS -lpthread

main.o: main.c
	gcc -c main.c 

sortQueue.o: sortQueue.c
	gcc -c sortQueue.c

clean:
	rm -f *.o MFS
