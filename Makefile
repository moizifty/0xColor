VERSION = 0.0
PREFIX = /usr/local
LIBS = -lX11

CPPFLAGS = -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -Wextra -pedantic -std=c99 $(CPPFLAGS)

CC = gcc

SRC = 0xColor.c
OBJ = $(SRC:.c=.o)

all: 0xColor

.c.o:
	$(CC) -c $(CFLAGS) $<

0xColor: 0xColor.o
	$(CC) -o $@ 0xColor.o $(LIBS)

clean:
	rm -f 0xColor $(OBJ) 0xColor-$(VERSION).tar.gz

dist: clean
	mkdir -p 0xColor-$(VERSION)
	cp Makefile $(SRC)\
		0xColor-$(VERSION)
	tar -cf 0xColor-$(VERSION).tar 0xColor-$(VERSION)
	gzip 0xColor-$(VERSION).tar
	rm -rf 0xColor-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin
	cp -f 0xColor $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/0xColor

uninstall:
	rm -f $(PREFIX)/bin/0xColor

.PHONY: all clean dis install uninstall
