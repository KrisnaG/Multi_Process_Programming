// makeargv written by ian a. mason @ une 1/8/2002
// the argv and each argv[i] can be freed after use.
// no sharing with the original string.

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define DEBUG 0

int makeargv(const char *s, const char *delimiters, char ***argvp){
  if(argvp == NULL)
    return -1;
  if(s == NULL){
    *argvp = NULL;
    return 0;
  } else {
    int argc = 0, len = strlen(s);
    if(len == 0){
      *argvp = NULL;
      return 0;
    } else {
      int start = 0, end = 0;
      char **argv = (char **)calloc(len, sizeof(char *));
      if(argv == NULL) return -1;
      if(DEBUG)
	fprintf(stderr,
		"Entering loop &s[start] = \"%s\"\n",
		&s[start]);
      while(s[start] != '\0'){
	if(DEBUG)
	  fprintf(stderr, "Looping\n");
	//remove all starting delimiters
	while((s[start] != '\0') &&
	      (strchr(delimiters, s[start]) != NULL))
	  start++;
	if(DEBUG)
	  fprintf(stderr,
		  "Delimiters removed &s[start] = \"%s\"\n",
		  &s[start]);
	if(s[start] == '\0'){
	  if(DEBUG)
	    fprintf(stderr, "End of string\n");
	  if(argc == 0){
	    free(argv);
	    *argvp = NULL;
	    return argc;
	  } else {
	    argv[argc] = NULL;
	    *argvp = argv;
	    return argc;
	  }
	} //s[start] != '\0'
	end = start;
	//find the end of the current token
	while((s[end] != '\0') &&
	      strchr(delimiters, s[end]) == NULL)
	  end++;
	if(DEBUG)
	  fprintf(stderr,
		  "Token end found: &s[end] = \"%s\"\n",
		  &s[end]);
	argv[argc] = (char *)calloc((end - start) + 1, sizeof(char));
	if(argv[argc] == NULL){
	  free(argv);
	  return -1;
	}
	strncpy(argv[argc], &s[start], end - start);
	argv[argc][end - start] = '\0';
	argc++;
	start = end;
      }
      argv[argc] = NULL;
      *argvp = argv;
      return argc;
    }
  }
}
