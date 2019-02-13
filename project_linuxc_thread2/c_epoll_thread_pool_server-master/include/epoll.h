#ifndef _EPOLL_H_
#define _EPOLL_H_

#include "client.h"

int send_pkt(Client* client);
void* epoll ();
void load_config(char* project_name,char* config_file);
void send_encrypted_data(unsigned char* data, int length, 
                         int encrypt_type,unsigned short messageID,Client* client);

#endif
