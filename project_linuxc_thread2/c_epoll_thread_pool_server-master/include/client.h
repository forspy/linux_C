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
	char recvBuf[MAX_RECV_BUF*3];		//����δ�����������
	int dataLength;					//δ���������������Ч���ݳ���
	
	char tmp[32];                       //��ʱ������䣬�����sendBuf��ǰһ����
	char sendBuf[MAX_MESSAGE_LENGTH];		//������Ҫ���͵�����
	
	char recvData[MAX_MESSAGE_LENGTH];		//Э�������֮���������
	unsigned int recvDataLength;
	
	unsigned char AESKey[AES_KEY_LENGTH * 2];
	unsigned char AESKeyW[AES_KEY_W_LENGTH * 2];
//	struct sockaddr in_addr;			//�ͻ���ip��ַ
    int fd;                             //socket fd
                                        //�رմ˽ӿڵ�time wait
    
    int status;                         //��¼״̬(�Ǹ����ӻ����Ѿ���֤ͨ��)
    
    unsigned short messageID;                       //��ǰ���ܵ�messageID
    unsigned short encryptType;                     //���ܷ�ʽ
    unsigned char identifyID[PUBLIC_ID_LENGTH];     //ʶ��ID
    int identifyed;                                 //�Ƿ��Ѿ���֤��id�Ϸ��ԣ���֤�����޷��޸�ʶ��id
    unsigned char identifyIDString[PUBLIC_ID_LENGTH*2+1];   //ʶ��IDת��Ϊ�������ַ���

    unsigned int carID;                             //����ID/�豸ID
    
    unsigned short pktNum;                          //��Ҫ���͵ڼ�����Ϣ

    unsigned short recvMessageNum;                  //�Զ˷��͵Ĵ��������ǵڼ�����Ϣ(���ܶ��������һ����Ϣ)

    unsigned int need_return;           //�Ƿ���Ҫ�ظ�����
    int return_data_length;             //�ظ����ݳ���
    unsigned int return_encrypt_type;   //�ظ����ݵļ��ܷ�ʽ
    unsigned int return_type;
    
    unsigned short wait_messageID;      //ͨ�������͵������Ҫ�������messageID������
    unsigned short wait_sub_messageID;  //�����Ҫ����ans���ĵ����ݣ�����Ҫ���������ans����
    unsigned short wait_fd;             //���ص�����Ӧ�÷��͵��ĸ��˿�

    int controlSocket;                  //��������ͽӿڣ������socket���Է������ݣ����յ����ϴ����ݺ���Ҫ���˽ӿ��ٷ���һ������
                                        //�˽ӿڵ�timewait���ɹر�

    time_t now;                         //��ǰʱ�䣬���ɿ�������
    long time;                          //��ʱʱ�䣬������ʱ����������ر����ӣ�����

    int in_thread_flag;                 //�жϵ�ǰ�ӿ��Ƿ������̳߳��б�����

    int client_over;                    //0:ûӰ��,1:�رյ�ǰ����

    char connectInfo[2048];             //�Զ�������Ϣ��������

    
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
