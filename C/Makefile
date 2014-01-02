CC = gcc
CFLAGS = -g -Wall -std=gnu11

all: light_bar.o

light_bar.o: light_bar.c light_bar.h
	$(CC) $(CFLAGS) -c light_bar.c

clean:
	$(RM) *.o
