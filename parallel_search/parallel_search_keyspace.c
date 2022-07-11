/**
 * @file parallel_search_keyspace.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "parallel_search_error.h"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @param np 
 * @return int 
 */
int parse_args(int argc,  char *argv[ ], int *np)
{
    if ( (argc != 5) || ((*np = atoi (argv[1])) <= 0) ) {
        fprintf(stderr, "Usage: %s <num. procs.> <partial key> <cipher file> <plain file>\n", argv[0]);
        return(PARSE_FAILURE);
    }

    return(SUCCESS);
}

/**
 * @brief 
 * 
 * @return int 
 */
int make_trivial_ring()
{
    int fd[2];
    
    if (pipe (fd) == -1)
        return(TR_PIPE_FAILURE);

    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1))
        return(TR_DUP_FAILURE);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(TR_CLOSE_FAILURE);

    return(SUCCESS); 
}

/**
 * @brief 
 * 
 * @param pid 
 * @return int 
 */
int add_new_node(int *pid)
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(NEW_NODE_PIPE_FAILURE);

    if ((*pid = fork()) == -1)
        return(NEW_NODE_FORK_FAILURE);

    if(*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return(NEW_NODE_DUPOUT_FAILURE);

    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return(NEW_NODE_DUPIN_FAILURE);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(NEW_NODE_CLOSE_FAILURE);

    return(SUCCESS);
}



/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc,  char *argv[ ])
{
    int i;             /* number of this process (starting with 1)   */
    int childpid;      /* indicates process should spawn another     */
    int nprocs;        /* total number of processes in ring          */
    
    if (parse_args(argc, argv, &nprocs) < 0)
        exit(EXIT_FAILURE);
    
    if (make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    }
    
    for (i = 1; i < nprocs;  i++) {
        if(add_new_node(&childpid) < 0){
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE); 
        }
        
        if (childpid) 
            break; 
    }
    
    /* ring process code  */

    if (childpid == 0) {

    }

    fprintf(stderr, "node %d of %d\n", i, nprocs);
    exit(EXIT_SUCCESS);
}