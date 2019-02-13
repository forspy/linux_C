#ifndef _AES_H_
#define _AES_H_

#include "global.h"

#define AES_BLOCK_SIZE 16
#define AES_OK 0
#define AES_KEY_LENGTH 16
#define AES_KEY_W_LENGTH 176

int AES_encrypt(unsigned char* in, unsigned char* out,int length,unsigned char* w);
int AES_decrypt(unsigned char* in, unsigned char* out,int length,unsigned char* w);
void key_expansion(uint8_t *key, uint8_t *w, int key_length);

#endif
