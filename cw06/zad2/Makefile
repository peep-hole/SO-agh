CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) client.c utilities.c -o client -lrt
	$(CC) server.c utilities.c -o server -lrt

clean:
	rm -f client server