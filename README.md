# Multi_Process_and_Threaded_Programming

Uutilising multi-process and multi-threaded programming to complete a task/problem.

These programs will use system calls which run on Linux systems.

### [Practice Functions](https://github.com/KrisnaG/Multi_Process_and_Threaded_Programming/tree/main/practice_functions)
* print_n_numbers(n)
* sum_int_array(array)

### [Parallel Search](https://github.com/KrisnaG/Multi_Process_and_Threaded_Programming/tree/main/parallel_search)
Simple program that demonstrates the use of the openSSL library functions to brute-force search for an AES encryption key given a partial key. <br>
The brute-force search is conducted with parallel processes arranged in a "ring-of-processes" topology.

### [Nordvik_Problem](https://github.com/KrisnaG/Multi_Process_and_Threaded_Programming/tree/main/Nordvik_Problem)
The road to Nordvik carries traffic (cars and trucks) in both directions, except at a 1 lane suspension bridge. <br> 
The bridge is only wide enough to carry traffic in one direction at a time. Only one truck is allowed on the bridge at anytime. <br>
This program uses threads to act as vehicles (cars and trucks), that crosses the Nordvik bridge which acts as a shared resource.

### [MPI Matrix weighted sum (blur)](https://github.com/KrisnaG/Multi_Process_and_Threaded_Programming/tree/main/MPI_Matrix_blur)
A C program using MPI that can process a square data matrix using a convolution filter that replaces each element with the weighted sum of its neighbours. Each element's contribution to the sum should be weighted by 1/n_depth, where the n_depth is the neighbourhood depth of the specific element.

### [CUDA Mandelbrot](https://github.com/KrisnaG/Multi_Process_and_Threaded_Programming/tree/main/CUDA_Mandelbrot)
A parallel implementation of the Mandelbrot algorithm using Nvidia CUDA C that produces bitmap images which contain the fractal representation of the Mandelbrot set. 
