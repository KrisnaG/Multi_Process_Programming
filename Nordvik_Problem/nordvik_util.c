#include <stdio.h>
#include <pthread.h>
#include "nordvik_util.h"

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void * car_traveling_east(void * arg)
{
    int *me;
    me = (int *) arg;

    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Car %d going east on the bridge", *me);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going east off the bridge", *me);

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
    int *me;
    me = (int *) arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Car %d going west on the bridge", *me);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Car %d going west off the bridge", *me);

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
    int *me;
    me = (int *) arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Truck %d going east on the bridge", *me);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going east off the bridge", *me);

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
    int *me;
    me = (int *) arg;
    
    INITAL_WAIT_TIME;

    /* critical section */
    fprintf(stdout, "Truckar %d going west on the bridge", *me);
    sleep(CROSSINGTIME);
    fprintf(stdout, "Truck %d going west off the bridge", *me);

    pthread_exit(NULL);
}

/**
 * @brief Create a and run thread object
 * 
 * @param t_info 
 * @param num_threads 
 * @return int : 0 if successful, -1 if failed
 */
int create_and_run_thread(thread_info_t *t_info, int num_threads, void *(*routine)(void *))
{
    int i;
    
    /* set up thread ids */
    for(i = 0; i < num_threads; ++i) 
        t_info[i].id = i;

    /* run the threads */
    for(i = 0; i < num_threads; ++i) {
        if ((pthread_create(&t_info[i].thread, NULL, routine, (void *) &t_info[i].id) )< 0)
            return FAILURE;
    }

    return SUCCESS;
}

