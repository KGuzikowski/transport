CC=gcc
CFLAGS=-std=gnu99 -Wextra -Wall

transport: main.o send.o receive.o
	$(CC) $(CFLAGS) -o transport $^

clean:
	rm -rf *.o

distclean:
	rm -rf *.o transport
