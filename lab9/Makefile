CC=gcc
CFLAGS=-c -Wall -g

all: zad1
.PHONY: all

zad1: zad1.o
	$(CC) zad1.o -lpthread -o zad1

zad1.o: zad1.c
	$(CC) $(CFLAGS) zad1.c

clean:
	$(RM) *o zad1
.PHONY: clean
