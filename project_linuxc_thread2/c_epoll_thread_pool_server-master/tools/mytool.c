#include "mytool.h"
#include "stdio.h"

char* convertBufToHex(unsigned char* inputbuf,unsigned char* outhex,int length)
{
    int i;
    for(i=0;i<length;i++)
    {
        sprintf((char*)(outhex+i*2),"%02X",inputbuf[i]);
    }
    outhex[length*2]=0;
    return (char*)outhex;
}
/*
void convertHexToBuf(unsigned char* inputhex,unsigned char* outbuf,int length)
{
    int i;
    for(i=0;i<length;i++)
    {
//        sscanf(inputhex,"%02X",);
//        sprintf(outhex+i*2,"%02X",inputbuf[i]);
    }
    outhex[length*2]=0;
}*/