#include "function_global.h"
//TODO:test
void handle_client_aes(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql)
{
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d recv aes data %d\n",__FUNCTION__,__LINE__,length);
    if(length < AES_KEY_LENGTH)
    {
        client_info->client->need_return = 1;
        client_info->client->return_type = 0x0000;
        client_info->client->return_data_length = make_ans((unsigned char*)client_info->client->sendBuf,
                                                           client_info->client->messageID,
                                                           FUNCTION_HANDLE_ERROR,
                                                           client_info->client->recvMessageNum);
        return ;
    }
    else
    {
        memcpy(client_info->client->AESKey,inputbuf,AES_KEY_LENGTH);

        THREADDEBUGPRINT;
        threadprintbuf(client_info->client->AESKey,AES_KEY_LENGTH,current_sql);
        
        key_expansion(client_info->client->AESKey,
                      client_info->client->AESKeyW,
                      16);
        client_info->client->need_return = 1;
        client_info->client->return_type = 0x0000;
        client_info->client->return_data_length = make_ans((unsigned char*)client_info->client->sendBuf,
                                                           client_info->client->messageID,
                                                           FUNCTION_HANDLE_SUCCESS,
                                                           client_info->client->recvMessageNum);
    }
}