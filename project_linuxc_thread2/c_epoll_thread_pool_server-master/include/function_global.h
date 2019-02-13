#ifndef _FUNCTION_GLOBAL_H_
#define _FUNCTION_GLOBAL_H_
#include "mysql.h"
#include <arpa/inet.h>
#include "client.h"
#include "car_protocol.h"
#include "mytool.h"
#include "cJSON.h"
#include "global.h"

enum {
    FUNCTION_HANDLE_USED_PLATE = -2,
    FUNCTION_HANDLE_SUCCESS    = 0,
    FUNCTION_HANDLE_FAILED     = 1,
    FUNCTION_HANDLE_ERROR      = 2,
    FUNCTION_HANDLE_NOT_SUPPORT = 3
};

void handle_client_aes(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_data_trans(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_driving_data(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_ans(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_heartbeat(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_confirmID(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_register(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_data_return(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_user_bind(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_check_upgrade(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_get_userinfo(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_nothing(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);
void handle_client_trans_internet_explorer(unsigned char* inputbuf,int length, Client_hash_node* client_info,SQL_STRUCT* current_sql);

int make_ans(
            unsigned char* buf,                 //传出数据
            unsigned short messageID,           //回复的messageID
            unsigned char  status,              //回复的状态
            unsigned short recvMessageNum);      //回复的是第几个消息

typedef struct _REGISTER_DATA_{
    unsigned char md5[MD5_STR_LEN];
//    unsigned char username[USER_NAME_LENGTH];
//    unsigned char password[PASSWORD_LENGTH];
    unsigned char imei[IMEI_LENGTH];
    unsigned char simNum[SIM_NUM_LENGTH];
    unsigned char id[];
}REGISTER_DATA;

typedef struct _BIND_USER_INFO_ {
    unsigned char username[USER_NAME_LENGTH];
    unsigned char password[PASSWORD_LENGTH];
    unsigned char plate[PLATE_LENGTH];
}BIND_USER_INFO;


#endif
