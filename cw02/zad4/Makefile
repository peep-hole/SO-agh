CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	./main sys ../zad2/file.txt result_sys.txt not ZAMIANA
	./main lib ../zad2/file.txt result_lib.txt not ZAMIANA

clean:
	rm -f main
	rm -f pomiar_zad_4.txt
	rm -f result_sys.txt result_lib.txt
