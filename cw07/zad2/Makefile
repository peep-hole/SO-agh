CC = gcc -Wall -std=c11 -ggdb

all: comp

comp:
	make clean
	$(CC) main.c pizza.c -o main -lpthread -lrt
	$(CC) courier.c pizza.c -o courier -lpthread -lrt
	$(CC) cook.c pizza.c -o cook -lpthread -lrt

clean:
	rm -f main courier cook