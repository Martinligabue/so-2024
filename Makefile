CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O0 -g

OBJDIR := obj
BINDIR := bin
LOGDIR := log
INCDIR := inc

SRC_SERVER := src/server.c
SRC_CLIENT := src/client.c
SRC_ADDRESS_BOOK := src/address_book.c
SRC_AUTH := src/auth.c

HEADERS := $(INCDIR)/common.h $(INCDIR)/address_book.h $(INCDIR)/auth.h

OBJ_SERVER := $(OBJDIR)/server.o
OBJ_CLIENT := $(OBJDIR)/client.o
OBJ_ADDRESS_BOOK := $(OBJDIR)/address_book.o
OBJ_AUTH := $(OBJDIR)/auth.o

all: create_dirs $(BINDIR)/server $(BINDIR)/client

create_dirs:
	@mkdir -p $(OBJDIR) $(BINDIR) $(LOGDIR)

$(BINDIR)/server: $(OBJ_SERVER) $(OBJ_ADDRESS_BOOK) $(OBJ_AUTH)
	$(CC) $(CFLAGS) -o $@ $^

$(BINDIR)/client: $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/server $(BINDIR)/client
	rm -f $(LOGDIR)/*

run:
	$(BINDIR)/server

.PHONY: all create_dirs clean run
