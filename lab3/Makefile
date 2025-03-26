CC=gcc
CFLAGS=-c -Wall -g

all: flipper
.PHONY: all

flipper: flipper.o
	$(CC) flipper.o -o flipper

flipper.o: flipper.c
	$(CC) $(CFLAGS) flipper.c

clean:
	$(RM) *o flipper
.PHONY: clean
