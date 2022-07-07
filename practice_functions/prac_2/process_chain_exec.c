/**
 * @file process_chain_exec.c
 * @brief execute the command given on the command line a number of times
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define SUCCESS 1
#define FAILURE 0

int parse_args(int argc, char *argv[], int *n) {
    // Error checking input
    if (argc <= 1) {
        printf("Requires an input\n");
        return FAILURE;
    } else if ((*n = atoi(argv[1])) < 1) {
        printf("Enter a valid integer greater than 0\n");
        return FAILURE;
    }
    return SUCCESS;
}

int main(int argc, char *argv[])
{
    pid_t pid;
    pid_t *children;
    int i, status, num_process;

    // check input parameters
    if( parse_args(argc, argv, &num_process) == 0) {
        exit(EXIT_FAILURE);
    }

    children = (pid_t*) malloc(num_process * sizeof(pid_t));

    // create child for each command
    for (i = 0; i < num_process; ++i) {
        if ((pid = fork()) < 0) {
            perror("The fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // child process
            if ((execvp(argv[2], &argv[2])) < 0) {
                perror("The exec of command failed");
                exit(1);
            }
        } else {
            // parent process
            children[i] = pid;
        }
    }

    // wait for each child to complete
    for (i = 0; i < num_process; ++i) {
        if (waitpid(children[i], &status, 0) < 0) {
            perror("No child to wait for");
        } else if (status != 0) {
            perror("Child exited abnormally");
        }
    }

    free(children);

    exit(EXIT_SUCCESS);
}
