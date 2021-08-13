CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main
	$(CC) second.c -o second

test:
	./main ignore
	./main handler
	./main pending
	./main mask
	./main ignore e
	./main handler e
	./main pending e
	./main mask e

clean:
	rm -f main
	rm -f second