# makefile for mandelbrot on CUDA
# author: Krisna Gusti (kgusti@myune.edu.au)

COMPILER = nvcc
CFLAGS = -I /usr/local/cuda-11.4/samples/common/inc -g -G -lm
CEXES = mandelbrot
COBJS = mandelbrot_util.o bmpfile.o
PROGRAM = mandelbrot
ARGS = 1920 1080

all: clean ${CEXES} run

mandelbrot:   mandelbrot.cu ${COBJS}
	${COMPILER} ${CFLAGS} mandelbrot.cu ${COBJS} -o mandelbrot

%.o: %.cu %.cuh  makefile
	${COMPILER} ${CFLAGS} $< -c

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c

clean:
	rm -f *.o *~ *.bmp ${CEXES}

build: ${CEXES}

run: 
	${PROGRAM} ${ARGS}