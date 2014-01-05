CC = gcc
CFLAGS = -g -Wall -std=gnu11

all: solid_white

solid_white: solid_white.c light_bar.o
	$(CC) $(CFLAGS) -o solid_white solid_white.c light_bar.o

light_bar.o: light_bar.c light_bar.h
	$(CC) $(CFLAGS) -c light_bar.c

clean:
	$(RM) *.o solid_white
