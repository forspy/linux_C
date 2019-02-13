#ifndef _RSA_H_
#define _RSA_H_

#include <openssl/rsa.h>
#include <openssl/err.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "global.h"

#define PUBLIC_EXPONENT RSA_F4          //0x10001
#define RSA_ERROR -1
#define RSA_OK 0

#define RSA_BLOCK_SIZE 100

int RSA_encrypt(char* in, char* out,int length);
int RSA_decrypt(char* in, char* out,int length);
void init_RSA();

#endif
