#include <stdio.h>
#include <pthread.h>
#include "nordvik_util.h"
#include "sem_ops.h"

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void * car_traveling_east(void * arg)
{
    thread_info_t *t_info = &arg;
    int *bridge_lock = &t_info->locks_counters->bridge_lock;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Car %d going east on the bridge", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going east off the bridge", t_info->id);

    pthread_exit(NULL);
}

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void * car_traveling_west(void * arg)
{
    thread_info_t *t_info = &arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Car %d going west on the bridge", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going west off the bridge", t_info->id);

    pthread_exit(NULL);
}

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void * truck_traveling_east(void * arg)
{
    thread_info_t *t_info = &arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Truck %d going east on the bridge", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going east off the bridge", t_info->id);

    pthread_exit(NULL);
}

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void * truck_traveling_west(void * arg)
{
    thread_info_t *t_info = &arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Truck %d going west on the bridge", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going west off the bridge", t_info->id);

    pthread_exit(NULL);
}

int initialise_locks_counters(locks_counters_t * lock_counter)
{
    lock_counter->car_east_count = 0;
    lock_counter->car_west_count = 0;
}

/**
 * @brief Create a and run thread object
 * 
 * @param t_info 
 * @param num_threads 
 * @param lock_count 
 * @param routine 
 * @return int : 0 if successful, -1 if failed 
 */
int create_and_run_thread(thread_info_t *t_info, int num_threads, 
    locks_counters_t * lock_count, void *(*routine)(void *))
{
    int i;
    
    /* set up thread ids */
    for(i = 0; i < num_threads; ++i) { 
        t_info[i].id = i;
        t_info[i].locks_counters = lock_count;
    }

    /* run the threads */
    for(i = 0; i < num_threads; ++i) {
        if ((pthread_create(&t_info[i].thread, NULL, routine, (void *) &t_info[i])) < 0)
            return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief 
 * 
 * @param t_info 
 * @param num_threads 
 * @return int 
 */
int wait_for_thread(thread_info_t *t_info, int num_threads)
{
    int i;

    for(i = 0; i < num_threads; ++i) {
        if ((pthread_join(t_info->thread, NULL)) < 0)
            return FAILURE;
    }

    return SUCCESS;
}

int cleanup()
{
   return SUCCESS; 
}