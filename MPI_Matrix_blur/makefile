# makefile for matrix convolution
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = mpicc
CFLAGS = -Wall -pedantic
CEXES = convolution getMatrix mkRandomMatrix
#COBJS = 
PROGRAM = convolution

all: clean ${CEXES} run

convolution: convolution.c
	${COMPILER} ${CFLAGS} convolution.c -o convolution

getMatrix:   getMatrix.c matrix.o
	gcc ${CFLAGS} getMatrix.c matrix.o -o getMatrix

mkRandomMatrix:    mkRandomMatrix.c  matrix.o
	gcc ${CFLAGS} mkRandomMatrix.c matrix.o -o mkRandomMatrix

matrix.o: matrix.c
	${COMPILER} ${CFLAGS} -c matrix.c
	
%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

build: 
	${CEXES}

clean: 
	rm -f *.o *~ ${CEXES}

run: 
	mpirun -np 2 ${PROGRAM} 1 3 input_file output_file