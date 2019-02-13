#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "debug.h"
#include "mysql.h"

#define DEBUG_MEM 1

#if DEBUG_MEM
void* my_debug_malloc(size_t size);
void my_debug_free(void* point);
#define MALLOC my_debug_malloc
#define FREE my_debug_free
#else
#define MALLOC malloc
#define FREE free
#endif


extern char AppKey[100];
extern char AppSecret[1000];

#define MAX_RECV_BUF (1024+20)

#define MAX_MESSAGE_LENGTH (MAX_RECV_BUF*10)

#define MAX_SEND_BUF (1024+20)
#define PUBLIC_ID_LENGTH (6)
#define PRIVATE_ID_LENGTH (12)
#define MAX_PAYLOAD_LENGTH 1023
#define MD5_BUF_LEN 16
#define MD5_STR_LEN (MD5_BUF_LEN*2)
#define USER_NAME_LENGTH 16
#define PLATE_LENGTH 16
#define BLUE_TOOTH_UUID_LENGTH 40

#define PASSWORD_LENGTH MD5_STR_LEN
#define IMEI_LENGTH 16
#define SIM_NUM_LENGTH 20
#define SERVER_PORT 30002
#define INVALID_SOCKET -1
#define MAXPENDING 5    /* Max connection requests */
#define MAX_RECV_PKT_LENGTH 1023
#define MAX_RECV_BUF_LENGTH (MAX_RECV_PKT_LENGTH*9)
#define PORT "30002"


#define NO_LOGIN_CLIENT_TIME_OUT_SECOND  (60)
#define LOGINED_CLIENT_TIME_OUT_SECOND  (30*60)

#define ENCRYPTDATA 1

typedef unsigned char uint8_t;

#define MAX_PKT_PAYLOAD_SIZE 0x3ff

#define true 1
#define false 0

void printbuf(unsigned char* buf, int size);
void threadprintbuf(unsigned char* buf, int size,SQL_STRUCT* current_sql);

#pragma pack(push)
#pragma pack(1)
typedef struct _CMD_STRUCT_ {
    unsigned char pubID[PUBLIC_ID_LENGTH];
    unsigned short messageID;
    unsigned int length;
    unsigned char data[];
}CMD_STRUCT;
#pragma pack(pop)



typedef struct _STRING_LIST_{
    char* string;
    struct _STRING_LIST_* next;
}STRING_LIST;

extern int need_deamon;

#endif
