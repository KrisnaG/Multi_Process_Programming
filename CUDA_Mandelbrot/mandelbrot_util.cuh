/**
 * @file mandelbrot_util.cuh
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief CUDA mandelbrot utility library for mandrebrot
 * 
 * Contains variable macros, error handling and mandelbrot utility functions.
 */

#ifndef MANDELBROT_UTIL_CUH
#define MANDELBROT_UTIL_CUH

#include <stdio.h>
#include "bmpfile.h"
#include <cuda_runtime.h>

/* Mandelbrot values */
#define RESOLUTION 8700.0
#define XCENTER -0.55
#define YCENTER 0.6
#define MAX_ITER 1000

/* Colour Values */
#define COLOUR_DEPTH 255
#define COLOUR_MAX 240.0
#define GRADIENT_COLOUR_MAX 230.0

/* Success and Failure */
#define SUCCESS 0
#define FAILURE -1

/* Max input argument length */
#define ARG_LENGTH 3

/* max number of threads */
#define THREADS 256

/* File name for BMP image */
#define FILENAME "my_mandelbrot_fractal.bmp"

/* Prints out error message passed and exits */
#define handle_error(msg) \
        do { fprintf(stderr,"%s\n", msg); exit(EXIT_FAILURE); } while (0)

/* Prints out error message passed and a CUDA error then exits */
#define handle_cuda_error(msg, error) \
        do { fprintf(stderr,"%s - Error: %s\n", msg, error); exit(EXIT_FAILURE); } while (0)

/* Free dynamic memory */
#define free_mem_on_error(h_pixels, d_pixels) \
        do { free(h_pixels); cudaFree(d_pixels); } while (0)

/* Utility functions */

int parse_args(int argc, char *argv[], int *width, int *height);
__device__ void GroundColorMix(double* color, double x, double min, double max);
__global__ void mandelbrot(int xoffset, int yoffset, rgb_pixel_t *pixel, long numElements, int width);

#endif