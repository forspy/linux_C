#include "master.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILES: client.c, 
--
--	An epoll implementation of a GUI chat client application for communicating over a server.
--
-- PROGRAM: HyperMegaChatProgramClient
--
-- FUNCTIONS:
--
-- client.c
--
-- void cleanStr(char *str)
-- void getipAddr(char ipbuf[], int fd)
-- int handleNewConnection(int socklsn, int epollfd, int usock)
-- int handleNewRead(int fd, int epollfd, int usock)
-- void setEpoll(int *socklsn, int *epollfd)
-- int epollStuff(struct ipc *stipc)
-- int makeSem(sem_t **sem)
-- int makeUSocks(int usockfd[2])
-- int main(int argc, char **argv)
-- 
--
-- client_read.c
--
-- static void make_recvipv4addr(struct sockaddr_in *addr, const int localport)
-- int setupRecv()
-- int recvPacket(int epollfd, char *buf)
-- int setNonBlocking(int sock)
-- int getNewConnection(int epollfd, int socklsn)
-- 
--
-- client_send.c
--
-- static void make_sendipv4addr(struct sockaddr_in *addr, const int localport)
-- int setSend(const char *ipAddr)
-- int sendPacket(int sockd, char *str)
-- 
--
-- clientGui.c
--
-- gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data )
-- void makeGui( struct ipc *stipc )
-- static void makeStatsbox(GtkWidget *horz_box, struct ipc *stipc)
-- static void callback_setSend( gpointer data )
-- static void makeChatbox(GtkWidget *horz_box, struct ipc *stipc)
-- static void makeColumn(GtkWidget *chatbox, const char *TITLE, int INDEX)
-- static void callback_sendPacket( gpointer data )
-- static void makeTabs(GtkWidget **tabs, GtkWidget **page, const size_t size)
-- static void createWindow(GtkWidget **win)
-- static void destroyGui()
--
--
-- DATE: March 24, 2014
--
-- REVISIONS: None
--
-- DESIGNER: Sam Youssef
--
-- PROGRAMMER: Sam Youssef
--
-- NOTES:
--
-- This is the client portion of a larger client-server chat program in which a server transmits text messages between clients.
----------------------------------------------------------------------------------------------------------------------*/


extern int sendSock;


void           makeGui( struct ipc * ipcs );
gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data );


/* send */
int setSend			( const char *ipAddr	); // create  & connect socket
int sendPacket	( int sockd, char *str); // send

