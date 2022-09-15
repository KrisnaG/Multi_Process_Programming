/**
 * @file mandelbrot_util.cu
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief CUDA mandelbrot utility implementation file.
 * 
 * Mandelbrot algorithm taken from mandelbrot.c single thread implmentation
 * from UNE moodle.
 */

#include "mandelbrot_util.cuh"

/**
 * @brief Checks input parameters are the correct, 
 * width and height.
 * 
 * @param argc number of input arguments
 * @param argv list of input arguments
 * @param width width of desired image
 * @param height height of desired image
 * @return int 0 is successful, -1 on failure
 */
int parse_args(int argc, char *argv[], int *width, int *height)
{
    if (argc != ARG_LENGTH || 
       (*width = atoi(argv[1])) <= 0 ||
       (*height = atoi(argv[2])) <= 0) {
        return FAILURE;
    }
    return SUCCESS;
}

/**
 * @brief Computes the color gradiant. Called by the device.
 * Check wiki for more details on the colour science: en.wikipedia.org/wiki/HSL_and_HSV.
 * 
 * @param color the output vector
 * @param x the gradiant (beetween 0 and 360)
 * @param min variation of the RGB channels (Move3D 0 -> 1)
 * @param max variation of the RGB channels (Move3D 0 -> 1)
 */
__device__ void GroundColorMix(double* color, double x, double min, double max)
{
  /*
   * Red = 0
   * Green = 1
   * Blue = 2
   */
    double posSlope = (max-min)/60;
    double negSlope = (min-max)/60;

    if( x < 60 )
    {
        color[0] = max;
        color[1] = posSlope*x+min;
        color[2] = min;
        return;
    }
    else if ( x < 120 )
    {
        color[0] = negSlope*x+2.0*max+min;
        color[1] = max;
        color[2] = min;
        return;
    }
    else if ( x < 180  )
    {
        color[0] = min;
        color[1] = max;
        color[2] = posSlope*x-2.0*max+min;
        return;
    }
    else if ( x < 240  )
    {
        color[0] = min;
        color[1] = negSlope*x+4.0*max+min;
        color[2] = max;
        return;
    }
    else if ( x < 300  )
    {
        color[0] = posSlope*x-4.0*max+min;
        color[1] = min;
        color[2] = max;
        return;
    }
    else
    {
        color[0] = max;
        color[1] = min;
        color[2] = negSlope*x+6*max;
        return;
    }
}

/**
 * @brief Mandelbrot kernel that calculates if a pixel (coordinate) is in
 * the mandelbrot set. 
 * 
 * @param xoffset X offset for pixel reference
 * @param yoffset Y offset for pixel reference
 * @param pixel Array of all pixels to calculate
 * @param numElements total number of pixel elements 
 * @param width width of bmp image
 */
__global__ void mandelbrot(int xoffset, int yoffset, rgb_pixel_t *pixel, long numElements, int width)
{
    long id = blockDim.x * blockIdx.x + threadIdx.x;    /* Thread ID - pixel coordinate */
    int row = id / width;                               /* Row that the pixel is in */
    int col = id % width;                               /* Column that the pixel is in */

    if (id < numElements) {
        // Determine where in the mandelbrot set, the pixel is referencing
        double x = XCENTER + (xoffset + col) / RESOLUTION;
        double y = YCENTER + (yoffset - row) / RESOLUTION;

        // Mandelbrot stuff
        double a = 0;
        double b = 0;
        double aold = 0;
        double bold = 0;
        double zmagsqr = 0;
        int iter = 0;
        double x_col;
        double color[3];
        pixel[id] = {0, 0, 0, 0};

        // Check if the x,y coord are part of the mendelbrot set - refer to the algorithm
        while(iter < MAX_ITER && zmagsqr <= 4.0) {
            ++iter;
            a = (aold * aold) - (bold * bold) + x;
            b = 2.0 * aold * bold + y;
            zmagsqr = (a * a) + (b * b);
            aold = a;
            bold = b;	
        }

        /* Generate the colour of the pixel from the iter value */
        /* You can mess around with the colour settings to use different gradients */
        /* Colour currently maps from royal blue to red */ 
        x_col =  (COLOUR_MAX - (( ((float) iter / ((float) MAX_ITER) * GRADIENT_COLOUR_MAX))));
        GroundColorMix(color, x_col, 1, COLOUR_DEPTH);
        pixel[id].red = color[0];
        pixel[id].green = color[1];
        pixel[id].blue = color[2];
    }
}