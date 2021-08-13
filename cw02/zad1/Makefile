CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	python3 py_gen.py
	./main sys g0.txt g1.txt
	./main lib g0.txt g1.txt

clean:
	rm -f *.txt
	rm -f main