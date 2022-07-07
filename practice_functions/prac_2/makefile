COMPILER = gcc
CFLAGS = -Wall
CEXES = prints_args process_chain_args

#all: {CEXES}

prints_args: prints_args.c
	${COMPILER} ${CFLAGS} prints_args.c -o prints_args

process_chain_args:	process_chain_args.c
	${COMPILER} ${CFLAGS} process_chain_args.c -o process_chain_args

clean:
	rm -f *.o ${CEXES} *~