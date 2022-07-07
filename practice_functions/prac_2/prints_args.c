/**
 * @file prints_args.c
 * @brief Prints the arguments to standard output.
 */

#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("Number of arguments: %d\n", argc-1);

    for(int i = 1; i < argc; ++i) {
        printf("%s\n", argv[i]);
    }

    return 0;
}
