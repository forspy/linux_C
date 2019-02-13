#include "client.h"
#include "cmd.h"
#include "epoll.h"

extern int total_num;
extern Client_hash_node* logined_clients;
//foreach命令不带回复
void foreachClient(unsigned char* inputbuf,int length,unsigned short messageID)
{
    int i;
    Client_hash_node* tmp;
    
    for(i=0;i<total_num;i++)
    {
        tmp = &logined_clients[i];
        while(tmp)
        {
            if(tmp->client)
            {
                //考虑到foreach的时候会发送大量数据，因此不带加密
                send_encrypted_data(inputbuf,length,NONE,messageID,tmp->client);
            }
            tmp = tmp->next_login_table;
        }
    }
}

void cmd_trans_data(unsigned char* inputbuf,int length,unsigned short protocol, Client_hash_node* client_info)
{
    int fd;
    Client* client;
    char tmpbuf[MAX_SEND_BUF*10];
    int final_length;
    int* tmp;

    DEBUGPRINT;
    
    if(client_info == NULL)
    {
        DEBUGPRINT;
        foreachClient(inputbuf,length,protocol);
        return;
    }

    if(!(client_info->client))
    {
        printf("%s %d error client_info\n",__FUNCTION__,__LINE__);
        return ;
    }
    fd = client_info->client->fd;

    client = client_info->client;
    
//#if !ENCRYPTDATA
    client->return_encrypt_type = NONE;
//#endif
    
    switch(client->return_encrypt_type)
    {
        case NONE:
            //send_final_pkt(fd,client->sendBuf,client->return_data_length,client);
            send_encrypted_data(inputbuf,
                                length,
                                client->return_encrypt_type,
                                protocol,
                                client);
            break;
        case TYPE_RSA:
            /*
            tmp = (int*)(inputbuf - sizeof(int));
            *tmp = htonl(length);
            final_length = RSA_encrypt((char*)tmp,tmpbuf,length+sizeof(int));
            if(final_length == RSA_ERROR)
            {
                close(fd);
                rm_client_hash_node_by_fd(fd);
            }*/
            send_encrypted_data(inputbuf,
                                length,
                                NONE,
                                protocol,
                                client);
            break;
        case TYPE_AES:
            
        key_expansion(client->AESKey,
                      client->AESKeyW,
                      16);
            AES_encrypt(inputbuf,(unsigned char*)tmpbuf,length,client->AESKeyW);
            send_encrypted_data((unsigned char*)tmpbuf,
                                final_length,
                                client->return_encrypt_type,
                                protocol,
                                client);
            break;
    }
    
}