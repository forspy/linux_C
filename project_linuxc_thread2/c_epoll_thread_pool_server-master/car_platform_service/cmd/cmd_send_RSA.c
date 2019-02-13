#include "cmd.h"


//TODO
void cmd_send_RSA(unsigned char* inputbuf,int length, Client_hash_node* client_info)
{
    
}


static unsigned char* RSA_DATA=NULL;

extern RSA *ClientRsa;


unsigned char getIntFromHex(unsigned char num)
{
    unsigned char tmp;
    tmp = num - '0';
    if(tmp > 9)
    {
        tmp = num - 'A' + 10;
    }
    if(tmp > 15)
    {
        tmp = num - 'a' + 10;
    }
    return tmp;
}

void convertStringToBuff(unsigned char* inputString,unsigned char* buf,int bufSize)
{
    int length;
    int i;
    int j;
    unsigned char num;
    length = strlen((const char*)inputString);
    memset(buf,0,bufSize);
    for(i=bufSize-1,j=length-1;i>=0;i--)
    {
        if(j==0)
        {
            num = getIntFromHex(inputString[j]);
        }
        else
        {
            num = getIntFromHex(inputString[j]) + (getIntFromHex(inputString[j-1])<<4);
        }
        buf[i] = num;
        j-=2;
        if(j<0)
        {
            break;
        }
    }
}

void make_RSA_data(unsigned char* buf)
{
    PROTOCOL_RSA_DATA* data;
    unsigned char* tmp;
    data =(PROTOCOL_RSA_DATA*) buf;
    if(ClientRsa == NULL)
    {
        init_RSA();
    }
    if (RSA_DATA)
    {
        memcpy(buf,RSA_DATA,sizeof(PROTOCOL_RSA_DATA));
        return ;
    }
    /*
    data->e = BN_bn2hex(ClientRsa->e);
    data->n = BN_bn2hex(ClientRsa->n);
    OPENSSL_free(data->e);
    OPENSSL_free(data->n);
    */
    tmp = (unsigned char*)BN_bn2hex(ClientRsa->e);
    convertStringToBuff(tmp,(unsigned char*)&(data->e),sizeof(int));
    OPENSSL_free(tmp);
    
    tmp = (unsigned char*)BN_bn2hex(ClientRsa->n);
    convertStringToBuff(tmp,(unsigned char*)&(data->n),RSA_KEY_BYTE_LENGTH);
    OPENSSL_free(tmp);

    RSA_DATA =(unsigned char*) MALLOC(sizeof(PROTOCOL_RSA_DATA));
    memcpy(RSA_DATA,buf,sizeof(PROTOCOL_RSA_DATA));
}