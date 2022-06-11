CC = gcc
CFLAGS = -Wall -g

all: sim

clean:
	rm -f *.o sim

sim: sim.c
	$(CC) $(CFLAGS) sim.c -o sim -lm

