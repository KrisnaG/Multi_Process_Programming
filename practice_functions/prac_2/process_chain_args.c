/**
 * @file process_chain_args.c
 * @brief Creates n process given by the user input in the command line.
 * Parent only creates one child process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SUCCESS 1
#define FAILURE 0

int parse_args(int argc, char const *argv[], int *n) {
    // Error checking input
    if (argc <= 1) {
        printf("Requires an input\n");
        return FAILURE;
    } else if (argc > 2) {
        printf("Only one argument is allowed\n");
        return FAILURE;
    }
    if ((*n = atoi(argv[1])) < 1) {
        printf("Enter a valid integer greater than 0\n");
        return FAILURE;
    }
    return SUCCESS;
}

int main(int argc, char const *argv[])
{
    int num_processes, i;
    pid_t pid;

    if (parse_args(argc, argv, &num_processes) == 0) {
        printf("Usage: chainargs <number of processes>\n");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < num_processes; ++i) {
        if ((pid = fork()) < 0) {
            printf("Error in fork\n");
            exit(-1);
        } else if (pid == 0) {
            // child process
        } else {
            // parent process
            break;
        }
    }

    printf("This is process %ld with parent %ld\n",
            (long)getpid(), (long)getppid());

    return 0;
}
