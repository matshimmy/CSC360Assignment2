all: MFS

MFS: main.o
	gcc main.o -o MFS -lpthread

main.o: main.c
	gcc -c main.c 

clean:
	rm -f *.o MFS
