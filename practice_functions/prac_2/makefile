COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = prints_args process_chain_args

all: ${EXES}

example01:  prints_args.c 
	${COMPILER} ${CFLAGS}  prints_args.c -o prints_args
example02:  process_chain_args.c 
	${COMPILER} ${CFLAGS}  process_chain_args.c -o process_chain_args
clean: 
	rm -f *~ *.o ${EXES}