/**
 * @file nordvik.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Nordvik Suspension Bridge Problem
 */

#include <pthread.h>
#include "sem_ops.h"
#include "nordvik_util.h"

#define NUM_CARS_EAST 4
#define NUM_CARS_WEST 4
#define NUM_TRUCKS_EAST 2
#define NUM_TRUCKS_WEST 2

int bridge_lock;

int car_east_lock;
int car_west_lock;

int car_east_count;
int car_east_count;


int main(int argc, char *argv[])
{
    thread_info_t cars_east[NUM_CARS_EAST];
    thread_info_t cars_west[NUM_CARS_WEST];
    thread_info_t trucks_east[NUM_TRUCKS_EAST];
    thread_info_t trucks_west[NUM_TRUCKS_WEST];

    /*  */
    if ((bridge_lock = semtran(IPC_PRIVATE)) < 0) 
        handle_error("Unable to get semaphore for brick lock");

    if ((car_east_lock = semtran(IPC_PRIVATE)) < 0)
        handle_error("Unable to get semaphore for car east lock");

    if ((car_west_lock = semtran(IPC_PRIVATE)) < 0)
        handle_error("Unable to get semaphore for car west lock");

    /* create and run threads */
    if (create_and_run_thread(cars_east, NUM_CARS_EAST, car_traveling_east))
        handle_error("a");
    
    if (create_and_run_thread(cars_west, NUM_CARS_WEST, car_traveling_west)) 
        handle_error("b");

    if (create_and_run_thread(trucks_east, NUM_TRUCKS_EAST, truck_traveling_east)) 
        handle_error("c");

    if (create_and_run_thread(trucks_west, NUM_TRUCKS_WEST, truck_traveling_west))
        handle_error("d");

    /* wait for threads to complete */


    /* clean up */


    exit(EXIT_SUCCESS);
}
