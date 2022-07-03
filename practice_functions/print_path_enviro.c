/**
 * @file print_path_enviro.c
 * @author Krisna Gusti
 * @brief A program that writes the value of the PATH environment variable to STDOUT.
 */

#include <stdio.h>
#include <string.h>

extern char **environ;

int main(int argc, char const *argv[], char *envp[])
{
    int i = 0;
    
    while (envp[i] != NULL) {
        // get the first token - environmental name
        char * token = strtok(envp[i], "=");

        if (strcmp(token, "PATH") == 0) {
            // get the second half of the token
            token = strtok(NULL, "=");
            printf("%s\n", token);
        }
        
        i++;
    }

    return 0;
}
