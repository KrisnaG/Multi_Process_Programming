/**
 * @file parallel_search_util.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Implementation of parallel_search_util.h. 
 * Utility library to support parallel search keyspace.
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <sys/wait.h>
#include "parallel_search_util.h"

/**
 * @brief Checks input parameters are the correct size and length.
 *  <num. procs.> <partial key> <cipher file> <plain file>
 * 
 * @param argc Number of input parameters
 * @param argv Input parameter list
 * @param np Number of processes
 * @return 0 on success, -1 on failure
 */
int parse_args(int argc,  char *argv[], int *np)
{
    if ((argc != ARG_LENGTH) || ((*np = atoi (argv[1])) <= 0)) {
        fprintf(stderr, 
                "Usage: %s <num. procs.> <partial key> <cipher file> <plain file>\n", 
                argv[0]);
        return(-1);
    }

    return(SUCCESS);
}

/**
 * @brief Reads the block of data from a given file and stores it into a block
 * of memory specified by buffer.
 * 
 * NOTE: buffer is created by calloc and an associated free call is required.
 * 
 * @return 0 on success, -1 on open failure, -2 on allocation failure, -3 on read failure
 */
int read_file(char *filename, int *file_length, unsigned char **buffer)
{
    FILE *file;
    
    // open file
    if ((file = fopen(filename, "r")) == NULL) {
        perror("File error");
        return(-1);
    }

    // obtain file size
    fseek(file, 0, SEEK_END);
    *file_length = ftell(file);
    rewind(file);   
    
    // allocate contiguous memory
    *buffer = calloc((*file_length+1), sizeof(unsigned char));
    if (buffer == NULL) {
        perror("Memory error");
        fclose(file);
        return(-2);
    }

    // read from file into buffer
    if (fread(*buffer, *file_length, 1, file) != 1) {
        perror("Reading error");
        fclose(file);
        return(-3);
    }

    // close file/free memory
    fclose(file);

    return(SUCCESS);
}

/**
 * @brief Set the up key data from the given input partial key.
 * 
 * @param maxSpace Maximum number of keys
 * @param keyLowBits Packed key bytes
 * @param input_key Input partial key
 * @param trialkey Key that will be trialed
 */
void setup_key(unsigned long *maxSpace, unsigned long *keyLowBits, char *input_key, 
unsigned char *trialkey) 
{
    unsigned char key[KEY_LENGTH];
    unsigned char *key_data;
    int key_data_len, i;
    
    // get partial key
    key_data = (unsigned char *) input_key;
    key_data_len = strlen(input_key);

    // Condition known portion of key
    // Only use most significant 32 bytes of data if > 32 bytes
    if (key_data_len > KEY_LENGTH)
        key_data_len = KEY_LENGTH;

    // Copy bytes to the front of the key array
    for (i = 0; i < key_data_len; i++) {
        key[i] = key_data[i];
        trialkey[i] = key_data[i];
    }

    // If the key data < 32 bytes, pad the remaining bytes with 0s
    for (i = key_data_len; i < KEY_LENGTH; i++) {
        key[i] = 0;
        trialkey[i] = 0;
    }

    // This code packs the last 8 individual bytes of the key into an
    // unsigned long-type variable that can be easily incremented 
    // to test key values.
    *keyLowBits = 0;
    *keyLowBits = ((unsigned long) (key[24] & 0xFFFF) << 56) |
        ((unsigned long) (key[25] & 0xFFFF) << 48) |
        ((unsigned long) (key[26] & 0xFFFF) << 40) |
        ((unsigned long) (key[27] & 0xFFFF) << 32) |
        ((unsigned long) (key[28] & 0xFFFF) << 24) |
        ((unsigned long) (key[29] & 0xFFFF) << 16) |
        ((unsigned long) (key[30] & 0xFFFF) << 8) |
        ((unsigned long) (key[31] & 0xFFFF));

    // Work out the maximum number of keys to test
    *maxSpace = 0;
    *maxSpace =
        ((unsigned long) 1 << ((TRIAL_LENGTH - key_data_len) * 8)) - 1;
}

/**
 * @brief Create a ring by utilising a pipe to connect the std input and output.
 * 
 * @return 0 on success, -1 on pipe failure, -2 on dup2 failure, -3 on close failure 
 */
int make_trivial_ring()
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(-1);

    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1))
        return(-2);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-3);

    return(SUCCESS); 
}

/**
 * @brief Adds a new node to a ring of processes.
 * 
 * @param pid Process ID of child process
 * @return 0 on success, -1 & -2 on pipe failure, -3 & -4 on dup2 failure, 
 * -5 on close failure  
 */
int add_new_node(int *pid)
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(-1);

    if ((*pid = fork()) == -1)
        return(-2);

    if(*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return(-3);

    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return(-4);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-5);

    return(SUCCESS);
}

