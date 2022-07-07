/**
 * @file tokenAt.c
 * @brief 
 */

#include <stdio.h> 
#include <stdlib.h>
#include "makeargv.h"

int main(int argc, char *argv[]) {
    char **myargv, delim[] = " \t";
    int  num, numtokens;
  
    if ((argc !=  3) || ((num = atoi(argv[1])) <= 0)) {
        fprintf(stderr, "Usage: %s <number> <string>\n", argv[0]);
        exit(EXIT_FAILURE); 
    }
  
    if ((numtokens = makeargv(argv[2], delim, &myargv)) < 0) { 
        fprintf(stderr, "Argument array could not be constructed\n"); 
        exit(EXIT_FAILURE); 
    }
  
    if (num > numtokens) { 
        fprintf(stderr, "String contains only %d tokens\n", numtokens); 
        exit(EXIT_FAILURE); 
    }
  
    fprintf(stderr, "%s\n", myargv[num - 1]);
  
    exit(EXIT_SUCCESS);
}