# makefile for parallel_search_keysapce
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = gcc
CFLAGS = -Wall -pedantic -g
COBJS = parallel_search_util.o
CEXES = parallel_search_keyspace

all: ${CEXES}

parallel_search_keyspace: parallel_search_keyspace.c ${COBJS}
	${COMPILER} ${CFLAGS} parallel_search_keyspace.c ${COBJS} -lcrypto  -o parallel_search_keyspace

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c

clean:
	rm -f *.o *~ ${CEXES}