#include "../master.h"


int setSend		(char *ipAddr 		 );
int sendPacket	(int sockd, char *str);


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: appendToListOfClients
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void appendToListOfClients(	struct bidirectional listOfClients[MAXCONNEC], char ipbuf[MAXBUFLEN], int fd)
--
--									struct bidirectional listOfClients[MAXCONNEC]: array containing the ip addresses of ALL clients and their associated sockets
--									char ipbuf[MAXBUFLEN]: buffer containing the ip address of the newly connected client
--									int fd: the receive socket file descriptor of the newly connected client
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Appends a client to a list of clients
--
-- Loops through the list of clients until it finds an empty spot, then appends the new client to it (copies the ip and socket descriptor)
-- Calls setSend to return and copy a new sending socket forever associated with that client.
------------------------------------------------------------------------------------------------------------------*/
void appendToListOfClients(struct bidirectional listOfClients[MAXCONNEC], char ipbuf[MAXBUFLEN], int fd)
{
	int i = 0;

	for(i = 0; i < MAXCONNEC; i++)
	{
		if(listOfClients[i].recvSock == 0)
		{	
			strcpy(listOfClients[i].ip, ipbuf);
			
			listOfClients[i].sendSock = setSend(ipbuf);
			listOfClients[i].recvSock = fd;
			
			fprintf(stderr, "\n[new client's remote socket: %d. new client's ip: %s]\n", listOfClients[i].sendSock, listOfClients[i].ip);
			break;
		}
	}	
}




/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendToListOfClients
--
-- DATE:		March 24, 2014
--
-- REVISIONS:	None
--
-- DESIGNER: 	Sam Youssef
--
-- PROGRAMMER:	Sam Youssef
--
-- INTERFACE:	void sendToListOfClients(	struct bidirectional listOfClients[MAXCONNEC], 
--													char msgbuf[MAXBUFLEN])
--
--									struct bidirectional listOfClients[MAXCONNEC]: array containing the ip addresses of ALL clients and their associated sockets
--									char msgbuf[MAXBUFLEN]: buffer containing the message to be sent
--
--
-- RETURNS:	void.
--															
--
-- NOTES:
--
-- Sends chat message to list of client
--
-- Loops through the list of clients and calls sendPacket on each file descriptor, passing in the message.
------------------------------------------------------------------------------------------------------------------*/
void sendToListOfClients( struct bidirectional listOfClients[MAXCONNEC], char msgbuf[MAXBUFLEN] )
{
	int i = 0;
	for(i = 0; i < MAXCONNEC; i++)
	{
		if(listOfClients[i].sendSock != 0)
		{	
			int nBytesSent = sendPacket( listOfClients[i].sendSock, msgbuf );

			if( nBytesSent == -1 )
			{
				close(listOfClients[i].sendSock);
				close(listOfClients[i].recvSock);
				puts("closed");
				return;
			}

			fprintf
			(
				stderr, 
				"\nclient ip: %s\nsendsock: %d\nnbytesrecv: %d\n", 
				listOfClients[i].ip			, 
				listOfClients[i].sendSock	,
				nBytesSent
			);
		}
	}	
}
