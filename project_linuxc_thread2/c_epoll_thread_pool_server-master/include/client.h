#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "AES.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "global.h"
#include <pthread.h>


extern pthread_mutex_t hash_node_lock;

enum {
    NULL_STATUS = 0,
    CONNECTED_STATUS = 1,           //just connected, need to identify it
    LOGINED_STATUS = 2,      //already logined, can exec anything
    ERROR_STATUS = 3,
    MAX_STATUS
};

enum ENCRYPT_TYPE {
    NONE = 0,
    TYPE_RSA = 1,
    TYPE_AES = 2,
};

typedef struct _Client{
	char recvBuf[MAX_RECV_BUF*3];		//所有未处理的数据流
	int dataLength;					//未处理的数据流中有效数据长度
	
	char tmp[32];                       //临时数据填充，填充在sendBuf的前一部分
	char sendBuf[MAX_MESSAGE_LENGTH];		//所有需要发送的数据
	
	char recvData[MAX_MESSAGE_LENGTH];		//协议解析完之后的数据流
	unsigned int recvDataLength;
	
	unsigned char AESKey[AES_KEY_LENGTH * 2];
	unsigned char AESKeyW[AES_KEY_W_LENGTH * 2];
//	struct sockaddr in_addr;			//客户端ip地址
    int fd;                             //socket fd
                                        //关闭此接口的time wait
    
    int status;                         //登录状态(是刚连接还是已经验证通过)
    
    unsigned short messageID;                       //当前功能的messageID
    unsigned short encryptType;                     //加密方式
    unsigned char identifyID[PUBLIC_ID_LENGTH];     //识别ID
    int identifyed;                                 //是否已经认证过id合法性，认证过后无法修改识别id
    unsigned char identifyIDString[PUBLIC_ID_LENGTH*2+1];   //识别ID转换为二进制字符串

    unsigned int carID;                             //汽车ID/设备ID
    
    unsigned short pktNum;                          //将要发送第几个消息

    unsigned short recvMessageNum;                  //对端发送的此条命令是第几个消息(可能多个包共享一个消息)

    unsigned int need_return;           //是否需要回复数据
    int return_data_length;             //回复数据长度
    unsigned int return_encrypt_type;   //回复数据的加密方式
    unsigned int return_type;
    
    unsigned short wait_messageID;      //通过程序发送的命令，需要获得哪条messageID的数据
    unsigned short wait_sub_messageID;  //如果需要返回ans报文的数据，则需要哪条命令的ans报文
    unsigned short wait_fd;             //返回的数据应该发送到哪个端口

    int controlSocket;                  //控制命令发送接口，如果此socket可以发送数据，则收到非上传数据后需要往此接口再发送一次数据
                                        //此接口的timewait不可关闭

    time_t now;                         //当前时间，不可靠，秒数
    long time;                          //超时时间，超过此时间无数据则关闭连接，秒数

    int in_thread_flag;                 //判断当前接口是否正常线程池中被处理

    int client_over;                    //0:没影响,1:关闭当前连接

    char connectInfo[2048];             //对端连接信息，调试用

    
}Client;


typedef struct _Client_hash_node{
	struct _Client* client;
	struct _Client_hash_node *prev;
	struct _Client_hash_node *next;
	struct _Client_hash_node *prev_login_table;
	struct _Client_hash_node *next_login_table;
}Client_hash_node;

int rm_client_hash_node_by_fd(int fd);
Client_hash_node* get_client_hash_node_by_fd(int fd);
Client_hash_node* get_client_hash_node_in_logined_clients(unsigned char* pubID,SQL_STRUCT* current_sql);
int add_client_hash_node_to_logined_clients(Client_hash_node* node,SQL_STRUCT* current_sql);
int add_fd_to_client_hash(int fd,time_t seconds);
int clients_hash_init(int number);
void for_every_client_connected(void (*func) (Client *));

#endif /*_CLIENT_H_*/
