#include "RSA.h"

RSA *ClientRsa=NULL;

static int bits = 1024;
#define RSA_ENCRYPT_BLOCK_SIZE 140

void init_RSA()
{
    if(ClientRsa)
    {
        RSA_free(ClientRsa);
    }
    ClientRsa=RSA_generate_key(bits,PUBLIC_EXPONENT,NULL,NULL);
    RSA_print_fp(stdout, ClientRsa, 5);
}

int RSA_decrypt(char* in, char* out,int length)
{
    int i;
    char* buf;
    int flen;
    int block_num;


    
    flen = RSA_size(ClientRsa);
//    flen = RSA_ENCRYPT_BLOCK_SIZE;
    
    block_num = (length-1)/flen + 1;
    buf = (char*)MALLOC(flen);
    
    for(i=0;i<length/flen;i++)
    {
        if(RSA_private_decrypt(flen,(const unsigned char*)(in+i*flen),(unsigned char*)buf,ClientRsa,RSA_NO_PADDING)<0)
        {
            FREE(buf);
            return RSA_ERROR;
        }
        memcpy(out+i*RSA_BLOCK_SIZE,buf,RSA_BLOCK_SIZE);
    }
    FREE(buf);
    
    return RSA_OK; 
}

int RSA_encrypt(char* in, char* out,int length)
{
    int i;
    char* buf;
    int flen;
    int block_num;
    flen = RSA_size(ClientRsa);

    block_num = (length-1)/RSA_BLOCK_SIZE + 1;

    buf = (char*)MALLOC(flen);
    for(i=0;i<block_num;i++)
    {
//        memcpy(buf,in+i*RSA_BLOCK_SIZE,RSA_BLOCK_SIZE);
        if(RSA_public_encrypt(flen,(const unsigned char*)(in+i*RSA_BLOCK_SIZE),(unsigned char*)buf,ClientRsa,RSA_NO_PADDING)<0)
        {
            FREE(buf);
            return RSA_ERROR;
        }
        memcpy(out+i*flen,buf,flen);
    }
    FREE(buf);
    return block_num*flen;
}
