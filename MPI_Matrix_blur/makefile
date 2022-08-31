# makefile for matrix weighted sum
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = mpicc
CFLAGS = -Wall -pedantic -lm
CEXES = weighted_sum getMatrix mkRandomMatrix 
COBJS = matrix.o
PROGRAM = weighted_sum
NPROC = 3
ARGS = 4 5 input_file output_file

all: clean ${CEXES} run

weighted_sum: weighted_sum.c ${COBJS}
	${COMPILER} ${CFLAGS} weighted_sum.c ${COBJS} -o weighted_sum

getMatrix:   getMatrix.c ${COBJS}
	gcc ${CFLAGS} getMatrix.c ${COBJS} -o getMatrix

mkRandomMatrix:    mkRandomMatrix.c  ${COBJS}
	gcc ${CFLAGS} mkRandomMatrix.c ${COBJS} -o mkRandomMatrix

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c

build: ${CEXES}

clean: 
	rm -f *.o *~ ${CEXES}

run: 
	mpirun -np ${NPROC} ${PROGRAM} ${ARGS}