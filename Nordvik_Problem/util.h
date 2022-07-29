#ifndef UTIL_H
#define UTIL_H

#define SUCCESS 0           /* Successful operation */
#define FAILURE -1          /* Failed operation */
#define MAXWAIT 20          /* Maximum initial waiting time */
#define CROSSINGTIME 4      /* Time for a vehicle to cross bridge */
#define BRIDGE_WEIGHT 4     /* Amount of cars allowed on bridge */
#define TRUCK_WEIGHT 4      /* Weight of a single truck */
#define CAR_WEIGHT 1        /* Weight of a single car */

/*  */
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

/*  */
typedef struct thread_info_t
{
    pthread_t thread;   /* thread creation id */
    int id;             /* application defined id */
} thread_info_t;

/* Thread routines */
void * car_east(void *);
void * car_west(void *);
void * truck_east(void *);
void * truck_west(void *);

/*  */
int create_and_run_thread(thread_info_t *, int, void *(*));

#endif