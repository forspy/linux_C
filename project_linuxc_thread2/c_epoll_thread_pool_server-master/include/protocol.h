#ifndef _PROTOCOL_H
#define _PROTOCOL_H

typedef struct _protocol_head {
	unsigned short messageID;
	unsigned short property;		// Ù–‘£¨0-9 message length,10-12 encrypt type, 13 is seperated
	unsigned char ID[6];
	unsigned short messageNum;
	unsigned short totalPktNum;
	unsigned short currentPktNum;
}protocol;

typedef struct _protocol_pkt{
	protocol head;
	unsigned char msg[];
}protocol_pkt;

#endif
