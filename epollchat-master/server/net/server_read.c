#include "../master.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: make_recvipv4addr
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	static void make_recvipv4addr(struct sockaddr_in *addr, const int localport)
--
--									struct sockaddr_in *addr: pointer to an sockaddr_in struct in which to store our results
--									const int localport:	the connection's local port
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Initializes a sockaddr_in struct with a specific local port for receiving.
------------------------------------------------------------------------------------------------------------------*/
static void make_recvipv4addr(struct sockaddr_in *addr, int localport)
{
	memset(addr, 0, sizeof(struct sockaddr_in));
	addr -> sin_family = AF_INET;
	addr -> sin_port   = htons(localport);
		
	addr -> sin_addr.s_addr = INADDR_ANY;
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setNonBlocking
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int setNonBlocking(int sock)
--
--									int sock: a socket file descriptor
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Calls fcntl with an O_NONBLOCK flag in order to set the passed socket fd to non-blocking.
------------------------------------------------------------------------------------------------------------------*/
int setNonBlocking(int sock)
{
	int flags = 0;
	
	flags = fcntl(sock, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flags);
	
	return 1;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getNewConnection
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int getNewConnection(int epollfd, int socklsn)
--
--									int epollfd: epoll file descriptor
--									int socklsn: the listening socket
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Handles new connections
--
-- Accepts an incoming connection. Calls accept(), which returns a new socket for receiving
-- Calls setNonBlocking() to sets the new socket to non-blocking mode
-- Adds the new socket to the epoll instance
------------------------------------------------------------------------------------------------------------------*/
int getNewConnection(int epollfd, int socklsn)
{	
	struct epoll_event 		event;
	struct sockaddr_in 		clientaddr;
	socklen_t 				clientaddrLen;
	int 					sockrcv;
	
	clientaddrLen = (socklen_t)sizeof(clientaddr);

	sockrcv = accept(socklsn, (struct sockaddr *)&clientaddr, &clientaddrLen);


	setNonBlocking (sockrcv);
	
	
	event . data.fd = sockrcv;
  	event . events  = EPOLLIN;
	
	epoll_ctl (epollfd, EPOLL_CTL_ADD, sockrcv, &event);
	return sockrcv;
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setupRecv
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int setupRecv()
--
--
--
-- RETURNS:	int. listening socket descriptor
--															
--
-- NOTES:
--
-- Calls make_recvip4addr 
-- Creates a SOCK_STREAM socket
-- Calls setsockopt to allow a connection to be reestablished on the same socket during a TIME_WAIT state
-- binds the socket to the local port
------------------------------------------------------------------------------------------------------------------*/
int setupRecv()
{
	int  optval = 1;
	int socklsn = 0;
	struct sockaddr_in serveraddr;	



	make_recvipv4addr(&serveraddr, LOCALPORT);


	socklsn = socket(AF_INET, SOCK_STREAM, 0);

	setsockopt( socklsn, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	

	bind(socklsn, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

	return socklsn;
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: recvPacket
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int recvPacket(int epollfd, char *buf)
--
--									int epollfd: epoll file descriptor
--									char *buf: buffer in which to store received packet
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Receives a complete packet from the specified file descriptor.
--
-- Calls read in a loop until the needed packet size is read.
-- Copies the result to passed buffer.
------------------------------------------------------------------------------------------------------------------*/
int recvPacket(int epollfd, char *buf)
{
	char packet[MAXBUFLEN] = { 0 };
	char * packetPtr = packet;
			
	int nBytesNeed = MAXBUFLEN;
	int nBytesRecv = 0;
			
	while( nBytesNeed > 0 )
	{
		nBytesRecv  = read(epollfd, packetPtr, nBytesNeed);
		
		if(nBytesRecv == -1 || nBytesRecv == 0) return 0;
		
		nBytesNeed -= nBytesRecv;
		packetPtr  += nBytesRecv;
	}

	strcpy(buf, packet);

	return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: getipAddr
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void getipAddr(char ipbuf[], int fd)
--
--									int fd		 	: socket descriptor
--									char ipbuf[]: buffer for storing retrieved ip address
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Gets the ip of a remote socket:
-- 获取对端IP地址
-- Calls getpeername to retrieve a sockaddr_in struct.
-- Passes the struct to inet_ntop to resolve the ip address and copy it to a buffer
------------------------------------------------------------------------------------------------------------------*/
void getipAddr(char ipbuf[], int fd)
{			

	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);

	printf("%d", getpeername(fd, (struct sockaddr*)&addr, &len));

	struct sockaddr_in *in = (struct sockaddr_in *)&addr;
	inet_ntop(AF_INET, &in->sin_addr, ipbuf, MAXBUFLEN);
	
	fprintf(stderr, "%s\n", ipbuf);
}


