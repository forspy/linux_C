#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "global.h"

int socketInit(int port)
{
	int serversock;
	struct sockaddr_in echoserver;
	
	if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    	return INVALID_SOCKET;
    }
	memset((void*)&echoserver, 0, sizeof(echoserver));       /* Clear struct */
    echoserver.sin_family = AF_INET;                  /* Internet/IP */
    echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   /* Incoming addr */
    echoserver.sin_port = htons(atoi(PORT));          /* server port */

	/* Bind the server socket */
	if (bind(serversock, (struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) {
		return INVALID_SOCKET;
	}
	/* Listen on the server socket */
	if (listen(serversock, MAXPENDING) < 0) {
		return INVALID_SOCKET;
	}
	return serversock;
}

void serverStart()
{
	int ret;
	int socket;
	int sin_size;
	int new_fd;
	struct sockaddr_in their_addr; /* 客户地址信息 */
	socket = socketInit(SERVER_PORT);
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(socket, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
            perror("accept");
            continue;
        }
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
        if (!fork()) { 
			handle(new_fd,their_addr);
        }
        close(new_fd); /*父进程不再需要该socket*/
	}
}

//废弃
void mainThread(void)
{
	while(1)
	{
		serverStart();
		sleep(1);
	}
}
