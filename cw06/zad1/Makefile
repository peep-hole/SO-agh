CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) client.c utilities.c -o client
	$(CC) server.c utilities.c -o server

clean:
	rm -f client server