CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender

clean:
	rm -f catcher sender
