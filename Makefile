all: dssh.o
	gcc -o dssh dssh.c

dssh.o: dssh.c
	gcc -c dssh.c dssh.h

run:
	./dssh

clean:
	rm ./dssh
	rm *.o

