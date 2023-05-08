all: prodcons.o philosophers.o
	gcc -o prodcons prodcons.o -pthread
	gcc -o philosophers philosophers.o -pthread

prodcons.o: prodcons.c
	gcc -c -o prodcons.o prodcons.c

philosophers.o: philosophers.c
	gcc -c -o philosophers.o philosophers.c

clean:
	rm -f *.o prodcons philosophers