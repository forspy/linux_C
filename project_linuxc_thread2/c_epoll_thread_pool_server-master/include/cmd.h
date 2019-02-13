#ifndef _CMD_H_
#define _CMD_H_

#include <arpa/inet.h>
#include "client.h"
#include "car_protocol.h"
#include "RSA.h"
#include "stdlib.h"
#include "string.h"

extern STRING_LIST accept_ctrl_ip;
extern int cmd_port;

void *cmd_listen_thread(void *arg);

int make_id_pkt(unsigned char* buf,
                unsigned char* imei,
                unsigned char* simNum,
                SQL_STRUCT* current_sql);

#define RSA_KEY_BYTE_LENGTH 128

typedef struct _PROTOCOL_RSA_DATA_{
    unsigned int e;
    unsigned char n[RSA_KEY_BYTE_LENGTH];
}PROTOCOL_RSA_DATA;
void make_RSA_data(unsigned char* buf);
void foreachClient(unsigned char* inputbuf,int length,unsigned short messageID);
void cmd_trans_data(unsigned char* inputbuf,int length,unsigned short protocol, Client_hash_node* client_info);
int checkUserNameAndPwd(unsigned char* username,unsigned char* password,SQL_STRUCT* current_sql);
void addCarUserInfoAndPlate(int carID,int userID,SQL_STRUCT* current_sql,unsigned char* plate);
#endif

