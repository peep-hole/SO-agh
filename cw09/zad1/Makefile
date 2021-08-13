CC = gcc #-Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) -o main main.c -lpthread -lm

test:
	./main

clean:
	rm -f main