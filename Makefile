CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: csim

csim: ./src/cachesim.c 
	$(CC) $(CFLAGS) -o cachesim ./src/cachesim.c -lm 

#
# cleanup
#
clean:
	rm -rf *.o
	rm -rf *.tmp
	rm -f cachesim 
	rm -f trace.all trace.f*

