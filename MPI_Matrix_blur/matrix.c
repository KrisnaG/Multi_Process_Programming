/**
 * @file matrix.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Matrix utility library. Implementation on matrix.h.
 * 
 * Functions get_slot, set_slot, get_row, set_row have been
 * written by ian a. mason @ une  march 15  '99 
 * touched up august '02 
 */


#include "matrix.h"
#include "mpi.h"

/**
 * @brief Returns that maximum integer between two integers.
 * 
 * @param a first int to compare
 * @param b second int to compare
 * @return int maximum int
 */
int max(int a, int b)
{
    return (a > b) ? a : b;
}

/**
 * @brief Checks input parameters are the correct size and file
 * descriptors can be opened.
 * 
 * @param argc number of input arguments
 * @param argv list of input arguments
 * @param depth depth of weighted sum
 * @param size size of matrix
 * @param fd file descriptor for input and output files
 * @return int 0 is successful, -1 on failure
 */
int parse_args(int argc, char *argv[], int *depth, int *size, int *fd)
{
    // check inputs and files
    if (argc != ARG_LENGTH || 
       (*depth = atoi(argv[1])) <= 0 ||
       (*size = atoi(argv[2])) <= 0 ||
       (fd[0] = open(argv[3], O_RDONLY)) <= 0 ||
       (fd[1] = open(argv[4], O_WRONLY)) <= 0 ) {
        return FAILURE;
    }
    return SUCCESS;
}

/**
 * @brief Calculates the weighted sum for each element up to the given
 * depth and stores it in a different given matrix.
 * 
 * @param depth depth of weighted sum
 * @param partition number of rows to conduct calculations
 * @param matrix_size size of matrix
 * @param end_matrix matrix for results
 * @param start_matrix matrix of starting values
 */
void calculate_weighted_sum(int depth, int partition, int matrix_size, 
    float end_matrix[][matrix_size], int start_matrix[][(depth * 2) + matrix_size])
{
    double distance;                /* Distance between two cells */
    int row, col, i, j;             /* Loop counters */

    // for each element
    for (row = depth; row < partition + depth; row++) {
        for (col = depth;  col < matrix_size + depth; col++) {
            // first set element to zero
            end_matrix[row - depth][col - depth] = 0;
            
            // calculate weighted sum of surrounding values
            for (i = row - depth; i < row + depth + 1; i++) {
                for (j = col - depth; j < col + depth + 1; j++) {
                    distance = max(abs(i - row), abs(j - col));
                    if (distance != 0)
                        end_matrix[row - depth][col - depth] += start_matrix[i][j] / distance;
                }
            }
        }
    }
}

/**
 * @brief Create a padded matrix with a width of the given depth.
 * 
 * @param padded_size size of padded matrix
 * @param matrix_size original matrix size
 * @param depth depth of padded matrix
 * @param padded_matrix output padded matrix
 * @param matrix input matrix
 */
void create_padded_matrix(int padded_size, int matrix_size, int depth, int padded_matrix[][padded_size], int matrix[][matrix_size + 1])
{
    int row, col;               /* Loop counters */

    // initialise supersized matrix wit all zeros
    for (row = 0; row < padded_size; row++)
        for (col = 0; col < padded_size; col++)
            padded_matrix[row][col] = 0;
    
    // fill in supersize with input matrix
    for (row = 1; row < matrix_size + 1; row++)
        for (col = 1; col < matrix_size + 1; col++)
            padded_matrix[row + depth - 1][col + depth - 1] = matrix[row][col];
}

/**
 * @brief Calculates the offset for each process based the partition size and
 * overflow. 
 * 
 * @param nproc number of processes
 * @param initial_partition initial size of partitions without overflow
 * @param size size of matrix
 * @param overflow size of overflow rows
 * @param group_size Size of send/recv items of each process 
 * @param displacement Offset for each process
 * @param overflow_group_size Size of send/recv overflow items of each process
 */
void calculate_displacement(int nproc, int initial_partition, int size, int overflow,
    int *group_size, int *displacement, int *overflow_group_size)
{
    int i, j;

    // calculate each process displacement from main process
    for (i = 0; i < nproc; i++) {
        group_size[i] = size;
        displacement[i] = size * initial_partition * i;
        if (overflow_group_size)
            overflow_group_size[i] = (i < overflow) ? size : 1;
    }

    // added displacement for any overflow
    for (i = 0; i < overflow; i++)
        for (j = i + 1; j < nproc; j++)
            displacement[j] += size;
}


