#ifndef _CAR_PROTOCOL_H_
#define _CAR_PROTOCOL_H_

#include "client.h"
#include "protocol.h"
#include "function_global.h"
#include "encrypt.h"

#define WAIT_NEXT_PKT 0
#define PKT_HANDLED 1


int handle_pkt(protocol_pkt* pkt,Client_hash_node* client_info,SQL_STRUCT* current_connection);

#endif