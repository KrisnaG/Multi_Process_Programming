#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

/**
 * @brief Prints n numbers to terminal using
 * multiple processing. This only utilises one
 * child process, printing half the numbers in the
 * child process and the rest in the parent process.
 * 
 * @param n amount of numbers to print (starts at 0)
 */
void print_n_numbers(int n) 
{
    if ( n > 1 ) {
        const int half = n / 2;
        pid_t pid = fork();

        if ( pid == -1 ) {
            printf("Error creating child process\n");
            _exit(-1);
        }
        // child process - prints first half
        else if ( pid == 0) {
            for (int i = 0; i < half; ++i) {
                printf("%d ", i);
            }
        }
        // parent process - waits and prints second half
        else {
            int status;

            if ( waitpid(pid, &status, 0) == -1 ) {
                printf("No child to wait for!\n");
            }
            else {
                for (int i = half; i <= n; ++i) {
                    printf("%d ", i);
                }
            }
        }
    } 
    // just print out if only 1 number
    else if ( n == 1 ) {
        printf("%d", n);
    } 
    else {
        printf("Number must be greater than one.\n");
    }
}

// Driver for testing
int main(int argc, char const *argv[])
{
    print_n_numbers(10);
    return 0;
}
