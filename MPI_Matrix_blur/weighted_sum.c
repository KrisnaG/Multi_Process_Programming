/**
 * @file weighted_sum.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief An MPI program that can process a square data matrix using a convolution 
 * filter that replaces each element with the weighted sum of its neighbours. 
 * Each element's contribution to the sum is weighted by 1/n_depth, where the n_depth 
 * is the neighbourhood depth of the specific element.
 * 
 * Input matrix must be a square matrix of arbitrary size and the number of MPI
 * processes cannot exceed that matrix size. The neighbourhood depth can be any 
 * arbitrary size. Input and output matrix files must exist prior executing.
 * 
 * Note: There appears to be a bug in MPI_Scatterv. If send counts of one process
 * is zero, the program will hang. 
 * 
 * Parameters:
 *      MPI:
 *      1. Flag: -np 
 *      2. Number of processes
 *      Program:
 *      1. Depth of weighted sum
 *      2. Size of matrix
 *      3. Input matrix file
 *      4. Output matrix file
 * 
 * Returns: 
 *      0 on Success
 * 
 * Library requirements:
 *      1. matrix.h
 *      2. matric.c
 *      3. getMatrix.c (optional)
 *      4. mkRandomMatrix.c (optional)
 * 
 * Build:
 *      1. make build
 * 
 * Run:
 *      1. make
 *      2. make run
 *      3. mpirun -np <number of processes> ./weighted_sum <depth> <matrix size> 
 *                                           <input filename> <output filename>
 * 
 * Print matrix to standard output:
 *      ./getMatrix <filename> <dimensions>
 * 
 * Create a matrix with random values and output to file:
 *      ./ mkRandomMatrix <filename> <dimensions>
 */

#include "matrix.h"
#include "mpi.h"

