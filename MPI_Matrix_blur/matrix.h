/**
 * @file matrix.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Matrix utility library for weighted sum.
 * Contains variable macros, error handling, calculation 
 * functions and matrix utility functions.
 * 
 * Functions get_slot, set_slot, get_row, set_row have been
 * written by ian a. mason @ une  march 15  '99 
 * touched up august '02 
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>


#define SUCCESS 0           /* Successful operation */
#define FAILURE -1          /* Failed operation */
#define ARG_LENGTH 5        /* Maximum number of input arguments */
#define MAIN_PROCESS 0      /* Master process */

/* Prints out error message passed and exits */
#define handle_error(msg) \
        do { fprintf(stderr,"%s\n", msg); MPI_Abort(MPI_COMM_WORLD, FAILURE); \
        MPI_Finalize(); exit(EXIT_FAILURE); } while (0)

/* Utility functions */

int max(int a, int b);
int parse_args(int, char *[], int *, int *, int *);
void calculate_weighted_sum(int depth, int partition, int matrix_size, 
    float [][matrix_size], int [][(depth * 2) + matrix_size]);
void create_padded_matrix(int padded_size, int matrix_size, int depth,
    int paddedA[][padded_size], int A[][matrix_size + 1]);
void calculate_displacement(int nproc, int initial_partition, int size, int overflow,
    int *group_size, int *displacement, int *overflow_group_size);


/* basic matrix library for  comp309                            */
/* written by ian a. mason @ une  march 15  '99                 */            
/* touched up august '02                                        */ 

int get_slot(int fd, int matrix_size, int row, int col, int *slot);
int set_slot(int fd, int matrix_size, int row, int col, int value);
int get_row(int fd, int matrix_size, int row, int matrix_row[]);
int set_row(int fd, int matrix_size, int row, int matrix_row[]);
    
#endif