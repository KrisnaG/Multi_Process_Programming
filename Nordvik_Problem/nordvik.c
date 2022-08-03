/**
 * @file nordvik.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Nordvik Suspension Bridge Problem
 *  
 * Problem: The road to Nordvik carries traffic (cars and trucks) in both 
 * directions, except over the 1 lane suspension bridge wide enough to carry
 * traffic in one direction. If there are vehicles traveling on the bridge,
 * any opposing vehicles must wait to all vehicles are off the bridge before
 * traveling across. Multiple cars traveling the same direction can
 * enter onto the bridge, however, only a single truck can be on the bridge
 * at any one time.
 * 
 * This program creates different numbers of threads representing cars and 
 * trucks crossing the bridge, from either direction.
 * 
 * If any errors are encountered, the program will print out the
 * error to standard error, then terminate the program.
 *
 * Parameters: 
 *      Nil
 * 
 * Returns: 
 *      0 on Success
 * 
 * Library requirements:
 *      1. nordvik_util.h
 *      2. sem_ops.h
 * 
 * Build (two methods):
 *      1. gcc -Wall -pedantic nordvik.c nordvik_util.c -pthread -o nordvik
 *      2. make build
 * 
 * Run (two methods):
 *      1. nordvik
 *      2. make run
 *  
 */

#include "nordvik_util.h"

#define NUM_CARS_EAST 4         /* Total number of cars traveling east */
#define NUM_CARS_WEST 4         /* Total number of cars traveling west */
#define NUM_TRUCKS_EAST 2       /* Total number of trucks traveling east */
#define NUM_TRUCKS_WEST 2       /* Total number of trucks traveling west */

int main(void)
{  
    thread_info_t cars_east[NUM_CARS_EAST];         /* all info for cars traveling east */
    thread_info_t cars_west[NUM_CARS_WEST];         /* all info for cars traveling west */
    thread_info_t trucks_east[NUM_TRUCKS_EAST];     /* all info for trucks traveling east */
    thread_info_t trucks_west[NUM_TRUCKS_WEST];     /* all info for trucks traveling west */
    locks_counters_t lock_counter;                  /* all semaphore locks and car counters */

    /* initialise all locks and counters */
    if (initialise_locks_counters(&lock_counter))
        handle_error("Could not initialise locks");

    /* create and run threads */
    if (create_and_run_threads(cars_east, NUM_CARS_EAST, &lock_counter, car_traveling_east))
        handle_error("Unable to create thread for cars_east");
    
    if (create_and_run_threads(cars_west, NUM_CARS_WEST, &lock_counter, car_traveling_west)) 
        handle_error("Unable to create thread for cars_west");

    if (create_and_run_threads(trucks_east, NUM_TRUCKS_EAST, &lock_counter, truck_traveling_east)) 
        handle_error("Unable to create thread for truck_east");

    if (create_and_run_threads(trucks_west, NUM_TRUCKS_WEST, &lock_counter, truck_traveling_west))
        handle_error("Unable to create thread for truck_west");

    /* wait for threads to complete */
    if (wait_for_threads(cars_east, NUM_CARS_EAST))
        handle_error("Unable to join threads for cars east");
    
    if (wait_for_threads(cars_west, NUM_CARS_WEST))
        handle_error("Unable to join threads for cars west");
    
    if (wait_for_threads(trucks_east, NUM_TRUCKS_EAST))
        handle_error("Unable to join threads for trucks east");
    
    if (wait_for_threads(trucks_west, NUM_TRUCKS_WEST))
        handle_error("Unable to join threads for trucks west");

    /* remove semaphores */
    if (cleanup(&lock_counter))
        handle_error("Failed to remove all semaphores");

    exit(EXIT_SUCCESS);
}
