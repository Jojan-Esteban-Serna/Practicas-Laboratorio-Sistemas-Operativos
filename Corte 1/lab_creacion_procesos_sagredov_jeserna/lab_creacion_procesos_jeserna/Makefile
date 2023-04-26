CC=gcc

all: split.o main.o
	$(CC) -o main main.o split.o


main.o: main.c
	$(CC) -c -o main.o main.c

split.o: split.c 
	$(CC) -c -o split.o split.c

doc:
	doxygen

clean:
	rm -rf main *.o docs



