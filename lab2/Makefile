CC=gcc
CFLAGS=-c -Wall -g

static: countdown
.PHONY: all

static: countdown.o
	$(CC) countdown.o -o countdown

collatz.o: collatz.c
	$(CC) $(CFLAGS) collatz.c

client.o: client.c
	$(CC) $(CFLAGS) client.c

clean:
	$(RM) *o client collatz
.PHONY: clean

client: 
