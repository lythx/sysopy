CC=gcc
CFLAGS=-c -Wall -g

all: zad1 catcher sender
.PHONY: all

zad1: zad1.o
	$(CC) zad1.o -o zad1

zad1.o: zad1.c
	$(CC) $(CFLAGS) zad1.c

catcher: catcher.o
	$(CC) catcher.o -o catcher

catcher.o: catcher.c
	$(CC) $(CFLAGS) catcher.c

sender: sender.o
	$(CC) sender.o -o sender

sender.o: sender.c
	$(CC) $(CFLAGS) sender.c

clean:
	$(RM) *o zad1 catcher 
.PHONY: clean
