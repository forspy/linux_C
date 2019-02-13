
#include "encrypt.h"
#include "RSA.h"
#include "AES.h"

void decrypt(unsigned char*buf,int length,int encryptType,unsigned char* key)
{
    char* tmp = NULL;
    switch(encryptType)
    {
        case NONE:
            return;
        case TYPE_RSA:
            //此处只考虑了AES报文传送的情况，其他情况可能出错
            tmp = (char*)MALLOC(length);
            if(RSA_decrypt((char*)buf,tmp,length) == RSA_OK)
            {
                memcpy(buf,tmp+4,length-4);
            }
            FREE(tmp);
            return ;
        case TYPE_AES:
            tmp = (char*)MALLOC(length);
            if(AES_decrypt(buf,(unsigned char*)tmp,length,key) == AES_OK)
            {
                memcpy(buf,tmp,length);
            }
            FREE(tmp);
            return ;
        default:
            return ;
    }
}