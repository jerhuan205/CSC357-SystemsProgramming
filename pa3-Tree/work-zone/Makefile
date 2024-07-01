CC = gcc
CFLAGS = -Wall -g -std=c99 -pedantic

tree: tree.o
	$(CC) $(CFLAGS) -o tree tree.o

tree.o: tree.c
	$(CC) $(CFLAGS) -c tree.c

clean:
	rm -f *.o tree
