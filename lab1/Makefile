CC=gcc
CFLAGS=-c -Wall -g

all: countdown

countdown: countdown.o
	$(CC) countdown.o -o countdown

countdown.o: countdown.c
	$(CC) $(CFLAGS) countdown.c

clean: 
	$(RM) *o countdown
.PHONY: clean
