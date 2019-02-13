#include "global.h"
#include <netinet/in.h>
#include <errno.h>
#include "client.h"
#include "arpa/inet.h"
#include <stdio.h>
#include <unistd.h>
#include "mytool.h"
#include "cmd.h"

int cmd_port = 30001;

#define CTRL_CMD_ERROR_OFF_LINE 0xFFFF
#define CTRL_CMD_ERROR_PERMISSION_DENY 0xFFFE
#define CTRL_CMD_ERROR_CMD_ERROR 0xFFFD
STRING_LIST accept_ctrl_ip;

enum service_cmd {
    SERVICE_CMD_UPDATA_UPGRADE_INFO = 0x00A0,
    SERVICE_CMD_BROADCAST_RANGE = 0x00A1,
    SERVICE_CMD_BROADCAST_DEVICES = 0x00A2,
};

static int  
create_listen_socket ()  
{
    int    socket_fd;
    struct sockaddr_in servaddr;

    if( (socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"create socket error: %s(errno: %d)\n",strerror(errno),errno);  
        exit(0);  
    }  
    //初始化
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址设置成INADDR_ANY,让系统自动获取本机的IP地址。
    servaddr.sin_port = htons(cmd_port);//设置的端口为DEFAULT_PORT

    if( bind(socket_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    if( listen(socket_fd, 10) == -1){
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    return socket_fd;
}  
  
int acceptIp(int ip)
{
    int submaskLength;
    int submask;
    unsigned char acceptIP[100];
    STRING_LIST* tmp;
    
    if((ip& (0xff000000)) == ((ntohl(inet_addr("127.0.0.1")) & (0xff000000))))
    {
        return 1;
    }

    tmp = &accept_ctrl_ip;

    return 1;

    while(tmp)
    {
        submaskLength = 32;
        if(tmp->string)
        {
            sscanf(tmp->string,"%s %d",acceptIP,&submaskLength);
        }
        else
        {
            tmp = tmp->next;
            continue;
        }

        submask = 0xFFFFFFFF << (32-submaskLength);

        if(inet_addr((const char*)acceptIP) == -1)
        {
            return 1;
        }
        if((ip& (submask)) == ((ntohl(inet_addr((const char*)acceptIP)) & (submask))))
        {
            return 1;
        }
        tmp = tmp->next;
    }

    return 0;
}

extern int should_upgrade_upgrade_info;
extern SQL_STRUCT main_sql;

//下发命令给特定client，不过这个实现就看看就好
void *cmd_listen_thread(void *arg)
{
    int fd;
    int infd;
    int in_len;
    int recv_len;
    int write_len;
    int i;
    struct sockaddr_in in_addr;
    unsigned char inputBuf[MAX_RECV_BUF_LENGTH];
    unsigned char pubIDString[PUBLIC_ID_LENGTH*2+1];
    unsigned char pubID[PUBLIC_ID_LENGTH];
    Client_hash_node* node;
    CMD_STRUCT* cmd;

    CMD_STRUCT error_return;

    const unsigned char all_client_ID[PUBLIC_ID_LENGTH]={0xFF,0XFF,0XFF,0XFF,0XFF,0XFF};
    const unsigned char server_ID[PUBLIC_ID_LENGTH]={0x00,0x00,0x00,0x00,0x00,0x00};

    fd = create_listen_socket();
    if(fd<0)
    {
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"Listen socket create failed, exit\n");
        exit(-1);
    }

    cmd =(CMD_STRUCT*) &inputBuf[0];

    while(1)
    {
        in_len = sizeof(in_addr);
        infd = accept (fd, (struct sockaddr *)&in_addr, (socklen_t*)&in_len);


        if(!acceptIp(ntohl(in_addr.sin_addr.s_addr)))
        {
            DEBUGPRINT;
            
            memcpy(error_return.pubID,cmd->pubID,PUBLIC_ID_LENGTH);
            error_return.messageID = htons(CTRL_CMD_ERROR_PERMISSION_DENY);
            error_return.length = htonl(0);
            write_len = write(infd,&error_return,sizeof(CMD_STRUCT));
            
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"%s %d %08X\n",__FUNCTION__,__LINE__,in_addr.sin_addr.s_addr);
            close(infd);
            continue;
        }
        
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %08X\n",__FUNCTION__,__LINE__,in_addr.sin_addr.s_addr);
        recv_len = read(infd,inputBuf,MAX_RECV_BUF_LENGTH);

        if(recv_len<sizeof(CMD_STRUCT))
        {
            DEBUGPRINT;
            
            memcpy(error_return.pubID,cmd->pubID,PUBLIC_ID_LENGTH);
            error_return.messageID = htons(CTRL_CMD_ERROR_CMD_ERROR);
            error_return.length = htonl(0);
            write_len = write(infd,&error_return,sizeof(CMD_STRUCT));
            
            close(infd);
            continue;
        }

            DEBUGPRINT;
        cmd->messageID = ntohs(cmd->messageID);
        cmd->length = ntohl(cmd->length);

        if(recv_len < (sizeof(CMD_STRUCT)+cmd->length))
        {
            DEBUGPRINT;
            
            memcpy(error_return.pubID,cmd->pubID,PUBLIC_ID_LENGTH);
            error_return.messageID = htons(CTRL_CMD_ERROR_CMD_ERROR);
            error_return.length = htonl(0);
            write_len = write(infd,&error_return,sizeof(CMD_STRUCT));
            
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d %d %d \n",
                __FUNCTION__,__LINE__,recv_len,(int)sizeof(CMD_STRUCT),cmd->length);
            close(infd);
            continue;
        }

        memcpy(pubID,cmd->pubID,PUBLIC_ID_LENGTH);
        
        convertBufToHex(pubID,pubIDString,PUBLIC_ID_LENGTH);
        node = NULL;

        if(memcmp(pubID,all_client_ID,PUBLIC_ID_LENGTH) == 0)
        {
            node = NULL;
            close(infd);
        }
        else if(memcmp(pubID,server_ID,PUBLIC_ID_LENGTH) == 0) {
            if(cmd->messageID == SERVICE_CMD_UPDATA_UPGRADE_INFO)
            {
                
                DEBUGPRINT;
                should_upgrade_upgrade_info = 1;
            }
            close(infd);
            continue;
        }
        else
        {
            node = get_client_hash_node_in_logined_clients(pubID,&main_sql);
            if(!node)
            {
                memcpy(error_return.pubID,cmd->pubID,PUBLIC_ID_LENGTH);
                error_return.messageID = htons(CTRL_CMD_ERROR_OFF_LINE);
                error_return.length = htonl(0);
                write_len = write(infd,&error_return,sizeof(CMD_STRUCT));
                
                close(infd);
                continue;
            }
        }

            DEBUGPRINT;
        if(node)
        {
            if(node->client->controlSocket!=0)
            {
                DEBUGPRINT;
                close(infd);
                continue;
            }
            else
            {
                DEBUGPRINT;
                node->client->controlSocket = infd;
            }
        }

            DEBUGPRINTF("send ctrl cmd\n");
        cmd_trans_data(cmd->data,cmd->length,cmd->messageID,node);
    }
    
    return NULL;
}
