CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c -o main

test:
	./main sys ../zad2/file.txt result_sys.txt
	./main lib ../zad2/file.txt result_lib.txt


clean:
	rm -f main
	rm -f pomiar_zad_5.txt result_sys.txt result_lib.txt
