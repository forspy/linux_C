#include "../master.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: make_sendipv4addr
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	static void make_sendipv4addr(struct sockaddr_in *addr, const int localport)
--
--									struct sockaddr_in *addr: pointer to the sockaddr_in struct in which to store our results
--									const int localport:	the connection's remote port
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Initializes a sockaddr_in struct with a specified remote port for sending
------------------------------------------------------------------------------------------------------------------*/
static void make_sendipv4addr(struct sockaddr_in *addr, int remoteport)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr -> sin_family = AF_INET;
	addr -> sin_port   = htons(remoteport);
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setSend
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int setSend(const char *ipAddr)
--
--									const char *ipAddr: ip address string
--
--
-- RETURNS:	int. socket descriptor
--															
--
-- NOTES:
--
-- Creates a connected socket with the specified remote ip, which it then returns.
------------------------------------------------------------------------------------------------------------------*/
int setSend(char *ipAddr)
{
	
	int sockd = 0;
	
	struct sockaddr_in addr;
	

	make_sendipv4addr(&addr, REMOTEPORT);
	
	addr.sin_addr.s_addr = inet_addr( ipAddr );	

	sockd = socket(AF_INET, SOCK_STREAM, 0);
	
	connect(sockd, (struct sockaddr *)&addr, sizeof(addr));
	
	return sockd;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendPacket
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int sendPacket(int fd, char *str)
--
--									int fd: sending socket
--									char *str: buffer containing user's message to be sent
--
--
-- RETURNS:	int. exit status of send
--															
--
-- NOTES:
--
-- Sends a packet.
--
-- Calls write the passed string to the passed socket.
------------------------------------------------------------------------------------------------------------------*/
int sendPacket(int fd, char *str)
{
	char data[MAXBUFLEN] = { 0 };
	
	// send
	strcpy ( data, str);
	return ( send( fd, data, sizeof(data), 0 ));
}








