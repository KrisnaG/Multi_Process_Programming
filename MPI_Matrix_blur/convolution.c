/**
 * @file blur.c
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


#define SUCCESS 0
#define FAILURE -1
#define ARG_LENGTH 3
#define MAIN_PROCESS 0

/* Prints out error message passed and exits */
#define handle_error(msg) \
    do { fprintf(stderr,"%s\n", msg); MPI_Finalize(); exit(EXIT_FAILURE); } while (0)


int get_user_input(int rank, int argc,  char *argv[], char *input, char *output, int *depth)
{
    // main process
    if (rank == MAIN_PROCESS) {
        if (argc != ARG_LENGTH || (*depth = atoi(argv[2])) <= 0) {
            fprintf(stderr, 
                    "Usage: %s -np <number of processes> <depth> <input file> <output file>\n", 
                    argv[0]);
            return FAILURE;
        }
        input = argv[2];
        output = argv[3];
    }
    return SUCCESS;
}

int read_file(char *filename, char **buffer)
{
    FILE *file;



    return SUCCESS;
}

// ************** PLACE INTO HEADER ABOVE ************** //



int main(int argc, char** argv) 
{
    int nproc;                  /* Number of processes */
    int me;                     /* Process rank ID */
    int depth;                  /* Depth of weighted sum */
    char * input_file;          /* Input file to read from */
    char * output_file;         /* Output file to write to */

    // setup MPI
    if (MPI_Init(&argc, &argv) == -1)
        handle_error("Unable to intialise MPI");

    if (MPI_Comm_rank(MPI_COMM_WORLD, &me) == -1)
        handle_error("Unalbe to determine the rank of the calling processor with the communicator");

    if (MPI_Comm_size(MPI_COMM_WORLD, &nproc) == -1)
        handle_error("Unalbe to determine the processors associated with communicator");

    // TODO: validate user input
    if (get_user_input(me, argc, argv, &input_file, &output_file, &depth) == -1)
        handle_error("Invalid user input");

    // TODO: Extract data from file

    // TODO: conduct task


    // terminate MPI
    if (MPI_Finalize() == -1)
        handle_error("Unable to terminate MPI execution environment");

    exit(EXIT_SUCCESS);
}