/**
 * @brief Trails a key through encryption/decryption and outputs result to plaintext
 * 
 * @param keyLowBits Packed key bytes
 * @param testKey Key to test on trial
 * @param plaintext Pointer to plaintext
 * @param cipher_in Input Cipher Text
 * @param cipher_length Input Cipher Text length
 * @param trialkey Key to trial
 * @param trial_key_length Trial key length
 * @return 0 on success, -1 on failure 
 */
int test_key(unsigned long keyLowBits, unsigned long testKey, char **plaintext, 
    unsigned char* cipher_in, int cipher_length, unsigned char trialkey[], int trial_key_length) 
{
    //OR the low bits of the key with the counter to get next test key
    unsigned long trialLowBits = keyLowBits | testKey;
    
    //Unpack these bits into the end of the trial key array
    trialkey[25] = (unsigned char) (trialLowBits >> 48);
    trialkey[26] = (unsigned char) (trialLowBits >> 40);
    trialkey[27] = (unsigned char) (trialLowBits >> 32);
    trialkey[28] = (unsigned char) (trialLowBits >> 24);
    trialkey[29] = (unsigned char) (trialLowBits >> 16);
    trialkey[30] = (unsigned char) (trialLowBits >> 8);
    trialkey[31] = (unsigned char) (trialLowBits);
    
    //Set up the encryption device
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new();
    
    //Initialise the encryption device
    if (aes_init(trialkey, trial_key_length, en, de) < 0) 
        return(-1);
    
    // Test permutation of the key to see if we get the desired plain text
    *plaintext = (char *) aes_decrypt(de, (unsigned char *) cipher_in, &cipher_length);
    
    // Cleanup Cipher Allocated memory
    EVP_CIPHER_CTX_cleanup(en);
    EVP_CIPHER_CTX_cleanup(de);
    EVP_CIPHER_CTX_free(en);
    EVP_CIPHER_CTX_free(de);
    
    return(SUCCESS);
}

/**
 * @brief Initialises the aes encryption struct using the provided key, key length,
 * and the EVP_aes_256_cbc() mode.
 * 
 * @param key_data Pointer to the Key
 * @param key_data_len The length of the key 
 * @param e_ctx Pointer to the encryption device
 * @param d_ctx Pointer to the decryption device
 * @return 0 on success, -1 on failure 
 */
int aes_init (unsigned char *key_data, int key_data_len, 
    EVP_CIPHER_CTX * e_ctx,EVP_CIPHER_CTX * d_ctx)
{
    int i;
    unsigned char key[32], iv[32];
    
    // Only use most significant 32 bytes of data if > 32 bytes
    if (key_data_len > 32)
        key_data_len = 32;

    // In a real-word solution, the key would be filled with a random
    // stream of bytes - we are taking a shortcut because encryption
    // is not the focus of this unit.
    for (i = 0; i < key_data_len; i++) {
        key[i] = key_data[i];
        iv[i] = key_data[i];
    }
    
    // Pad out to 32 bytes if key < 32 bytes
    for (i = key_data_len; i < 32; i++) {
        key[i] = 0;
        iv[i] = 0;
    }
  
    // Create and initialize the encryption device.
    EVP_CIPHER_CTX_init(e_ctx);
    EVP_CIPHER_CTX_init(d_ctx);

    if (EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc (), NULL, key, iv) == 0 ||
        EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc (), NULL, key, iv) == 0) 
        return(-1);

    return(SUCCESS);
}

/**
 * @brief Decrypt *len bytes of ciphertext. All data going in & out is considered 
 * binary (unsigned char[]).
 * 
 * NOTE: plaintext is created by malloc and an associated free call is required.
 * 
 * @param e Newly created EVP_CIPHER_CTX
 * @param ciphertext Cipher text
 * @param len Cipher length
 * @return unsigned char* 
 */
unsigned char * aes_decrypt(EVP_CIPHER_CTX * e, unsigned char *ciphertext, int *len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = malloc (p_len);
    
    EVP_DecryptInit_ex (e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate (e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex (e, plaintext + p_len, &f_len);
    
    return(plaintext);
}

/**
 * @brief Prints a message of type char array out to stderr
 * 
 * @param message message to print
 * @param length length of message
 */
void print_message(unsigned char * message, int length) {
    int i;
    for (i = 0; i < length; ++i) {
        fprintf(stderr, "%c", message[i]);
    }
    fprintf(stderr, "\n");
}

/**
 * @brief Wait for a given child process to complete
 * 
 * @param childpid Child process to wait for
 * @return 0 on success, -1 no child, -2 abnormal child exit 
 */
int wait_for_child(int childpid) {
    int status;
    
    // if process has child process
    if (childpid > 0) {
        if (waitpid(childpid, &status, 0) < 0) {
            fprintf(stderr, "No child to wait for\n");
            return(-1);
        } else if (status < 0) {
            fprintf(stderr, "Child exited with an error\n");
            return(-2);
        }
    }

    return(SUCCESS);
}