/**
 * @file nordvik_util.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Implementation of nordvik_util.h
 * Utility library for the nordvik problem containing thread routines
 * (cars and trucks traveling east and west) and thread and semaphore
 * handling functions.
 * 
 */

#include <stdio.h>
#include <pthread.h>
#include "nordvik_util.h"
#include "sem_ops.h"

/**
 * @brief Thread routine for a car traveling east
 * on the bridge
 * 
 * @param arg pointer to thread argument - thread info
 */
void * car_traveling_east(void * arg)
{
    /* extract thread info from arg */
    thread_info_t *t_info = (thread_info_t *) arg;
    int *bridge_lock = &t_info->locks_counters->bridge_lock;
    int *east_lock = &t_info->locks_counters->east_lock;
    int *east_limit = &t_info->locks_counters->car_east_limit;
    int *east_counter = &t_info->locks_counters->car_east_count;
    
    /* varies arrival time to bridge */
    INITAL_WAIT_TIME;

    /* wait for limit lock if bridge is full then get lock on limit */
    sem_wait(*east_limit);

    /* get lock for counter, then increment counter */
    sem_wait(*east_lock);
    *east_counter = *east_counter + 1;

    /* first car waits and locks bridge (decrement) */
    if (*east_counter == 1)
        sem_wait(*bridge_lock);

    /* release lock for counter */
    sem_release(*east_lock);
    
    /* cross bridge */
    fprintf(stdout, "Car %d going east on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going east off the bridge\n", t_info->id);

    /* get lock for counter, then decrement counter */
    sem_wait(*east_lock);
    *east_counter = *east_counter - 1;

    /* last car on bridge releases lock on bridge (increment) */
    if (*east_counter == 0)
        sem_release(*bridge_lock);
    
   /* release lock for counter */
    sem_release(*east_lock);

    /* release limit lock to allow more cars on bridge */
    sem_release(*east_limit);

    pthread_exit(NULL);
}

/**
 * @brief Thread routine for a car traveling west
 * on the bridge
 * 
 * @param arg pointer to thread argument - thread info
 */
void * car_traveling_west(void * arg)
{
    /* extract thread info from arg */
    thread_info_t *t_info = (thread_info_t *) arg;
    int *bridge_lock = &t_info->locks_counters->bridge_lock;
    int *west_lock = &t_info->locks_counters->west_lock;
    int *west_limit = &t_info->locks_counters->car_west_limit;
    int *west_counter = &t_info->locks_counters->car_west_count;
    
    /* varies arrival time to bridge */
    INITAL_WAIT_TIME;

    /* wait for limit lock if bridge is full then get lock on limit */
    sem_wait(*west_limit);

    /* get lock for counter, then increment counter */
    sem_wait(*west_lock);
    *west_counter = *west_counter + 1;
    
    /* first car waits and locks bridge (decrement) */
    if (*west_counter == 1)
        sem_wait(*bridge_lock);

    /* release lock for counter */
    sem_release(*west_lock);

    /* cross bridge */
    fprintf(stdout, "Car %d going west on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going west off the bridge\n", t_info->id);

    /* get lock for counter, then decrement counter */
    sem_wait(*west_lock);
    *west_counter = *west_counter - 1;

    /* last car on bridge releases lock on bridge (increment) */
    if (*west_counter == 0)
        sem_release(*bridge_lock);
    
    /* release lock for counter */
    sem_release(*west_lock);

    /* release limit lock to allow more cars on bridge */
    sem_release(*west_limit);

    pthread_exit(NULL);
}

/**
 * @brief Thread routine for a truck traveling east
 * on the bridge
 * 
 * @param arg pointer to thread argument - thread info
 */
void * truck_traveling_east(void * arg)
{
    /* extract thread info from arg */
    thread_info_t *t_info = (thread_info_t *) arg;
    int *bridge_lock = &t_info->locks_counters->bridge_lock;
    
    /* varies arrival time to bridge */
    INITAL_WAIT_TIME;

    /* wait for bridge to be free, then lock (decrement) */
    sem_wait(*bridge_lock);

    /* cross bridge */
    fprintf(stdout, "Truck %d going east on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going east off the bridge\n", t_info->id);

    /* release bridge lock (increment) */
    sem_release(*bridge_lock);

    pthread_exit(NULL);
}

