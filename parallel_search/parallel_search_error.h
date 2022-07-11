/**
 * @file parallel_search_error.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#define parallel_search_error.h

/* Successful operation */
#define SUCCESS 0

/* Error parsing arguments from command line */
#define PARSE_FAILURE -1

/* Errors making trivial ring */
#define TR_PIPE_FAILURE -2
#define TR_DUP_FAILURE -3
#define TR_CLOSE_FAILURE -4

/* Errors with adding new nodes */
#define NEW_NODE_PIPE_FAILURE -5
#define NEW_NODE_FORK_FAILURE -6
#define NEW_NODE_DUPOUT_FAILURE -7 
#define NEW_NODE_DUPIN_FAILURE -8
#define NEW_NODE_CLOSE_FAILURE -9