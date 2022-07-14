/**
 * @file aes_util.h
 * @author Krisna Gusti (kgusti@myune.edu.au)
 * @brief 
 */

#ifndef AES_UTIL_H
#define AES_UTIL_H

int aes_init(unsigned char *key_data, int key_data_len, 
    EVP_CIPHER_CTX * e_ctx, EVP_CIPHER_CTX * d_ctx);

unsigned char * aes_decrypt(EVP_CIPHER_CTX * e, 
    unsigned char *ciphertext, int *len);

#endif