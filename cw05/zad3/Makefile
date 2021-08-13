CC = gcc -Wall -std=c11 -ggdb

all: comp test

comp:
	make clean
	$(CC) main.c -o main
	$(CC) producer.c -o producer
	$(CC) customer.c -o customer

test:
	./main

clean:
	rm -f main pipe producer customer