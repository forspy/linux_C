#include "master.h"

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILES: client.c, 
--
--	An epoll implementation of a GUI chat server for handling multiple clients.
--
-- PROGRAM: HyperMegaChatProgramClient
--
-- FUNCTIONS:
--
-- server.c
--
-- void cleanStr(char *str)
-- void setEpoll(int *socklsn, int *epollfd)
-- int epollStuff(struct ipc *stipc)
-- int makeSem(sem_t **sem)
-- int makeUSocks(int usockfd[2])
-- int main(int argc, char **argv)
-- void cleanListOfClients(struct bidirectional *listOfClients)
-- int handleNewConnection(int socklsn, int epollfd, struct bidirectional listOfClients[MAXCONNEC], int usock)
-- int handleNewRead(int fd, int epollfd, struct bidirectional listOfClients[MAXCONNEC], int usock)
--
--
-- server_read.c
--
-- static void make_recvipv4addr(struct sockaddr_in *addr, const int localport)
-- int setupRecv()
-- int recvPacket(int epollfd, char *buf)
-- int setNonBlocking(int sock)
-- int getNewConnection(int epollfd, int socklsn)
-- void getipAddr(char ipbuf[], int fd)
-- 
--
-- server_send.c
--
-- static void make_sendipv4addr(struct sockaddr_in *addr, const int localport)
-- int setSend(const char *ipAddr)
-- int sendPacket(int fd, char *str)
-- 
--
-- serverGui.c
--
-- gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data )
-- void makeGui( struct ipc *stipc )
-- static void makeStatsbox(GtkWidget *horz_box, struct ipc *stipc)
-- static void makeClientbox(GtkWidget *horz_box, GtkListStore **store)
-- static void makeColumn(GtkWidget *chatbox, const char *TITLE, int INDEX)
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
-- This is the server portion of a larger client-server chat program in which a server transmits text messages between clients.
----------------------------------------------------------------------------------------------------------------------*/

void           makeGui( struct ipc * ipcs );
gboolean appendChatMsg( GIOChannel * channel, GIOCondition condition, gpointer data );


int setupRecv				(						 						 );
int getNewConnection(int epollfd, int socklsn);
int setNonBlocking	(int sock								 );
int recvPacket			(int epollfd , char *buf );
void getipAddr			(char ipbuf[], int   fd	 );

