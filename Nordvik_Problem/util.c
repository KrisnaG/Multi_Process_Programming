#include <pthread.h>
#include "util.h"

void * car_east(void * arg)
{

}

void * car_west(void * arg)
{

}

void * truck_east(void * arg)
{

}

void * truck_west(void * arg)
{

}

int create_and_run_thread(thread_info_t *t_info, int num_threads, void *(*routine))
{
    int i;
    
    /* set up thread ids */
    for(i = 0; i < num_threads; ++i) 
        t_info[i].id = i;

    /* run the threads */
    for(i = 0; i < num_threads; ++i) {
        if (pthread_create(&t_info[i].thread, NULL, routine, (void *) &t_info[i].id) < 0)
            return FAILURE;
    }

    return SUCCESS;
}