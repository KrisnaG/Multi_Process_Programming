/**
 * @file ring_util.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#include <unistd.h>
#include "ring_util.h"

/**
 * @brief 
 * 
 * @return int 
 */
int make_trivial_ring()
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(-2);

    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1))
        return(-3);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-4);

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
        return(-5);

    if ((*pid = fork()) == -1)
        return(-6);

    if(*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return(-7);

    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return(-8);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-9);

    return(SUCCESS);
}
