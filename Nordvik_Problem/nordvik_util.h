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

/*  */
#define INITAL_WAIT_TIME sleep( rand() % MAXWAIT )

/*  */
#define handle_error(msg) \
    do { fprintf(stderr,"%s\n", msg); exit(EXIT_FAILURE); } while (0)

/*  */
typedef struct locks_counters_t 
{
    int bridge_lock;                    /* bridge semaphore lock */
    int east_lock;                      /* car east semaphore lock */
    int west_lock;                      /* car west semaphore lock */
    int car_east_limit;                 /*  */
    int car_west_limit;                 /*  */
    int car_east_count;                 /* number of cars currently traveling east */
    int car_west_count;                 /* number of cars currently traveling west */
} locks_counters_t;

/*  */
typedef struct thread_info_t
{
    pthread_t thread;                   /* thread creation id */
    int id;                             /* application defined id */
    locks_counters_t *locks_counters;   /*  */
} thread_info_t;

/* Thread routines */
void * car_traveling_east(void *);
void * car_traveling_west(void *);
void * truck_traveling_east(void *);
void * truck_traveling_west(void *);

/*  */
int initialise_locks_counters(locks_counters_t *);
int create_and_run_thread(thread_info_t *, int, locks_counters_t *, void *(*)(void *));
int wait_for_thread(thread_info_t *, int );
int cleanup(locks_counters_t *lock_counter);

#endif