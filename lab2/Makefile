CC=gcc
CFLAGS=-c -Wall

all: static shared dynamic
.PHONY: all

static: libcollatz.a
	$(CC) client.c libcollatz.a -o prog_static

shared: libcollatz.so.1
	ln -s libcollatz.so.1 libcollatz.so
	$(CC) client.c -lcollatz -L ./ -Wl,-rpath,. -o prog_shared 

dynamic: libcollatz.so.1
	$(CC) $(CFLAGS) -DDYNAMIC client.c
	$(CC) client.o -o prog_dynamic

libcollatz.a:
	$(CC) $(CFLAGS) collatz.c
	ar rcs libcollatz.a collatz.o

libcollatz.so.1:
	$(CC) $(CFLAGS) -fPIC -c collatz.c;
	$(CC) -shared -Wl,-soname,libcollatz.so.1 -o libcollatz.so.1 collatz.o -lc

clean:
	$(RM) *o lib* prog*
.PHONY: clean
