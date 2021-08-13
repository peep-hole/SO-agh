CC = gcc -Wall -std=c11 -D_DEFAULT_SOURCE -ggdb

all: comp test clean

comp:
	make clean
	$(CC) main.c -o main

test:
	./main . aba 3
	./main . aba 4
	./main . kjadbfji 4
	
clean:
	rm -f main