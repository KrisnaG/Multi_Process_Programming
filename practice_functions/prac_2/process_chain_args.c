/**
 * @file process_chain_args.c
 * @brief Creates n process given by the user input in the command line
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int num_processes, i;
    pid_t pid;
    // Error checking input
    if (argc <= 1) {
        printf("Requires an input\n");
        exit(-1);
    } else if (argc > 2) {
        printf("Only one argument is allowed\n");
        exit(-1);
    }
    num_processes = atoi(argv[1]);
    if (num_processes < 1) {
        printf("Enter a valid integer greater than 0\n");
        exit(-1);
    }

    for (i = 1; i < num_processes; ++i) {
        if ((pid = fork()) < 0) {
            prinft("Error in fork\n");
            exit(-1);
        } else if (pid == 0) {
            // child process
        } else {
            // parent process
        }
    }

    return 0;
}
