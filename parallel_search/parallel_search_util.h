#ifndef PARALLEL_SEARCH_UTIL_H
#define PARALLEL_SEARCH_UTIL_H

/**
 * @file parallel_search_util.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Utility library to support parallel search keyspace.
 */

/* Successful operation */
#define SUCCESS 0

/* Message structure to pass between processes */
typedef struct {
    unsigned char key[32];
    long key_number;
} message_t;

/* Parallel search keyspace utility functions */
int aes_init(unsigned char *key_data, int key_data_len, 
    EVP_CIPHER_CTX * e_ctx, EVP_CIPHER_CTX * d_ctx);
unsigned char * aes_decrypt(EVP_CIPHER_CTX * e, 
    unsigned char *ciphertext, int *len);
int parse_args(int argc,  char *argv[], int *np);
int make_trivial_ring();
int add_new_node(int *pid);
int test_key(unsigned long keyLowBits, unsigned long testKey, char** plaintext, 
    unsigned char* cipher_in, int cipher_length, unsigned char trialkey[], int trial_key_length);
int read_file(char *filename, int *file_length, unsigned char **buff);

#endif