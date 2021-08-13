CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	./main 300
	./main 10000
	
clean:
	rm -f main