all: client.o server.o util.o
	gcc -o server server.o util.o -pthread
	gcc -o client client.o util.o -pthread

server.o: server.c
	gcc -c -o server.o server.c 

client.o: client.c
	gcc -c -o client.o client.c 

util.o: util.c
	gcc -c -o util.o util.c 
doc:
	doxygen

clean:
	rm -rf docs/ *.o server client