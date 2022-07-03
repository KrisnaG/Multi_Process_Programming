#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

/**
 * @brief Sums an array of integers by using
 * multiple processing
 * 
 * @param array integer array to sum
 * @return int - sum
 */
int sum_int_array(const int array[], const int SIZE)
{
    int NUM_CHILDREN;
    
    if ( SIZE > 3 ) {
        NUM_CHILDREN = SIZE / 4;
    }
    else {
        NUM_CHILDREN = 1;
    }

    pid_t child;
    pid_t* children;
    int status;
    int sum = 0;

    for ( int i = 0; i < NUM_CHILDREN; ++i ) {
        child = fork();
        if ( child == -1 ) {
            printf("Error creating child process.\n");
            _exit(-1);
        }
        else if ( child == 0 ) {

        }
        else {
            children[i] = child;
        }
    }

    for ( int i = 0; i < NUM_CHILDREN; ++i ) {
        child = children[i];
        
        if ( waitpid(child, &status, 0) < 0 ) {
            printf("Error - No child to wait for!");
            return 0;
        }
        else if (WEXITSTATUS(status) >= 0) {

        }

    }

    return sum;
}

// Driver for testing
int main(int argc, char const *argv[])
{
    int numbers[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};

    printf("Sum: %d\n", sum_int_array(numbers, (sizeof(numbers)/sizeof(int))));

    return 0;
}