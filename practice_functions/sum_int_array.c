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
int sum_int_array(int* array)
{
    const int SIZE = sizeof(array) / sizeof(int);
    
}

// Driver for testing
int main(int argc, char const *argv[])
{
    int numbers[] = {1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};

    int sum = sum_int_array(&numbers);

    printf("Sum: %d", sum);

    return 0;
}