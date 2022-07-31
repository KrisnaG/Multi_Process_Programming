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
    thread_info_t *t_info = (thread_info_t *) arg;
    int bridge_lock = t_info->locks_counters->bridge_lock;
    int east_lock = t_info->locks_counters->car_east_lock;
    int *east_counter = &t_info->locks_counters->car_east_count;
    
    INITAL_WAIT_TIME;

    sem_wait(east_lock);

    *east_counter = *east_counter + 1;
    
    if (*east_counter == 1)
        sem_wait(bridge_lock);

    sem_release(east_lock);

    /* critical section */
    fprintf(stdout, "Car %d going east on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going east off the bridge\n", t_info->id);

    sem_wait(east_lock);

    *east_counter = *east_counter - 1;

    if (*east_counter == 0)
        sem_release(bridge_lock);
    
    sem_release(east_lock);

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
    thread_info_t *t_info = (thread_info_t *) arg;
    int bridge_lock = t_info->locks_counters->bridge_lock;
    int west_lock = t_info->locks_counters->car_west_lock;
    int *west_counter = &t_info->locks_counters->car_west_count;
    
    INITAL_WAIT_TIME;

    sem_wait(west_lock);

    *west_counter = *west_counter + 1;
    
    if (*west_counter == 1)
        sem_wait(bridge_lock);

    sem_release(west_lock);

    /* critical section */
    fprintf(stdout, "Car %d going west on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going west off the bridge\n", t_info->id);

    sem_wait(west_lock);

    *west_counter = *west_counter - 1;

    if (*west_counter == 0)
        sem_release(bridge_lock);
    
    sem_release(west_lock);

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
    thread_info_t *t_info = (thread_info_t *) arg;
    int bridge_lock = t_info->locks_counters->bridge_lock;
    
    INITAL_WAIT_TIME;

    sem_wait(bridge_lock);

    /* critical section */
    fprintf(stdout, "Truck %d going east on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going east off the bridge\n", t_info->id);

    sem_release(bridge_lock);

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
    thread_info_t *t_info = (thread_info_t *) arg;
    int bridge_lock = t_info->locks_counters->bridge_lock;
    
    INITAL_WAIT_TIME;
    
    sem_wait(bridge_lock);

    /* critical section */
    fprintf(stdout, "Truck %d going west on the bridge\n", t_info->id);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going west off the bridge\n", t_info->id);

    sem_release(bridge_lock);

    pthread_exit(NULL);
}

int initialise_locks_counters(locks_counters_t * lock_counter)
{
    lock_counter->car_east_count = 0;
    lock_counter->car_west_count = 0;

    /*  */
    if ((lock_counter->bridge_lock = semtran(IPC_PRIVATE)) < 0) 
        return FAILURE;

    if ((lock_counter->car_east_lock = semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    if ((lock_counter->car_west_lock = semtran(IPC_PRIVATE)) < 0)
        return FAILURE;

    /*  */
    if (sem_release(lock_counter->bridge_lock))
        return FAILURE;
    
    if (sem_release(lock_counter->car_east_lock))
        return FAILURE;
    
    if (sem_release(lock_counter->car_west_lock))
        return FAILURE;

    return SUCCESS;
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

int cleanup(locks_counters_t *lock_counter)
{
   return SUCCESS; 
}