void sendToListOfClients	(struct bidirectional listOfClients[MAXCONNEC], char msgbuf[MAXBUFLEN]);
void appendToListOfClients(struct bidirectional listOfClients[MAXCONNEC], char ipbuf [MAXBUFLEN], int fd);


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
	size_t len = sizeof(str);
	memset (  str, 0, len  );
}
	
	
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: cleanListOfClients
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void cleanStr(struct bidirectional *listOfClients)
--
--									struct bidirectional *listOfClients: array listing all connected clients (contains ip and associated file descriptors)
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Removes a client and its associated sockets from the list of clients on disconnect
------------------------------------------------------------------------------------------------------------------*/
void cleanListOfClients(struct bidirectional *listOfClients)
{
	size_t i   = 0;
	memset ( listOfClients, 0,  MAXCONNEC	 );
	
	for(i = 0; i < MAXCONNEC; i++)
	{
		cleanStr(listOfClients[i].ip);
		
		listOfClients[i].sendSock = 0;
		listOfClients[i].recvSock = 0;
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
	
	int templsn = 0;
	int tempfd = 0;
	//服务器的一般流程
	templsn = setupRecv(); 			// create & bind socket
	
	//设置把socket设置为非阻塞
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
--									int fd		: listening socket descriptor
--									int epollfd	: epoll file descriptor
--									struct bidirectional listOfClients[]: array listing all connected clients (contains ip and associated file descriptors)
--									int usock	: unix socket (for ipc between gui and background network process)
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
-- Appends the new client's ip and associated file descriptors to an array
------------------------------------------------------------------------------------------------------------------*/
int handleNewConnection(int socklsn, int epollfd, struct bidirectional listOfClients[MAXCONNEC], int usock)
{
	char ipbuf[MAXBUFLEN] = { 0 };
	
	int fd = getNewConnection(epollfd, socklsn);
	
	if( fd == -1 ) return 0;
		
	getipAddr( ipbuf, fd );

	appendToListOfClients(listOfClients, ipbuf, fd);
	
	strcat( ipbuf, " <--- this guy just connected" );
	send( usock, (void*)&ipbuf, sizeof(ipbuf), 0 );

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
-- If EOF received (server disconnects), writes "disconnected" along with the client's ip to gui and closes the receiving socket.
-- Otherwise, writes the packet data to the gui.
------------------------------------------------------------------------------------------------------------------*/
int handleNewRead(int fd, int epollfd, struct bidirectional listOfClients[MAXCONNEC], int usock)
{
	char msgbuf[MAXBUFLEN + 30];
	char rcvbuf[MAXBUFLEN];
	char ipbuf [MAXBUFLEN];
					
	getipAddr (ipbuf, fd);


	if(recvPacket(fd, rcvbuf) == 0)
	{
		int i = 0;
		struct epoll_event  event;
		event . data.fd = fd;
		event . events  = EPOLLIN;
		

		for(i = 0; i < MAXCONNEC; i++)
		{
            //遍历聊天服务器的客户链表
			if(listOfClients[i].recvSock == fd)
			{
				char ipbuf[MAXBUFLEN] = { 0 };

				strcpy( ipbuf, listOfClients[i].ip  );
				strcat( ipbuf, " <--- disconnected" );
				send 	( usock, (void*)&ipbuf, sizeof(ipbuf), 0 );
			
				cleanStr(listOfClients[i].ip);
				
				close(listOfClients[i].sendSock);
				close(listOfClients[i].recvSock);
								
				listOfClients[i].sendSock = 0;
				listOfClients[i].recvSock = 0;
			}
		}
		
		epoll_ctl (epollfd, EPOLL_CTL_DEL, fd, &event);
		close( fd );
		
		return 0;
	}
	
	else
	{	
		int i = 0;
		for(i = 0; i < MAXCONNEC; i++)
		{

			if(listOfClients[i].recvSock == fd)
			{
				char ipbuf[MAXBUFLEN] = { 0 };

                //给前台界面发送
				strcpy( ipbuf, listOfClients[i].ip  );
				strcat( ipbuf, " wrote something" );
				send 	( usock, (void*)&ipbuf, sizeof(ipbuf), 0 );
				
				sprintf(msgbuf, "%s said: %s"            , listOfClients[i].ip, rcvbuf  );
				fprintf(stderr, "\n\n----------\n%s %s\n", ipbuf, msgbuf); fflush(stdout);
			}
		}
		
		
		sendToListOfClients(listOfClients, msgbuf);
		return 1;
	}
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
int epollStuff(struct ipc * stipc)
{
	int				i = 0;
	int socklsn = 0;
	int epollfd = 0;
	
	struct epoll_event events[MAXEVENTS];

	struct bidirectional listOfClients[MAXCONNEC];
	//做线程同步的信号
	sem_wait( stipc -> sem );
	
	cleanListOfClients(listOfClients);

	setEpoll( &socklsn, &epollfd );


	while( 1 )
	{
		int        i;
		int  numEvts; 
		
		char closebuf[MAXBUFLEN];
        
		//从前台接收数据
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
			//处理新的链接  监听句柄事件触发
				if( handleNewConnection(socklsn, epollfd, listOfClients, stipc -> usockfd[BACKGROUND]) == 0 ) continue;
			}
			
			// read ready
			else
			{
			//处理接收链接  链接句柄事件触发
				handleNewRead(events[i].data.fd, epollfd, listOfClients, stipc -> usockfd[BACKGROUND]);
			}
		}
	}
	
	for(i = 0; i < MAXCONNEC; i++)
	{
		close(listOfClients[i].sendSock);
		close(listOfClients[i].recvSock);
	}
	
	close(socklsn);
	close(epollfd);
	sem_post(stipc->sem);
	
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
	
	
	
	if( (stipc.pid = fork()) == 0 )
	{	
	    //服务器后台
		epollStuff( &stipc );
		fprintf(stderr, "exited recv process\n");
	}
	else
	{
	    //显示界面前台
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

		fprintf(stderr, "exited gui process\n");
	}

	close(stipc.usockfd[FOREGROUND]);
	close(stipc.usockfd[BACKGROUND]);
	
	return EXIT_SUCCESS;
}
