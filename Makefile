CC = gcc
CFLAGS = -Wall -Wextra -O2

SOURCES = ./src/tsc.c
OBJECTS = tsc.o
HEADERS = ./lib/tsc.h

MAIN = main
MAIN_SRC = main.c

.PHONY: all clean help

all: $(MAIN)

$(OBJECTS): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -c $(SOURCES) -o $(OBJECTS)

$(MAIN): $(MAIN_SRC) $(OBJECTS) $(HEADERS)
	$(CC) $(CFLAGS) $(MAIN_SRC) $(OBJECTS) -o $(MAIN)

clean:
	rm -f $(OBJECTS) $(MAIN)


