/**
 * @file parallel_search_util.c
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#include <stdlib.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "parallel_search_util.h"

/*****************************************************************************
 * Function: aes_init
 * 
 * Initialises the aes encryption struct using the provided key, key length,
 * and the EVP_aes_256_cbc() mode.
 * 
 * Parameters: 
 *     unsigned char *key_data - Pointer to the Key
 *     int key_data_len - The length of the key 
 *     EVP_CIPHER_CTX *e_ctx - Pointer to the encryption device
 *     EVP_CIPHER_CTX *d_ctx - Pointer to the decryption device
 * 
 * Returns: 0 on Success
 * ***************************************************************************/
int aes_init (unsigned char *key_data, int key_data_len, 
    EVP_CIPHER_CTX * e_ctx,EVP_CIPHER_CTX * d_ctx)
{
    int i;
    unsigned char key[32], iv[32];
    
    //Some robust programming to start with
    //Only use most significant 32 bytes of data if > 32 bytes
    if (key_data_len > 32)
        key_data_len = 32;

    //In a real-word solution, the key would be filled with a random
    //stream of bytes - we are taking a shortcut because encryption
    //is not the focus of this unit.
    for (i = 0; i < key_data_len; i++) {
        key[i] = key_data[i];
        iv[i] = key_data[i];
    }
    
    //Pad out to 32 bytes if key < 32 bytes
    for (i = key_data_len; i < 32; i++) {
        key[i] = 0;
        iv[i] = 0;
    }
  
    //Create and initialize the encryption device.
    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc (), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc (), NULL, key, iv);

    return 0;
}

/*
 * Decrypt *len bytes of ciphertext
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char * aes_decrypt (EVP_CIPHER_CTX * e, 
    unsigned char *ciphertext, int *len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char *plaintext = malloc (p_len);
    EVP_DecryptInit_ex (e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate (e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex (e, plaintext + p_len, &f_len);
    return plaintext;
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @param np 
 * @return int 
 */
int parse_args(int argc,  char *argv[], int *np)
{
    if ( (argc != 5) || ((*np = atoi (argv[1])) <= 0) ) {
        fprintf(stderr, "Usage: %s <num. procs.> <partial key> <cipher file> <plain file>\n", argv[0]);
        return(-1);
    }

    return(SUCCESS);
}

/**
 * @brief 
 * 
 * @return int 
 */
int make_trivial_ring()
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(-2);

    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1))
        return(-3);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-4);

    return(SUCCESS); 
}

/**
 * @brief 
 * 
 * @param pid 
 * @return int 
 */
int add_new_node(int *pid)
{
    int fd[2];
    
    if (pipe(fd) == -1)
        return(-5);

    if ((*pid = fork()) == -1)
        return(-6);

    if(*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return(-7);

    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return(-8);

    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return(-9);

    return(SUCCESS);
}

/**
 * @brief 
 * 
 * @param keyLowBits 
 * @param testKey 
 * @param plaintext 
 * @param cipher_in 
 * @param cipher_length 
 * @param trialkey 
 * @param trial_key_length 
 * @return int 
 */
int test_key(unsigned long keyLowBits, unsigned long testKey, char** plaintext, 
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
    EVP_CIPHER_CTX *en = EVP_CIPHER_CTX_new ();
    EVP_CIPHER_CTX *de = EVP_CIPHER_CTX_new ();
    //Initialise the encryption device
    if (aes_init (trialkey, trial_key_length, en, de)) {
        return(-10);
    }
    // Test permutation of the key to see if we get the desired plain text
    *plaintext = (char *) aes_decrypt (de,
                    (unsigned char *) cipher_in,
                    &cipher_length);
    // Cleanup Cipher Allocated memory
    EVP_CIPHER_CTX_cleanup (en);
    EVP_CIPHER_CTX_cleanup (de);
    EVP_CIPHER_CTX_free (en);
    EVP_CIPHER_CTX_free (de);
    
    return(SUCCESS);
}