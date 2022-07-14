/**
 * @file parallel_search_keyspace.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <sys/wait.h>
#include "parallel_search_util.h"

/**
 * @brief Main program that utilises a AES key search using n number of processes
 * given by the user.
 * 
 * @param argc 
 * @param argv 
 * @return 0 on Success, key printed to stderr 
 */
int main(int argc,  char *argv[ ])
{
    /* process related variables */
    int childpid;               // indicates process should spawn another
    int nprocs;                 // total number of processes in ring
    int process;
    unsigned long partition_start = 0, partition_end = 0;
    message_t message;
    size_t message_size = sizeof(message);

    /* encryption/decryption related variables */
    unsigned char *key_data;    // Pointers to key data location
    int key_data_len, i;        // Key length
    char *plaintext;            // Pointer to plain text
    unsigned char key[32];
    unsigned char trialkey[32];
    int cipher_length, plain_length;
    

    key_data = (unsigned char *) argv[2];
    key_data_len = strlen(argv[2]);

    if (parse_args(argc, argv, &nprocs) < 0)
        exit(EXIT_FAILURE);

    // Read encrypted bytes from file
    //TODO: Make this more dynamic
    FILE *mycipherfile;
    mycipherfile = fopen (argv[3], "r");
    fseek (mycipherfile, 0, SEEK_END);
    cipher_length = ftell (mycipherfile);
    rewind (mycipherfile);
    unsigned char cipher_in[cipher_length];
    fread (cipher_in, cipher_length, 1, mycipherfile);
    fclose (mycipherfile);

    // Read decrypted bytes(to cross reference key results) from file
    FILE *myplainfile;
    myplainfile = fopen (argv[4], "r");
    fseek (myplainfile, 0, SEEK_END);
    plain_length = ftell (myplainfile);
    rewind (myplainfile);
    char plain_in[plain_length];
    fread (plain_in, plain_length, 1, myplainfile);
    fclose (myplainfile);

    int y;
    printf ("\nPlain: ");
    for (y = 0; y < plain_length; y++) {
        printf ("%c", plain_in[y]);
    }
    printf ("\n");

    printf ("Ciphertext: %s\n\n", (char *) cipher_in);

    // Condition known portion of key
    // Only use most significant 32 bytes of data if > 32 bytes
    if (key_data_len > 32)
        key_data_len = 32;

    // Copy bytes to the front of the key array
    for (i = 0; i < key_data_len; i++) {
        key[i] = key_data[i];
        trialkey[i] = key_data[i];
    }

    // If the key data < 32 bytes, pad the remaining bytes with 0s
    for (i = key_data_len; i < 32; i++) {
        key[i] = 0;
        trialkey[i] = 0;
    }

    // This code packs the last 8 individual bytes of the key into an
    // unsigned long-type variable that can be easily incremented 
    // to test key values.
    unsigned long keyLowBits = 0;
    keyLowBits = ((unsigned long) (key[24] & 0xFFFF) << 56) |
        ((unsigned long) (key[25] & 0xFFFF) << 48) |
        ((unsigned long) (key[26] & 0xFFFF) << 40) |
        ((unsigned long) (key[27] & 0xFFFF) << 32) |
        ((unsigned long) (key[28] & 0xFFFF) << 24) |
        ((unsigned long) (key[29] & 0xFFFF) << 16) |
        ((unsigned long) (key[30] & 0xFFFF) << 8) |
        ((unsigned long) (key[31] & 0xFFFF));

    int trial_key_length = 32;
    unsigned long maxSpace = 0;

    // Work out the maximum number of keys to test
    maxSpace =
        ((unsigned long) 1 << ((trial_key_length - key_data_len) * 8)) - 1;


    if (make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    }

    for (process = 0; process < nprocs;  ++process) {
        if(add_new_node(&childpid) < 0){
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE); 
        }
        // parent process
        if (childpid) break; 
    }

    if (process == 0) {
        // parent process
        message.key_number = -1;
        memcpy(message.key, trialkey, sizeof(trialkey));
        if (write(STDOUT_FILENO, &message, message_size) < 0) {
                    fprintf(stderr, "Error writing");
        }
        if (read(STDIN_FILENO, &message, message_size) == sizeof(message)) {
            if (message.key_number != -1) {
                // test key was passed correctly
                if (test_key(keyLowBits, message.key_number, &plaintext, cipher_in, 
                cipher_length, trialkey, trial_key_length) < 0) {
                    fprintf(stderr, "Couldn't initialize AES cipher\n");
                }
                // print results to user
                fprintf(stderr, "\nOK: enc/dec ok for \"%s\"\n", plaintext);
	            fprintf(stderr, "Key No.:%ld:", message.key_number);
                int y;
	            for (y = 0; y < 32; y++) {
	                fprintf (stderr, "%c", message.key[y]);
                }
	            fprintf(stderr, "\n");
                free(plaintext);
            } else {
                // no key found
                fprintf(stderr, "Key not found\n");
            }
        }
    } else {
        // worker processes

        // partition unknown permutations
        partition_start = (maxSpace / (nprocs)) * (process-1);
        if (process == nprocs)
            partition_end = maxSpace;
        else
            partition_end = (maxSpace / (nprocs)) * (process);
        
        // Iterate over total number of unknown permutations
        for (; partition_start < partition_end; ++partition_start) {
            
            if (test_key(keyLowBits, partition_start, &plaintext, cipher_in, 
            cipher_length, trialkey, trial_key_length) == - 1) {
                fprintf(stderr, "Couldn't initialize AES cipher\n");
            }

            // key found - forward message to neighbour process
            if (strncmp(plaintext, plain_in, plain_length) == 0) {
                memcpy(message.key, trialkey, sizeof(trialkey));
                message.key_number = partition_start;
                // send key to next process
                if (write(STDOUT_FILENO, &message, message_size) < 0) {
                    fprintf(stderr, "Error writing");
                }
                
                // free memory and clear buffer
                free(plaintext);
                read(STDIN_FILENO, &message, message_size);
                exit(EXIT_SUCCESS);
            }

            free(plaintext);
        }

        // key not found in worker - wait and forward message from neighbour process
        if (read(STDIN_FILENO, &message, message_size) == sizeof(message)) {
            if (write(STDOUT_FILENO, &message, message_size) < 0) {
                fprintf(stderr, "Error writing");
            }
        } 
    }

    exit(EXIT_SUCCESS);
}