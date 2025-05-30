CC=gcc
CFLAGS=-c -Wall -g

all: client server
.PHONY: all

client: client.o
	$(CC) client.o -lpthread -o client

client.o: client.c defs.h
	$(CC) $(CFLAGS) client.c

server: server.o
	$(CC) server.o -lpthread -o server

server.o: server.c defs.h
	$(CC) $(CFLAGS) server.c

clean:
	$(RM) *o client server
.PHONY: clean
