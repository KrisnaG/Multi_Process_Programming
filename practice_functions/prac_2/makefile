COMPILER = gcc
CFLAGS = -Wall -pedantic

EXES = prints_args process_chain_args process_chain_exec

all: ${EXES}

prints_args:  prints_args.c 
	${COMPILER} ${CFLAGS}  prints_args.c -o prints_args

process_chain_args:  process_chain_args.c 
	${COMPILER} ${CFLAGS}  process_chain_args.c -o process_chain_args

process_chain_exec:  process_chain_exec.c 
	${COMPILER} ${CFLAGS}  process_chain_exec.c -o process_chain_exec

clean: 
	rm -f *~ *.o ${EXES}