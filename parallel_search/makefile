# makefile for parallel_search_keysapce
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = gcc
CFLAGS = -Wall -pedantic -g
COBJS = parallel_search_util.o
CEXES = parallel_search_keyspace
PROGRAM = parallel_search_keyspace 
ARGS = 5 B1AF2507B69F11CCB3AE2C3592039 assignment_cipher.txt plain.txt

all: clean ${CEXES} run

parallel_search_keyspace: parallel_search_keyspace.c ${COBJS}
	${COMPILER} ${CFLAGS} parallel_search_keyspace.c ${COBJS} -lcrypto  -o parallel_search_keyspace

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c

clean:
	rm -f *.o *~ ${CEXES}

run:
	./${PROGRAM} ${ARGS}