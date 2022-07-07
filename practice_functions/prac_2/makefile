COMPILER = gcc
CFLAGS = -Wall -pedantic

COBJS = makeargv.o

EXES = prints_args process_chain_args process_chain_exec

all: tokenAt ${EXES}

prints_args:  prints_args.c 
	${COMPILER} ${CFLAGS}  prints_args.c -o prints_args

process_chain_args:  process_chain_args.c 
	${COMPILER} ${CFLAGS}  process_chain_args.c -o process_chain_args

process_chain_exec:  process_chain_exec.c 
	${COMPILER} ${CFLAGS}  process_chain_exec.c -o process_chain_exec

tokenAt: tokenAt.c ${COBJS}
	${COMPILER} ${CFLAGS} tokenAt.c ${COBJS}  -o tokenAt

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

clean: 
	rm -f *~ *.o ${EXES} ${COBJS} tokenAt