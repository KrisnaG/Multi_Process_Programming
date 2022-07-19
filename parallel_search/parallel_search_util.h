#ifndef PARALLEL_SEARCH_UTIL_H
#define PARALLEL_SEARCH_UTIL_H

/**
 * @file parallel_search_util.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief Utility library to support parallel search keyspace.
 */

/* Successful operation */
#define SUCCESS 0

#define ARG_LENGTH 5

/* Maximum key length */
#define KEY_LENGTH 32
#define TRIAL_LENGTH 32

/* Message structure to pass between processes */
typedef struct {
    unsigned char key[KEY_LENGTH];
    long key_number;
} message_t;

/* Parallel search keyspace utility functions */
int parse_args(int, char *[], int *);
int read_file(char *, int *, unsigned char **);
void setup_key(unsigned long *, unsigned long *, char *, unsigned char *);
int make_trivial_ring();
int add_new_node(int *);
int test_key(unsigned long, unsigned long, char **, unsigned char *, int,
            unsigned char [], int);
int aes_init(unsigned char *, int, EVP_CIPHER_CTX *, EVP_CIPHER_CTX *);
unsigned char * aes_decrypt(EVP_CIPHER_CTX *, unsigned char *, int *);
void print_message(unsigned char *, int);
int wait_for_child(int);

#endif