int main(int argc, char *argv[])
{
    int nproc;                  /* Number of processes */
    int me;                     /* Process rank ID */
    int depth;                  /* Depth of neighbourhood weighted sum */
    int matrix_size;            /* Size of initial matrix */
    int fd[2];                  /* File descriptors to input and output file */
    int i, j;                   /* Loop counters */

    // initialise MPI
    if (MPI_Init(&argc, &argv) < 0)
        handle_error("Unable to intialise MPI");

    // get process ranks
    if (MPI_Comm_rank(MPI_COMM_WORLD, &me) < 0)
        handle_error("Unable to determine the rank of the calling processor with the communicator");

    // get number of processes
    if (MPI_Comm_size(MPI_COMM_WORLD, &nproc) < 0)
        handle_error("Unable to determine the processors associated with communicator");

    
    // main - process check input
    if (me == MAIN_PROCESS) {
        // get command line arguments
        if (parse_args(argc, argv, &depth, &matrix_size, fd) < 0)
            handle_error("Usage: mpirun -np <number of processes> ./weighted_sum <depth> <matrix size> "
                        "<input filename> <output filename>");

        // check number processes doesn't exceed matrix size
        if (nproc > matrix_size)
            handle_error("Number of processes cannot be greater than the matrix dimensions");
        
        // depth is redundant if above matrix size - 1
        if (depth > matrix_size - 1)
            depth = matrix_size - 1;
    }

    // broadcast matrix size, depth
    if ((MPI_Bcast(&matrix_size, 1, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD)) < 0 || 
        (MPI_Bcast(&depth, 1, MPI_INT, MAIN_PROCESS, MPI_COMM_WORLD)) < 0)
        handle_error("Unable to broadcast matrix size, depth and partition size to all processes");

    
    /* variables declared after input arguments have be verified and sent */

    int padded_size = (depth * 2) + matrix_size;            /* Size of padded matrix */
    int initial_partition = matrix_size / nproc;            /* Initial size of rows to process, excluding overflow */
    int partition = initial_partition;                      /* Size of number of rows each process has to process */
    int initial_req_rows = (depth * 2) + partition;         /* Initial number of rows required for calculation */
    int start_matrix[matrix_size + 1][matrix_size + 1];     /* Original matrix */
    float complete_matrix[matrix_size][matrix_size];        /* Completed calculated matrix */
    int padded_matrix[padded_size][padded_size];            /* Zero padded matrix */
    int overflow = matrix_size - (nproc * partition);       /* Number of remaining rows to distribute */
    int displacement[nproc];                                /* Offset for each process */
    int group_size[nproc];                                  /* Size of send/recv items of each process */
    int overflow_group_size[nproc];                         /* Size of send/recv overflow items of each process */
    
    // distribute any overflow to partitions
    if (me < overflow) partition++;
    
    int required_rows = (depth * 2) + partition;            /* Rows required to calculate a partitioned amount of row */
    int matrix_partition = partition * matrix_size;         /* Size of number of element in partition */
    int padded_partition[required_rows][padded_size];       /* Partition of padded matrix */
    float calculated_partition[partition][matrix_size];     /* Completed partition */

    // main - get rows and setup
    if (me == MAIN_PROCESS) {
        // get initial matrix
        for (i = 1; i < matrix_size + 1; i++)
            if (get_row(fd[0], matrix_size, i, &start_matrix[i][1]) == -1)
                handle_error("Unable to get and intialise matrix");
        
        // create padded matrix of zeros with depth width
        create_padded_matrix(padded_size, matrix_size, depth, padded_matrix, start_matrix);
        
        // calculate the offset of each process
        calculate_displacement(nproc, initial_partition, padded_size, overflow, 
                                group_size, displacement, overflow_group_size);
    }
    
    // scatter work
    for (i = 0; i < initial_req_rows; i++)
        if ((MPI_Scatterv(&padded_matrix[i], 
                        group_size,
                        displacement,
                        MPI_INT, 
                        &padded_partition[i], 
                        padded_size, 
                        MPI_INT,
                        MAIN_PROCESS,
                        MPI_COMM_WORLD)) < 0)
            handle_error("Scattering failed");

    // scatter any overflow
    if (overflow > 0)
        if ((MPI_Scatterv(&padded_matrix[initial_req_rows], 
                        overflow_group_size,
                        displacement,
                        MPI_INT, 
                        &padded_partition[initial_req_rows], 
                        padded_size, 
                        MPI_INT,
                        MAIN_PROCESS,
                        MPI_COMM_WORLD)) < 0)
            handle_error("Scattering failed");
    
    // conduct calculations
    calculate_weighted_sum(depth, partition, matrix_size, calculated_partition, padded_partition);

    // adjust displacement and group size for gather
    if (me == MAIN_PROCESS)
        calculate_displacement(nproc, initial_partition, matrix_size, overflow, 
                                group_size, displacement, 0);
    // gather partition sizes
    if (MPI_Gather(&matrix_partition,
                    1,
                    MPI_INT,
                    &group_size,
                    1,
                    MPI_INT,
                    MAIN_PROCESS,
                    MPI_COMM_WORLD) < 0)
        handle_error("Unable to gather all elements");

    // gather all results
    if (MPI_Gatherv(&calculated_partition[0],
                    matrix_size * partition,
                    MPI_FLOAT,
                    &complete_matrix[0][0],
                    group_size,
                    displacement,
                    MPI_FLOAT,
                    MAIN_PROCESS,
                    MPI_COMM_WORLD) < 0)
        handle_error("Unable to gather all elements");

    
    // main write to file
    if (me == MAIN_PROCESS) {
        int final_matrix[matrix_size + 1][matrix_size + 1];     /* Completed rounded matrix */
        
        // round matrix
        for (i = 0; i < matrix_size; i++)
            for (j = 0; j < matrix_size; j++) 
                final_matrix[i + 1][j + 1] = round(complete_matrix[i][j]);

        // write to file
        for (i = 1; i < matrix_size + 1; i++)
            if (set_row(fd[1], matrix_size, i, &final_matrix[i][1]) < 0) 
                handle_error("Unable to write matrix to file");
    
        // close file descriptors
        for (i = 0; i < 2; i++)
            close(fd[i]);
    }

    // terminate MPI
    if (MPI_Finalize() == -1)
        handle_error("Unable to terminate MPI execution environment");
    
    return SUCCESS;
}