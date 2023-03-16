all: main.o
	gcc -o main main.o

main.o: main.c
	gcc -c -o main.o main.c

# Borra los archivos ejecutables y objeto
clean:
	rm -rf main main.o html latex

doc:
	doxygen