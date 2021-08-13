CC = gcc -Wall -std=c11 -ggdb -pthread

all:
	make clean
	make server
	make client
	./server 6666 socket

client:
	$(CC) -o client client.c

server:
	$(CC) -o server server.c


client1:
	./client cl1 local socket

client2:
	./client cl2 local socket

client_net_1:
	./client cl3 xxx 6666

client_net_2:
	./client cl4 xxx 6666

clean:
	rm -f client server socket