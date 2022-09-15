/**
 * @file mandelbrot.cu
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief A parallel implementation of the Mandelbrot algorithm using Nvidia 
 * CUDA C that produces bitmap images which contain the fractal representation 
 * of the Mandelbrot set.
 * 
 * Mandelbrot algorithm taken from mandelbrot.c single thread implmentation
 * from UNE moodle.
 * 
 * Parameters:
 *      1. Width of image
 *      2. Height of image
 * 
 * Returns: 
 *      0 on Success
 * 
 * Library requirements:
 *      1. mandelbrot_util.cuh
 *      2. mandelbrot_util.cu
 *      3. bmpfile.h
 *      4. bmpfile.c
 * 
 * Build:
 *      1. make build
 * 
 * Run:
 *      1. make
 *      2. make run
 *      3. mandelbrot <width> <height>
 */

#include <stdlib.h>
#include "mandelbrot_util.cuh"

/**
 * Host main routine
 */
int main(int argc, char *argv[])
{
    int width;          /* Width of bmp image */
    int height;         /* Height of bmp image */
    bmpfile_t *bmp;     /* BMP file */
    cudaError_t error;  /* Error code to check return values for CUDA calls */

    // check input arguments
    if (parse_args(argc, argv, &width, &height) < 0)
        handle_error("Usage: ./mandelbrot <width> <height>");

    long numElements = width * height;                      /* Total number of elements */
    long size = numElements * sizeof(rgb_pixel_t);          /* Size of total number of elements */
    int xoffset = -(width - 1) / 2;                         /* X offset for pixel reference */
    int yoffset = (height - 1) / 2;                         /* Y offset for pixel reference */
    bmp = bmp_create(width, height, 32);                    /* BMP object */
    rgb_pixel_t *h_pixels = (rgb_pixel_t *) malloc (size);  /* Host pixel memory */
    rgb_pixel_t *d_pixels = NULL;                           /* Device pixel memory*/
    
    // check host memory allocation
    if (h_pixels == NULL)
        handle_error("Failed to allocate host memory");

    // allocate device memory
    error = cudaMalloc((void **)&d_pixels, size);
    if (error != cudaSuccess) {
        free(h_pixels);
        handle_cuda_error("Failed to allocate host memory", cudaGetErrorString(error));
    }
    
    // copy from host to device
    error = cudaMemcpy(d_pixels, h_pixels, size, cudaMemcpyHostToDevice);
    if (error != cudaSuccess) {
        free_mem_on_error(h_pixels, d_pixels);
        handle_cuda_error("Failed to copy from host to device", cudaGetErrorString(error));
    }

    // set thread and block size
    int threadsPerBlock = THREADS;
    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;

    // CUDA kernel operation
    mandelbrot<<<blocksPerGrid, threadsPerBlock>>>(xoffset, yoffset, d_pixels, numElements, width);

    // check for any kernel errors
    error = cudaGetLastError();
    if (error != cudaSuccess) {
        free_mem_on_error(h_pixels, d_pixels);
        handle_cuda_error("Failed to launch kernel", cudaGetErrorString(error));
    }

    // copy from device to host
    error = cudaMemcpy(h_pixels, d_pixels, size, cudaMemcpyDeviceToHost);
    if (error != cudaSuccess) {
        free_mem_on_error(h_pixels, d_pixels);
        handle_cuda_error("Failed to copy from device to host", cudaGetErrorString(error));
    }

    // set bmp pixel to construct image
    for (int col = 0; col < width; col++)
        for (int row = 0; row < height; row++)
            if (bmp_set_pixel(bmp, col, row, h_pixels[row * width + col]) == 0) {
                free_mem_on_error(h_pixels, d_pixels);
                handle_error("Failed to set pixel");
            }

    // save bmp
    if (bmp_save(bmp, FILENAME) == 0) {
        free_mem_on_error(h_pixels, d_pixels);
        handle_error("Failed to save bmp to file");
    }

    printf("Mandelbrot image created and saved in %s\n", FILENAME);

    // free bmp
    bmp_destroy(bmp);

    // free host memory
    free(h_pixels);
    
    // free device memory
    error = cudaFree(d_pixels);
    if (error != cudaSuccess)
        handle_cuda_error("Failed free CUDA memory", cudaGetErrorString(error));

    // clean and flush  
    error = cudaDeviceReset();
    if (error != cudaSuccess)
        handle_cuda_error("Failed to deinitialize the device", cudaGetErrorString(error));

    exit(EXIT_SUCCESS);
}