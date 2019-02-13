#include "car_protocol.h"
#include <stdio.h>
#include <unistd.h>


typedef struct _car_platform_function_map{
    unsigned short MessageID;
    void (*FuncP) (unsigned char*,int, Client_hash_node*,SQL_STRUCT*);  //inputbuf, inputbuflen, clients
    unsigned int acceptStatus[MAX_STATUS];
    int returnMsgFlag;                                      //当来报文后，若存在控制端口，此报文是否需要回复给控制端口
}car_platform_function_map;

static car_platform_function_map function_map[] = {
    {0x0000,handle_client_aes,
        {CONNECTED_STATUS,LOGINED_STATUS,NULL_STATUS},0},

/*以下暂不实现，因为写这个程序的时候还不需要这些功能*/
    {0,NULL},
};


int handle_pkt(protocol_pkt* pkt,Client_hash_node* client_info,SQL_STRUCT* current_sql)
{
    int i;
/*
    if( pkt->head->messageID == CAR_PLATFORM_PROTOCOL_CLIENT_HEARTBEAT || 
        pkt->head->messageID == CAR_PLATFORM_PROTOCOL_CLIENT_ANS)
    {
        return;
    }*/
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)(pkt->head.property));
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)MAX_PAYLOAD_LENGTH);
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)((pkt->head.property)&MAX_PAYLOAD_LENGTH));

    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)sizeof(struct _protocol_head));
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %02x\n",__FUNCTION__,__LINE__,(int)pkt->head.messageID);
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)pkt->head.currentPktNum);
    
    threadprintbuf((unsigned char*)pkt->msg,(int)((pkt->head.property)&MAX_PAYLOAD_LENGTH),current_sql);


    //数据报文过大
    if((MAX_MESSAGE_LENGTH/MAX_RECV_BUF) < pkt->head.totalPktNum)
    {
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"%s %d too much data, handle error, please FIX!\n",__FUNCTION__,__LINE__);
        return ERROR_STATUS;
    }

    memcpy(client_info->client->recvData + ((pkt->head.currentPktNum - 1) * MAX_PAYLOAD_LENGTH),pkt->msg,((pkt->head.property)&MAX_PAYLOAD_LENGTH));
        
    if(pkt->head.currentPktNum == 1)
    {
        client_info->client->messageID = pkt->head.messageID;
        client_info->client->encryptType = (pkt->head.property>>10)&0x07;
        client_info->client->recvMessageNum = pkt->head.messageNum;
        if(client_info->client->identifyed == 0)
        {
            memcpy(client_info->client->identifyID,pkt->head.ID,PUBLIC_ID_LENGTH);
        }
    }
    if(pkt->head.currentPktNum == pkt->head.totalPktNum)
    {
        
        client_info->client->recvDataLength = ((pkt->head.totalPktNum - 1) * MAX_PAYLOAD_LENGTH) + ((pkt->head.property)&MAX_PAYLOAD_LENGTH);
        
        decrypt((unsigned char*)client_info->client->recvData,
                client_info->client->recvDataLength,
                client_info->client->encryptType,
                client_info->client->AESKeyW);
        i=0;

        threadprintbuf((unsigned char*)client_info->client->recvData,client_info->client->recvDataLength,current_sql);
        
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)client_info->client->recvDataLength);
        while(function_map[i].FuncP)
        {
            if(function_map[i].MessageID == client_info->client->messageID)
            {
                THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)client_info->client->recvDataLength);
                client_info->client->need_return = 0;
                client_info->client->return_data_length = 0;
                client_info->client->return_encrypt_type = TYPE_AES;
                
                THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)client_info->client->recvDataLength);
                function_map[i].FuncP((unsigned char*)client_info->client->recvData,
                                      client_info->client->recvDataLength,
                                      client_info,current_sql);
                THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,(int)client_info->client->recvDataLength);

                if(function_map[i].returnMsgFlag && client_info->client->controlSocket)
                {
                    unsigned char returnHead[12];
                    unsigned int length;
                    memcpy(returnHead,client_info->client->identifyID,6);
                    returnHead[6]=0;
                    returnHead[7]=1;
                    length = htonl(client_info->client->recvDataLength);

                    memcpy(returnHead+8,&length,sizeof(int));
                    
                    send(client_info->client->controlSocket,
                         returnHead,
                         12,0);
                    
                    send(client_info->client->controlSocket,
                         client_info->client->recvData,
                         client_info->client->recvDataLength,0);
                    close(client_info->client->controlSocket);
                    client_info->client->controlSocket = 0;
                }
                
                return PKT_HANDLED;
            }
            else
            {
                i++;
            }
        
        }
        
        
        client_info->client->need_return = 1;
        
        client_info->client->return_type = 0x0000;
        client_info->client->return_data_length = make_ans((unsigned char*)client_info->client->sendBuf,
                                                           client_info->client->messageID,
                                                           FUNCTION_HANDLE_NOT_SUPPORT,
                                                           client_info->client->recvMessageNum);
        
        return PKT_HANDLED;
    }
    return WAIT_NEXT_PKT;
}
