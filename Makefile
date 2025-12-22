CC = gcc
CFLAGS = -Wall -Wextra
PREFIX = /usr/local

all: note

note: note.c
	$(CC) $(CFLAGS) note.c -o note

install: note
	mkdir -p $(PREFIX)/bin
	cp note $(PREFIX)/bin/note

uninstall:
	rm -f $(PREFIX)/bin/note

clean:
	rm -f note

