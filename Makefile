CC= g++
CFLAGS= -Wall -pedantic -g
RM= rm -rf

all: sim

sim: Set.o Block.o Cache.o  main.cpp
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o 
	$(RM) sim