/* recv */
int setupRecv				( 											 ); // create & bind socket
int getNewConnection(int epollfd, int socklsn);
int setNonBlocking	(int sock								 ); // fcntl
int recvPacket			(int epollfd, char *buf	 ); // recv


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: cleanStr
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void cleanStr(char *str)
--
--									char *str: a string value
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Sets a string's value to null.
------------------------------------------------------------------------------------------------------------------*/
void cleanStr(char *str)
{
	int len = sizeof( str );
	memset ( str, 0, len  );
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
--
-- Calls getpeername to retrieve a sockaddr_in struct.
-- Passes the struct to inet_ntop to resolve the ip address and copy it to a buffer
------------------------------------------------------------------------------------------------------------------*/
void getipAddr(char ipbuf[], int fd)
{			
	struct sockaddr_storage addr;
	socklen_t len = sizeof(addr);

	getpeername(fd, (struct sockaddr*)&addr, &len);

	struct sockaddr_in *in = (struct sockaddr_in *)&addr;
	inet_ntop(AF_INET, &in->sin_addr, ipbuf, MAXBUFLEN);
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: handleNewConnection
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int handleNewConnection(int socklsn, int epollfd, int usock)
--
--									int fd		 	: listening socket descriptor
--									int epollfd	: epoll file descriptor
--									int usock		: unix socket (for ipc between gui and background network process)
--
--
-- RETURNS:	int. 1 or 0 on timeout
--															
--
-- NOTES:
--
-- Interface for handling new connections:
--
-- Calls getNewConnection
-- If a connection arrives, calls getipAddr on returned socket and writes the remote ip address to the gui
------------------------------------------------------------------------------------------------------------------*/
int handleNewConnection(int socklsn, int epollfd, int usock)
{
	char ipbuf[MAXBUFLEN] = { 0 };
		
	int fd = getNewConnection(epollfd, socklsn);
	
	getipAddr(ipbuf, fd);
	
	if( fd == -1 ) return 0;
	
	strcat(ipbuf, " <--- connected to this address");
	send( usock, (void*)&ipbuf, sizeof(ipbuf), 0 );

	fprintf(stderr, "%s", ipbuf); fflush(stdout);
	
	return 1;
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: handleNewRead
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int handleNewRead(int fd, int epollfd, int usock)
--
--									int fd		 	: receive socket descriptor
--									int epollfd	: epoll file descriptor
--									int usock		: unix socket (for ipc between gui and background network process)
--
--
-- RETURNS:	int. 1 or 0 on EOF
--															
--
-- NOTES:
--
-- Interface for handling file descriptors ready to be read:
--
-- Calls recvPacket to receive a packet.
-- If EOF received (server disconnects), writes "disconnected" to gui and closes the receiving socket.
-- Otherwise, writes the packet data to the gui.
------------------------------------------------------------------------------------------------------------------*/
int handleNewRead(int fd, int epollfd, int usock)
{
	char rcvbuf[MAXBUFLEN] = { 0 };
	struct epoll_event event;
		
	if(recvPacket(fd, rcvbuf) == 0)
	{
		epoll_ctl (epollfd, EPOLL_CTL_DEL, fd, &event);
		close( fd );
		close( sendSock );
		
		strcpy(rcvbuf, "disconnected");
		
		send( usock, (void*)&rcvbuf, sizeof(rcvbuf), 0 );
		return 0;
	}
	else
	{			
		fprintf(stderr, "(%s)\n", rcvbuf); fflush(stdout);
		
		send( usock, (void*)&rcvbuf, sizeof(rcvbuf), 0 );
		return 1;
	}

}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setEpoll
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void setEpoll(int *socklsn, int *epollfd)
--
--									int *socklsn: ptr to listening socket file descriptor
--									int *epollfd: ptr to epoll file descriptor
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Creates a listening socket and an epoll file descriptor:
--
-- Calls setupRecv to create and bind the listening socket
-- Sets the listening socket to nonblocking
-- Sets it to listen
--
-- Creates an epoll instance
-- Adds the listening socket file descriptor to the epoll instance
------------------------------------------------------------------------------------------------------------------*/
void setEpoll(int *socklsn, int *epollfd)
{
	struct epoll_event event;
	
	int templsn;
	int tempfd;

	templsn = setupRecv(); 			// create & bind socket
	
		
	setNonBlocking ( templsn );	// fcntl
	listen(templsn, MAXCONNEC);	// listen


	tempfd = epoll_create1(0);

	event . data.fd = ( templsn );
	event . events  = EPOLLIN | EPOLLOUT;
	
	epoll_ctl(tempfd, EPOLL_CTL_ADD, templsn, &event);
	
	*socklsn = templsn;
	*epollfd = tempfd;
}

	
	
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: epollStuff
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int epollStuff(struct ipc *stipc)
--
--									struct ipc *stipc: struct containing ipc variables
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Main interface for receiving network packets
--
-- Calls setEpoll to create the listening socket and add it to the epoll file descriptor
-- Calls a non-blocking receive to check for a termination message from the gui process; terminates loop if recv succeeds
-- Cals epoll_wait to wait for a file descriptor to be read
-- 	If an fd is ready: 
--		If it's a new connection --> calls handleNewConnection to add the new file descriptor to the epoll instance
--		If it's a new read on an existing fd --> calls handleNewRead to read the packet
-- On loop termination: closes send socket, listening socket and the epoll file descriptor
------------------------------------------------------------------------------------------------------------------*/
int epollStuff(struct ipc *stipc)
{
	int socklsn = 0;
	int epollfd = 0;
	struct epoll_event events[MAXEVENTS];


	setEpoll( &socklsn, &epollfd );


	while( 1 )
	{
		int        i;
		int  numEvts;
		
		char closebuf[MAXBUFLEN] = { 0 };

		if( recv(stipc->usockfd[BACKGROUND], (char*)&closebuf, sizeof(closebuf), MSG_DONTWAIT) != -1 )
		{
			break;
		}

		numEvts = epoll_wait(epollfd, events, MAXEVENTS, 10);

		for(i = 0; i < numEvts; i++)
		{
			// bad
			if((events[i].events & EPOLLERR) ||			
         	(events[i].events & EPOLLHUP) ||
       		(!(events[i].events & EPOLLIN)))
			{
				//char ipbuf [MAXBUFLEN];
				//getipAddr(ipbuf, events[i].data.fd);
				//strcat(ipbuf, " error");
				//send( stipc -> sockfd[ 0 ], (void*)&ipbuf, sizeof(ipbuf), 0 );
				close( events[i].data.fd );
				continue;
			}
		
			// new conn
			else if(socklsn == events[i].data.fd)		
			{
				if( handleNewConnection(socklsn, epollfd, stipc -> usockfd[BACKGROUND]) == 0 ) continue;
			}
			
			// read ready
			else	
			{
				handleNewRead(events[i].data.fd, epollfd, stipc -> usockfd[BACKGROUND]);
			}
		}
	}
	
	close(sendSock);
	close(socklsn);
	close(epollfd);
	return 1;
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeSem
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int makeSem(sem_t **sem)
--
--									sem_t **sem: an unintialized semaphore
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Makes a semaphore:
--
-- Unlinks any existing semaphore with the same name as the one we want to create
-- Creates an open semaphore for reading and writing with 0666 privileges
------------------------------------------------------------------------------------------------------------------*/
int makeSem(sem_t **sem)
{
	sem_unlink( OPEN_SESAME );
	
	if((*sem = sem_open( OPEN_SESAME, O_CREAT, 0666 | S_IRUSR | S_IWUSR, 0)) == SEM_FAILED)
	{
		perror("failed to create semaphore");
		return 0;
	}
	return 1;
}



/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: makeUSocks
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int makeUSocks(int usockfd[2])
--
--									int usockfd: a socket pair
--
--
-- RETURNS:	int. 1 on success, 0 on fail
--															
--
-- NOTES:
--
-- Creates a bridged pair of unix sockets and sets both to non-blocking
------------------------------------------------------------------------------------------------------------------*/
int makeUSocks(int usockfd[2])
{
	if(socketpair(AF_UNIX, SOCK_STREAM, 0, usockfd) == -1)
	{
		perror("socketpair");
		return 0;
	}

	else
	{
		return setNonBlocking( usockfd[BACKGROUND] ) && setNonBlocking( usockfd[FOREGROUND] );
	}
	
	return 1;
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: main
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	int main(int argc, char **argv)
--
--									int argc: number of cmd-line args
--									char **argv: array of cmd-line argument strings
--
--
-- RETURNS:	int. 0 on exit success. 1 on exit failure.
--															
--
-- NOTES:
--
-- Main entry point:
--
-- Creates a semaphore to coordinate gui and network process termination sequence (background finishes first and increments the sem)
-- Creates a pair of connected sockets to communicate between the gui and network process
-- 
-- Forks the background network process (calls epollStuff)
-- Initializes the Gtk Window (calls makeGui) and binds one of the unix socket pair to GTK
-- Closes both unix sockets once finished
------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
	int pid;
	
	struct ipc stipc;
	memset(&stipc, 0, sizeof(stipc));

		
	if(makeSem(&stipc.sem) == 0)
	{
		perror("makeSem");
		return 1;
	}
	
	if(makeUSocks(stipc.usockfd) == 0)
	{
		perror("makeUSocks");
		return 1;
	}

	
	
	
	if( (pid = fork()) == 0 )
	{	
		epollStuff( &stipc );
		
		sem_post(stipc.sem);
		
		fprintf(stderr, "exited recv process\n");
	}
	else
	{
		char *closebuf = "close";
	
		GIOChannel * gioch;

	
		gtk_init(&argc, &argv);
		makeGui ( &stipc );
	
		gioch = g_io_channel_unix_new( (gint)stipc.usockfd[FOREGROUND] );
		
		g_io_channel_set_encoding	( gioch, 0, 0  );
		g_io_channel_set_flags		( gioch, G_IO_FLAG_APPEND | G_IO_FLAG_NONBLOCK, 0 );
		
		g_io_add_watch( gioch, G_IO_IN, appendChatMsg, (gpointer)&stipc );	

		gtk_main();


		send(stipc.usockfd[FOREGROUND], (void*)&closebuf, sizeof(closebuf), 0);

		sem_wait(stipc.sem);
		
		fprintf(stderr, "exited send process\n");
	}

	close(stipc.usockfd[FOREGROUND]);
	close(stipc.usockfd[BACKGROUND]);
	
	return EXIT_SUCCESS;
}






