/* BELOW:                                                       */
/* basic matrix library for  comp309                            */
/* written by ian a. mason @ une  march 15  '99                 */            
/* touched up august '02                                        */            
/* rows & columns are numbers 1 through dimension               */

/**
 * @brief Get the slot object from file
 * 
 * @param fd file descriptor of input file
 * @param matrix_size size of matrix
 * @param row row of element
 * @param col column of element
 * @param slot input matrix element location
 * @return int 0 if successful, -1 if failed  
 */
int get_slot(int fd, int matrix_size, int row, int col, int *slot)
{
    off_t offset = (((row - 1)*matrix_size) + (col - 1))*sizeof(int);
    
    if (offset < 0) {
        fprintf(stderr,"offset overflow");
        return -1;
    } else if ((row > matrix_size) || (col > matrix_size)) {
        fprintf(stderr,"indexes out of range");
        return -1; 
    } else if (lseek(fd, offset, 0) < 0) {
        perror("lseek failed");
        return -1; 
    } else if (read(fd, slot, sizeof(int)) < 0) {
        perror("read failed");
        return -1; 
    };
    
    return 0;
}

/**
 * @brief Set the slot object to file
 * 
 * @param fd file descriptor of output file
 * @param matrix_size size of matrix
 * @param row row of element
 * @param col column of element
 * @param value value to set it to
 * @return int 0 if successful, -1 if failed  
 */
int set_slot(int fd, int matrix_size, int row, int col, int value)
{
    off_t offset = (((row - 1)*matrix_size) + (col - 1))*sizeof(int);
    
    if (offset < 0) {
        fprintf(stderr,"offset overflow");
        return -1; 
    } else if ((row > matrix_size) || (col > matrix_size)) {
        fprintf(stderr,"indexes out of range"); 
    } else if (lseek(fd, offset, 0) < 0) {
        perror("lseek failed");
        return -1; 
    } else if (write(fd, &value, sizeof(int)) < 0) {
        perror("write failed");
        return -1;
    };
    
    return 0;
}

/**
 * @brief Get the row object from file
 * 
 * @param fd file descriptor of input file
 * @param matrix_size size of matrix
 * @param row row of matrix to get from
 * @param matrix_row matrix to input to
 * @return int 0 if successful, -1 if failed 
 */
int get_row(int fd, int matrix_size, int row, int matrix_row[])
{
    int column;
    off_t offset =  ((row - 1) * matrix_size)*sizeof(int);
    
    if (offset < 0) {
        fprintf(stderr,"offset overflow");
        return -1; 
    } else if (row > matrix_size) {
        fprintf(stderr,"index out of range");
        return -1;
    } else if (lseek(fd, offset, 0) < 0) {
        perror("lseek failed");
        return -1; 
    } else 
        for (column = 0; column < matrix_size; column++)
            if(read(fd, &matrix_row[column], sizeof(int)) < 0) {
                fprintf(stderr,"read failed column = %d\n",column);
                perror("read failed");
                return -1; 
            };

    return 0;
}

/**
 * @brief Set the row object to file
 * 
 * @param fd file descriptor of output file
 * @param matrix_size size of matrix
 * @param row row of matrix to output
 * @param matrix_row matrix to output
 * @return int 0 if successful, -1 if failed 
 */
int set_row(int fd, int matrix_size, int row, int matrix_row[]) {
    int column;
    off_t offset = ((row - 1) * matrix_size)*sizeof(int);
    
    if (offset < 0) {
        fprintf(stderr,"offset overflow");
        return -1;
    } else if (row > matrix_size) {
        fprintf(stderr,"indexes out of range");
    } else if (lseek(fd, offset, 0) < 0) {
        perror("lseek failed");
        return -1;
    } else 
        for (column = 0; column < matrix_size; column++)
            if (write(fd, &matrix_row[column], sizeof(int)) < 0) {
                perror("write failed");
                return -1; 
            };

    return 0;
}

