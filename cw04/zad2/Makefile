CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	./main info
	./main nostop
	./main nowait

clean:
	rm -f main
