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

#define REMOTEPORT 	5150
#define LOCALPORT 	7000

#define MAXEVENTS 100
#define MAXCONNEC   1

#endif
