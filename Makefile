CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O0 -g
OBJDIR := obj

all: server client

server: $(OBJDIR)/server.o $(OBJDIR)/address_book.o $(OBJDIR)/auth.o
	$(CC) $(CFLAGS) -o server $(OBJDIR)/server.o $(OBJDIR)/address_book.o $(OBJDIR)/auth.o

client: $(OBJDIR)/client.o
	$(CC) $(CFLAGS) -o client $(OBJDIR)/client.o

$(OBJDIR)/server.o: src/server.c include/common.h include/address_book.h include/auth.h
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/server.o src/server.c

$(OBJDIR)/client.o: src/client.c include/common.h
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/client.o src/client.c

$(OBJDIR)/address_book.o: src/address_book.c include/address_book.h include/common.h
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/address_book.o src/address_book.c

$(OBJDIR)/auth.o: src/auth.c include/auth.h
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/auth.o src/auth.c

clean:
	rm -f $(OBJDIR)/*.o server client allowedClients.txt

.PHONY: all clean
