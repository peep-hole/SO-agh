CC = gcc #-Wall -std=c11 -ggdb

all: comp test

comp:
	make clean
	$(CC) -o main main.c -lpthread -lm

test:
	./main 1 blocks mona_lisa.ascii.pgm new.ascii.pgm
	./main 1 numbers mona_lisa.ascii.pgm new2.ascii.pgm
	./main 2 blocks mona_lisa.ascii.pgm new.ascii.pgm
	./main 2 numbers mona_lisa.ascii.pgm new2.ascii.pgm
	./main 4 blocks mona_lisa.ascii.pgm new.ascii.pgm
	./main 4 numbers mona_lisa.ascii.pgm new2.ascii.pgm
	./main 8 blocks mona_lisa.ascii.pgm new.ascii.pgm
	./main 8 numbers mona_lisa.ascii.pgm new2.ascii.pgm
	./main 16 blocks mona_lisa.ascii.pgm new.ascii.pgm
	./main 16 numbers mona_lisa.ascii.pgm new2.ascii.pgm

clean:
	rm -f main