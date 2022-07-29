#ifndef NORDVIK_UTIL_H
#define NORDVIK_UTIL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0           /* Successful operation */
#define FAILURE -1          /* Failed operation */
#define MAXWAIT 20          /* Maximum initial waiting time */
#define CROSSINGTIME 4      /* Time for a vehicle to cross bridge */
#define BRIDGE_WEIGHT 4     /* Amount of cars allowed on bridge at once */

/*  */
#define INITAL_WAIT_TIME sleep( rand() % MAXWAIT )

/*  */
#define handle_error(msg) \
    do { fprintf(stderr,"%s\n", msg); exit(EXIT_FAILURE); } while (0)

/*  */
typedef struct thread_info_t
{
    pthread_t thread;   /* thread creation id */
    int id;             /* application defined id */
} thread_info_t;

/* Thread routines */
void * car_traveling_east(void *);
void * car_traveling_west(void *);
void * truck_traveling_east(void *);
void * truck_traveling_west(void *);

/*  */
int create_and_run_thread(thread_info_t *, int, void *(*)(void *));
int cleanup();

#endif