CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	python3 py_gen.py
	./main sys
	./main lib

clean:
	rm -f main
	rm -f pomiar_zad_3.txt
	rm -f a.txt b.txt c.txt dane.txt