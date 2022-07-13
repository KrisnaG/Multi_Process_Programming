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
#include "ring_util.h"

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @param np 
 * @return int 
 */
int parse_args(int argc,  char *argv[ ], int *np)
{
    if ( (argc != 5) || ((*np = atoi (argv[1])) <= 0) ) {
        fprintf(stderr, "Usage: %s <num. procs.> <partial key> <cipher file> <plain file>\n", argv[0]);
        return(-1);
    }

    return(0);
}

/**
 * @brief 
 * 
 * @return int 
 */
int read_file(char *filename, int *file_length, unsigned char *buff)
{
    FILE *file;
    
    if ((file = fopen(filename, "r")) == NULL) {
        perror("File error");
        return(-1);
    }

    // obtain file size
    fseek(file, 0, SEEK_END);
    *file_length = ftell(file);
    rewind(file);   
    
    // allocate memory
    buff = (unsigned char*) malloc (sizeof(unsigned char)*(*file_length));
    if (buff == NULL) {
        perror("Memory error");
        return(-1);
    }

    if (fread(buff, *file_length, 1, file) != 1) {
        perror("Reading error");
        return(-1);
    }

    // free memory
    fclose(file);

    return(0);
}



/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc,  char *argv[ ])
{
    /* process related variables */
    int childpid;               // indicates process should spawn another
    int nprocs;                 // total number of processes in ring
    int process;
    unsigned long partition_start;
    unsigned long partition_end;

    /* encryption/decryption related variables */
    unsigned char *key_data;    // Pointers to key data location
    int key_data_len, i;        // Key length
    char *plaintext;            // Pointer to plain text
    unsigned char key[32];
    unsigned char trialkey[32];
    int cipher_length, plain_length;
    

    key_data = (unsigned char *) argv[1];
    key_data_len = strlen(argv[1]);

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

    printf(": %ld\n", maxSpace);

    if (make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    }

    for (process = 1; process < nprocs;  ++process) {
        if(add_new_node(&childpid) < 0){
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE); 
        }
        
        // parent process
        if (childpid) break; 
    }
    
    /* ring process code  */

    if (childpid == 0) {
        partition_start = (maxSpace / nprocs) * process-1;
        
        if (process = nprocs)
            partition_end = (maxSpace / nprocs) * process;
        else
            partition_end = maxSpace;
        
        for (partition_start; partition_start < partition_end; ++partition_start) {
            //OR the low bits of the key with the counter to get next test key
            unsigned long trialLowBits = keyLowBits | partition_start;
            //Unpack these bits into the end of the trial key array
            trialkey[25] = (unsigned char) (trialLowBits >> 48);
            trialkey[26] = (unsigned char) (trialLowBits >> 40);
            trialkey[27] = (unsigned char) (trialLowBits >> 32);
            trialkey[28] = (unsigned char) (trialLowBits >> 24);
            trialkey[29] = (unsigned char) (trialLowBits >> 16);
            trialkey[30] = (unsigned char) (trialLowBits >> 8);
            trialkey[31] = (unsigned char) (trialLowBits);

            //Set up the encryption device
            EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new ();
            EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new ();

            //Initialise the encryption device
            if (aes_init (trialkey, trial_key_length, en, de)) {
	            printf ("Couldn't initialize AES cipher\n");
	            return -1;
	        }

            // Test permutation of the key to see if we get the desired plain text
            plaintext = (char *) aes_decrypt (de,
					        (unsigned char *) cipher_in,
					        &cipher_length);

            // Cleanup Cipher Allocated memory
            EVP_CIPHER_CTX_cleanup (en);
            EVP_CIPHER_CTX_cleanup (de);
            EVP_CIPHER_CTX_free (en);
            EVP_CIPHER_CTX_free (de);

            //Key match checking
            //Hint - If key is found(decrypted string matches) return 
            //value back to parent
            if (strncmp (plaintext, plain_in, plain_length) == 0) {
	            printf ("\nOK: enc/dec ok for \"%s\"\n", plaintext);
	            printf ("Key No.:%ld:", partition_start);

	            //Hint - Won't print here(print in parent process)
	            int y;
	            for (y = 0; y < 32; y++) {
	                  printf ("%c", trialkey[y]);
	            }
                printf ("\n");
	            break;
	        }

            free (plaintext);
        /*
         Hint - In child processes make sure to release allocated memory and 
         if key found return key to parent.
         Lastly have parent display results.
        */

        }
    }

    exit(EXIT_SUCCESS);
}