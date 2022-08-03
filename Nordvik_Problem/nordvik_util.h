/**
 * @file nordvik_util.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Utility library for the nordvik problem containing
 * variable macros, error handling, wait time, thread and 
 * semaphore structures, thread routines and utility functions.
 * 
 */

#ifndef NORDVIK_UTIL_H
#define NORDVIK_UTIL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 0           /* Successful operation */
#define FAILURE -1          /* Failed operation */
#define MAXWAIT 20          /* Maximum initial waiting time */
#define CROSSINGTIME 4      /* Time for a vehicle to cross bridge */
#define BRIDGE_WEIGHT 3     /* Amount of cars allowed on bridge at once */

/* Sleep a random amount of time up to a maximum wait time */
#define INITAL_WAIT_TIME sleep( rand() % MAXWAIT )

/* Prints out error message passed and exits */
#define handle_error(msg) \
    do { fprintf(stderr,"%s\n", msg); exit(EXIT_FAILURE); } while (0)

/* Stores all semaphore locks and car counters */
typedef struct locks_counters_t 
{
    int bridge_lock;                    /* bridge semaphore lock */
    int east_lock;                      /* car east semaphore lock */
    int west_lock;                      /* car west semaphore lock */
    int car_east_limit;                 /* limit for car traveling east semaphore lock */
    int car_west_limit;                 /* limit for car traveling west semaphore lock */
    int car_east_count;                 /* number of cars currently traveling east */
    int car_west_count;                 /* number of cars currently traveling west */
} locks_counters_t;

/* Stores all relevant thread information */
typedef struct thread_info_t
{
    pthread_t thread;                   /* thread creation id */
    int id;                             /* application defined id */
    locks_counters_t *locks_counters;   /* address of all semaphore locks and counters */
} thread_info_t;

// Thread routines
void * car_traveling_east(void *);
void * car_traveling_west(void *);
void * truck_traveling_east(void *);
void * truck_traveling_west(void *);

// Utility functions
int initialise_locks_counters(locks_counters_t *);
int create_and_run_threads(thread_info_t *, int, locks_counters_t *, void *(*)(void *));
int wait_for_threads(thread_info_t *, int );
int cleanup(locks_counters_t *);

#endif