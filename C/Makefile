CC = gcc
CFLAGS = -g -Wall -std=gnu11

all: clear freq_vu prog_test solid_white

fast: CFLAGS += -O3 -march=native -D NDEBUG
fast: all

clear: clear.c light_bar.o
	$(CC) $(CFLAGS) -o clear clear.c light_bar.o

freq_vu: freq_vu.c light_bar.o
	$(CC) $(CFLAGS) -o freq_vu freq_vu.c light_bar.o -lfftw3 -lm

prog_test: prog_test.c light_bar.o
	$(CC) $(CFLAGS) -o prog_test prog_test.c light_bar.o

solid_white: solid_white.c light_bar.o
	$(CC) $(CFLAGS) -o solid_white solid_white.c light_bar.o

light_bar.o: light_bar.c light_bar.h
	$(CC) $(CFLAGS) -c light_bar.c

clean:
	$(RM) *.o clear freq_vu prog_test solid_white
