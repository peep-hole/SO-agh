CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	./main sys z file.txt
	./main lib z file.txt

clean:
	rm -f main
	rm -f pomiar_zad_2.txt