#include "function_global.h"
//TODO:test
void handle_client_ans(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql)
{
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d recv ans\n",__FUNCTION__,__LINE__);
    client_info->client->need_return = 0;
    /*
    client_info->client->return_data_length = make_ans(client_info->client->sendBuf,
                                                       client_info->client->messageID,
                                                       FUNCTION_HANDLE_SUCCESS,
                                                       client_info->client->recvMessageNum);*/
}
