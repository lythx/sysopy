CC=gcc
CFLAGS=-c -Wall -g

all: zad1 zad2
.PHONY: all

zad1: zad1.o
	$(CC) zad1.o -o zad1

zad1.o: zad1.c
	$(CC) $(CFLAGS) zad1.c

zad2: zad2.o
	$(CC) zad2.o -o zad2

zad2.o: zad2.c
	$(CC) $(CFLAGS) zad2.c

clean:
	$(RM) *o zad1 zad2 
.PHONY: clean
