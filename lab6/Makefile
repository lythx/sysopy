CC=gcc
CFLAGS=-c -Wall -g

all: zad1 zad2
.PHONY: all

zad1: zad1.o
	$(CC) zad1.o -o zad1

zad1.o: zad1.c
	$(CC) $(CFLAGS) zad1.c

zad2: zad2_1 zad2_2
.PHONY: zad2

zad2_1: zad2_1.o
	$(CC) zad2_1.o -o zad2_1

zad2_1.o: zad2_1.c
	$(CC) $(CFLAGS) zad2_1.c

zad2_2: zad2_2.o
	$(CC) zad2_2.o -o zad2_2

zad2_2.o: zad2_2.c
	$(CC) $(CFLAGS) zad2_2.c

clean:
	$(RM) *o zad1 zad2_1 zad2_2 fifo2
.PHONY: clean
