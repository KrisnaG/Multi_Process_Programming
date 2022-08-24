# makefile for matrix convolution
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = mpicc
CFLAGS = -Wall -pedantic
CEXES = convolution
#COBJS = 
PROGRAM = convolution 

all: clean ${CEXES} run

convolution: convolution.c
	${COMPILER} ${CFLAGS} convolution.c -o convolution

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c 

build: 
	${CEXES}

clean: 
	rm -f *.o *~ ${CEXES}

run: 
	./${PROGRAM}