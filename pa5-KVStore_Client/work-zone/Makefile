CC = gcc
CFLAGS = -Wall -pedantic -std=c99 -g

all: kvstore kvclient

kvstore: kvstore.c
	$(CC) $(CFLAGS) -o kvstore kvstore.c

kvclient: kvclient.c
	$(CC) $(CFLAGS) -o kvclient kvclient.c
