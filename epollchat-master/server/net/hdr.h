#ifndef HDR_H
#define HDR_H


#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>

#define LOCALPORT 	5150
#define REMOTEPORT 	7000

#define MAXEVENTS 100
#define MAXCONNEC  10

struct bidirectional {

	int sendSock;
	int recvSock;
	char ip[MAXBUFLEN];

};

#endif
