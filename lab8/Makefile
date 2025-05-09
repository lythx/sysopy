CC=gcc
CFLAGS=-c -Wall -g

all: server client printer
.PHONY: all

server: server.o
	$(CC) server.o -o server

server.o: server.c common.h
	$(CC) $(CFLAGS) server.c

client: client.o
	$(CC) client.o -o client

client.o: client.c common.h
	$(CC) $(CFLAGS) client.c

printer: printer.o
	$(CC) printer.o -o printer

printer.o: printer.c common.h
	$(CC) $(CFLAGS) printer.c

clean:
	$(RM) *o server client printer
.PHONY: clean
