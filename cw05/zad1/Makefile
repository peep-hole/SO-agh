CC = gcc -Wall -std=c11 -ggdb

all: comp test

comp:
	make clean
	$(CC) main.c -o main

test:
	./main manuals.txt
clean:
	rm -f main