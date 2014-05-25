all: primes.c
	gcc primes.c -o bin/primes -D_REENTRANT -lpthread -Wall -lm
