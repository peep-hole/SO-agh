CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c semaphore.c -o main
	$(CC) courier.c semaphore.c -o courier
	$(CC) cook.c semaphore.c -o cook

clean:
	rm -f main courier cook