/**
 * @brief Thread routine for a truck traveling west
 * on the bridge
 * 
 * @param arg pointer to thread argument - thread info
 */
void * truck_traveling_west(void * arg)
{
    /* extract thread info from arg */
    thread_info_t *t_info = (thread_info_t *) arg;
    int *bridge_lock = &t_info->locks_counters->bridge_lock;
    
    /* varies arrival time to bridge */
    INITAL_WAIT_TIME;

    /* wait for bridge to be free, then lock (decrement) */
    sem_wait(*bridge_lock);

    /* cross bridge */
    fprintf(stdout, "Truck %d going west on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going west off the bridge\n", t_info->id);

    /* release bridge lock (increment) */
    sem_release(*bridge_lock);

    pthread_exit(NULL);
}

/**
 * @brief Initialise all semaphore locks and car counters
 * 
 * @param lock_counter semaphore locks and counters struct to initialise
 * @return int : 0 if successful, -1 if failed  
 */
int initialise_locks_counters(locks_counters_t * lock_counter)
{
    int i;

    /* set all counters */
    lock_counter->car_east_count = 0;
    lock_counter->car_west_count = 0;

    /* initialise all semaphores */
    if ((lock_counter->bridge_lock = semtran(IPC_PRIVATE)) < 0) 
        return FAILURE;

    if ((lock_counter->east_lock = semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    if ((lock_counter->west_lock = semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    if ((lock_counter->car_east_limit = semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    if ((lock_counter->car_west_limit= semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    /* increment all semaphore locks */
    if (sem_release(lock_counter->bridge_lock))
        return FAILURE;
    
    if (sem_release(lock_counter->east_lock))
        return FAILURE;
    
    if (sem_release(lock_counter->west_lock))
        return FAILURE;

    /* increment east and west car limit semaphore to max bridge weight */
    for(i = 0; i < BRIDGE_WEIGHT; ++i) {
        if (sem_release(lock_counter->car_east_limit))
            return FAILURE;

        if (sem_release(lock_counter->car_west_limit))
            return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief Setups thread info, then creates a thread and runs the given 
 * routine.
 * 
 * @param t_info struct containing threads to wait for
 * @param num_threads number of threads to wait for
 * @param lock_count struct containing shared semaphores and counters
 * @param routine routine for the thread to execute
 * @return int : 0 if successful, -1 if failed 
 */
int create_and_run_threads(thread_info_t *t_info, int num_threads, 
    locks_counters_t *lock_count, void *(*routine)(void *))
{
    int i;
    
    /* set up thread ids and locks */
    for(i = 0; i < num_threads; ++i) { 
        t_info[i].id = i;
        t_info[i].locks_counters = lock_count;
    }

    /* run the threads */
    for(i = 0; i < num_threads; ++i) {
        if ((pthread_create(&t_info[i].thread, NULL, routine, (void *) &t_info[i])) != 0)
            return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief Wait for a number of given threads to finish execution.
 * 
 * @param t_info struct containing threads to wait for
 * @param num_threads number of threads to wait for
 * @return int : 0 if successful, -1 if failed  
 */
int wait_for_threads(thread_info_t *t_info, int num_threads)
{
    int i;

    for(i = 0; i < num_threads; ++i) {
        if ((pthread_join(t_info[i].thread, NULL)) != 0)
            return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief Removes all semaphores from the system.
 * 
 * NOTE: cleanup will fail if not all semaphores in 
 * locks_counters_t have been created.
 * 
 * @param locks struct containing semaphores to remove
 * @return int : 0 if successful, -1 if failed  
 */
int cleanup(locks_counters_t *locks)
{
    if (rm_sem(locks->bridge_lock) ||
        rm_sem(locks->east_lock) ||
        rm_sem(locks->west_lock) ||
        rm_sem(locks->car_east_limit) ||
        rm_sem(locks->car_west_limit))
        return FAILURE;

    return SUCCESS; 
}