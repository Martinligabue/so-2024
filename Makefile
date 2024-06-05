CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O0 -g
OBJDIR := obj

all: server client

server: server.o address_book.o auth.o
	$(CC) $(CFLAGS) -o server server.o address_book.o auth.o

client: client.o
	$(CC) $(CFLAGS) -o client client.o

server.o: src/server.c include/common.h include/address_book.h include/auth.h
client.o: src/client.c include/common.h
address_book.o: src/address_book.c include/address_book.h include/common.h
auth.o: src/auth.c include/auth.h

clean:
	rm -f *.o server client allowedClients.txt

.PHONY: all clean
