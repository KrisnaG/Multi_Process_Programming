/**
 * @file convolution.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 * 
 * Parameters:
 * 
 * Returns: 0 on Success
 * 
 * Library requirements:
 * 
 * Build (two methods):
 * 
 * Run:
 */


#include "mpi.h"

// ************** PLACE INTO HEADER BELOW ************** //

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define SUCCESS 0           /*  */
#define FAILURE -1          /*  */
#define ARG_LENGTH 5        /*  */
#define MAIN_PROCESS 0      /*  */

/* Prints out error message passed and exits */
void handle_error(char * msg) {
    fprintf(stderr,"%s\n", msg); 
    MPI_Finalize(); 
    exit(EXIT_FAILURE);
}


int parse_args(int rank, int argc, char *argv[], int *depth, int *size, int *fd)
{
    // main process
    if (rank == MAIN_PROCESS) {
        // check inputs and files
        if (argc != ARG_LENGTH || 
           (*depth = atoi(argv[1])) <= 0 ||
           (*size = atoi(argv[2])) <= 0 ||
           (fd[0] = open(argv[3], O_RDONLY)) <= 0 ||
           (fd[1] = open(argv[4], O_WRONLY)) <= 0 ) {
            return FAILURE;
        }
    }
    return SUCCESS;
}

// ************** PLACE INTO HEADER ABOVE ************** //



int main(int argc, char** argv) 
{
    int nproc;                  /* Number of processes */
    int me;                     /* Process rank ID */
    int depth;                  /* Depth of weighted sum */
    int matrix_size;            /*  */
    int fd[2];                  /*  */

    // setup MPI
    if (MPI_Init(&argc, &argv) < 0)
        handle_error("Unable to intialise MPI");

    // set process rank
    if (MPI_Comm_rank(MPI_COMM_WORLD, &me) < 0)
        handle_error("Unalbe to determine the rank of the calling processor with the communicator");

    // set number of processes
    if (MPI_Comm_size(MPI_COMM_WORLD, &nproc) < 0)
        handle_error("Unalbe to determine the processors associated with communicator");

    // get command line arguments
    if (parse_args(me, argc, argv, &depth, &matrix_size, fd) < 0)
        handle_error("Usage: mpirun -np <number of processes> ./convolution <depth> <matrix size> "
                     "<input filename> <output filename>");

    // broadcast matrix size and depth
    if ((MPI_Bcast (&matrix_size, 1, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD)) < 0 || 
        (MPI_Bcast (&depth, 1, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD)) < 0)
        handle_error("Unable to broadcast matrix size and depth to all processes");

    // row / nproc = process block to calculate

    // TODO: Get rows

    // TODO: conduct task


    // terminate MPI
    if (MPI_Finalize() == -1)
        handle_error("Unable to terminate MPI execution environment");

    exit(EXIT_SUCCESS);
}