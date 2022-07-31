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

int main(int argc, char *argv[])
{  
    thread_info_t cars_east[NUM_CARS_EAST];
    thread_info_t cars_west[NUM_CARS_WEST];
    thread_info_t trucks_east[NUM_TRUCKS_EAST];
    thread_info_t trucks_west[NUM_TRUCKS_WEST];
    locks_counters_t lock_counter;

    /*  */
    if ((lock_counter.bridge_lock = semtran(IPC_PRIVATE)) < 0) 
        handle_error("Unable to get semaphore for brick lock");

    if ((lock_counter.car_east_lock = semtran(IPC_PRIVATE)) < 0)
        handle_error("Unable to get semaphore for car east lock");

    if ((lock_counter.car_west_lock = semtran(IPC_PRIVATE)) < 0)
        handle_error("Unable to get semaphore for car west lock");

    /*  */
    if (initialise_locks_counters(&lock_counter))
        handle_error("");

    /* create and run threads */
    if (create_and_run_thread(cars_east, NUM_CARS_EAST, &lock_counter, car_traveling_east))
        handle_error("Unable to create thread for cars_east");
    
    if (create_and_run_thread(cars_west, NUM_CARS_WEST, &lock_counter, car_traveling_west)) 
        handle_error("Unable to create thread for cars_west");

    if (create_and_run_thread(trucks_east, NUM_TRUCKS_EAST, &lock_counter, truck_traveling_east)) 
        handle_error("Unable to create thread for truck_east");

    if (create_and_run_thread(trucks_west, NUM_TRUCKS_WEST, &lock_counter, truck_traveling_west))
        handle_error("Unable to create thread for truck_west");

    /* wait for threads to complete */
    if (wait_for_thread(cars_east, NUM_CARS_EAST))
        handle_error("");
    
    if (wait_for_thread(cars_west, NUM_CARS_WEST))
        handle_error("");
    
    if (wait_for_thread(trucks_east, NUM_TRUCKS_EAST))
        handle_error("");
    
    if (wait_for_thread(trucks_west, NUM_TRUCKS_WEST))
        handle_error("");

    /* clean up */
    if (cleanup())

    exit(EXIT_SUCCESS);
}
