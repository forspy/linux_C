#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <errno.h>  
#include <sys/socket.h>  
#include <netdb.h>  
#include <fcntl.h>  
#include <sys/epoll.h>  
#include <string.h>
#include "global.h"
#include "client.h"
#include "protocol.h"
#include "car_protocol.h"
#include "RSA.h"
#include "epoll.h"
#include "cmd.h"
#include "mysql.h"
#include "thread_pool.h"
  
int max_events=40000;

int max_so_connect = 2000;

char AppKey[100];
char AppSecret[1000];

time_t seconds;

enum data_type{
	TYPE_NULL=-1,
	TYPE_INT=0,
};

typedef struct _CONFIG{
	const char* name;
	void* point;
	void (*FunP)(char* buf, void* data);
}CONFIG;

void read_int_str(char* buf, void* data);
void read_str_str(char* buf, void* data);
void add_str_list(char* buf, void* data);

static CONFIG conf[] = {
	{"MAX_EVENTS",(void*)&max_events,read_int_str},
	{"MYSQL_ADDR",sqlIP,read_str_str},
	{"MYSQL_DATABASE",sqlDatabaseName,read_str_str},
	{"MYSQL_PORT",(void*)&sqlPort,read_int_str},
	{"MYSQL_USERNAME",sqlUserName,read_str_str},
	{"MYSQL_PASSWORD",sqlPwd,read_str_str},
	{"CTRL_LISTEN_IP",(void*)&accept_ctrl_ip,add_str_list},
	{"CTRL_LISTEN_PORT",(void*)&cmd_port,read_int_str},
	{"DEAMON",(void*)&need_deamon,read_int_str},
	{"LOG_FILE",log_file,read_str_str},
	{"LOG_LEVEL",(void*)&log_level,read_int_str},
	{"MAX_TPOOL_THREAD_NUM",(void*)&MAX_TPOOL_THREAD_NUM,read_int_str},
	{"MAX_SO_CONNECT",(void*)&max_so_connect,read_int_str},
	{NULL,NULL,NULL}
};

void add_str_list(char* buf, void* data)
{
    int l;
    STRING_LIST* tmp=(STRING_LIST*)data;
    l = strlen(buf);
    while(tmp)
    {
        if(tmp->next == NULL)
        {
            tmp->next = (STRING_LIST*) MALLOC(sizeof(STRING_LIST));
            tmp->next->string = (char*)MALLOC(l+1);
            tmp->next->next = NULL;
            memcpy(tmp->next->string,buf,l+1);
            break;
        }
        else
        {
            tmp = tmp->next;
        }
    }
}

void read_str_str(char* buf, void* data)
{
    int i;
    char* b;
    b=(char*) data;
//    memcpy(b,buf,strlen(buf));
    sscanf(buf,"%s",b);
}

void read_int_str(char* buf, void* data)
{
	int tmp;
	int*b;
	b=(int*)data;
	sscanf(buf,"%d",&tmp);
	*b = tmp;
}


extern Client_hash_node* all_clients;

  
//函数:  
//功能:创建和绑定一个TCP socket  
//参数:端口  
//返回值:创建的socket  
static int  
create_and_bind ()  
{  
  struct addrinfo hints;  
  struct addrinfo *result, *rp;  
  int s, sfd;  
  
  memset (&hints, 0, sizeof (struct addrinfo));  
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */  
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */  
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */  
  
  s = getaddrinfo (NULL, PORT, &hints, &result);  
  if (s != 0)  
    {  
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));  
      return -1;  
    }  
  
  for (rp = result; rp != NULL; rp = rp->ai_next)  
    {  
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);  
      if (sfd == -1)  
        continue;  
  
      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);  
      if (s == 0)  
        {  
          /* We managed to bind successfully! */  
          break;  
        }  
  
      close (sfd);  
    }  
  
  if (rp == NULL)  
    {  
      fprintf (stderr, "Could not bind\n");  
      return -1;  
    }  
  
  freeaddrinfo (result);  
  
  return sfd;  
}  


static int make_socket_no_time_wait(int fd)
{
    int z;
    struct linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;

    z = setsockopt(fd,
            SOL_SOCKET,
            SO_LINGER,
            &so_linger,
            sizeof so_linger);
    if(z)
    {
        return -1;
    }

    return 0;
}
  
  
//函数  
//功能:设置socket为非阻塞的  
static int  
make_socket_non_blocking (int sfd)  
{  
  int flags, s;  
  
  //得到文件状态标志  
  flags = fcntl (sfd, F_GETFL, 0);  
  if (flags == -1)  
    {  
      perror ("fcntl");  
      return -1;  
    }  
  
  //设置文件状态标志  
  flags |= O_NONBLOCK;  
  s = fcntl (sfd, F_SETFL, flags);  
  if (s == -1)  
    {  
      perror ("fcntl");  
      return -1;  
    }  
  
  return 0;  
}  

void get_key_value(char* buf, char* key, char* value)
{
	int i,l,j;
	int seperate = 0;
	l = strlen(buf);
	for(i=0;i<l;i++)
	{
		if(buf[i] == '#')
		{
			for(j=i;j<l;j++)
			{
				buf[j] = 0;
			}
			break;
		}
		if((buf[i] == '=') && (seperate == 0))
		{
			seperate = i;
			break;
		}
	}
	for(i=0;i<seperate;i++)
	{
		key[i] = buf[i];
	}
	key[i] = 0;
	for(i=seperate+1;i<l;i++)
	{
		value[i-seperate-1]=buf[i];
	}
	value[i-seperate-1]=0;
	return;
}

void load_config(char* project_name,char* config_file)
{
	FILE *pFile;
	int i;
	char buf[1024];
	char key[1024];
	char value[1024];
	char* buff;
	int dataLen;
	
	pFile = fopen(config_file,"r");
	if(pFile == NULL)
	{
		fprintf (stderr, "Usage: %s [config file]\n", project_name);  
		exit(-1);
	}
	while(!feof(pFile))
	{
	    buff = fgets(buf,1023,pFile);
		get_key_value(buf,key,value);
		i = 0;
		while(conf[i].name)
		{
			if(strcmp(conf[i].name,key) == 0)
			{
				conf[i].FunP(value,conf[i].point);
				break;
			}
            i++;
		}
	}
}

//反转义
void anti_escape(unsigned char* buf,int length)
{
    int i;
    int j;
    int flag;

    flag = 0;
    for(i=0,j=0;i<length;i++)
    {
        if(flag)
        {
            flag = 0;
            continue;
        }
        if(buf[i] == 0x7d)
        {
            flag = 1;
            if(i+1 >= length)
            {
                return ;
            }
            if(buf[i+1] == 0x02)
            {
                buf[j] = 0x7e;
            } else if(buf[i+1] == 0x01)
            {
                buf[j] = 0x7d;
            } else
            {
                return;
            }
            j++;
        } else 
        {
            buf[j] = buf[i];
            j++;
        }
    }
}

//获取一个数据包
protocol_pkt* getPkt(Client_hash_node* client_info,
                     unsigned char* buffer,
                     int receive_data_length,
                     SQL_STRUCT* current_sql)
{
	int recv_len;
	protocol_pkt* pkt;
	int flag;
	int i;
	int start;
	int end;

    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d %d %d\n",__FUNCTION__,__LINE__,
                        receive_data_length,client_info->client->dataLength,MAX_RECV_BUF*3);
	
    threadprintbuf((unsigned char*)client_info->client->recvBuf,
            client_info->client->dataLength,current_sql);
    threadprintbuf(buffer,
            receive_data_length,current_sql);
            
    memcpy(client_info->client->recvBuf+client_info->client->dataLength,buffer,receive_data_length);
    client_info->client->dataLength += receive_data_length;
	flag = 0;
	
	
	for(i=0;i<client_info->client->dataLength;i++)
	{
		if(client_info->client->recvBuf[i] == 0x7e)
		{
			flag++;
			if(flag == 1)
			{
				start = i;
			}
		}
		if(flag >= 2)
		{
			end = i;
			pkt = (protocol_pkt*)MALLOC(end - start);
			memcpy(pkt,&client_info->client->recvBuf[start+1],end-start-1);
			//反转义
			anti_escape((unsigned char*)pkt,end-start);

            THREADDEBUGPRINT;
//            printbuf((unsigned char*)client_info->client->recvBuf,client_info->client->dataLength);
			
			memmove(client_info->client->recvBuf,&client_info->client->recvBuf[end+1],client_info->client->dataLength - end-1);
			client_info->client->dataLength = client_info->client->dataLength-end-1;
            threadprintbuf((unsigned char*)pkt,
                    end - start,current_sql);
            threadprintbuf((unsigned char*)client_info->client->recvBuf,
                    client_info->client->dataLength,current_sql);
						
//            printbuf((unsigned char*)client_info->client->recvBuf,client_info->client->dataLength);
			pkt->head.messageID     = ntohs(pkt->head.messageID);
			pkt->head.property      = ntohs(pkt->head.property);
			pkt->head.messageNum    = ntohs(pkt->head.messageNum);
			pkt->head.totalPktNum   = ntohs(pkt->head.totalPktNum);
			pkt->head.currentPktNum = ntohs(pkt->head.currentPktNum);
            THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d %d %d %d\n",__FUNCTION__,__LINE__,
                                receive_data_length,client_info->client->dataLength,MAX_RECV_BUF*3,
                                client_info->client->fd);
			return pkt;
		}
	}
	return NULL;
}


int handle_fd(int fd,Client_hash_node* client_info,SQL_STRUCT* current_sql)
{
    int done = 0;
    if(client_info == NULL || (client_info->client == NULL))
    {
        done = 1;
        THREADDEBUGPRINT;
        return done;
    }
    while (1)  
    {
      ssize_t count;  
      char buf[MAX_RECV_BUF];  
      protocol_pkt* pkt;
      pkt = NULL;
      int status;


      count = read(fd, buf, sizeof(buf));  
      if (count == -1)  
        {  
          /* If errno == EAGAIN, that means we have read all 
             data. So go back to the main loop. */  
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR))
			{
                count = 0;
                break;
			}
            perror ("read error");
            done = 1;  
            break;
        }  
      else if (count == 0)  
        {
          done = 1;
          break;
        }  
        //数据报文处理
/*        
        while(1) {
            pkt = getPkt(client_info,(unsigned char*)buf,count,current_sql);

            count = 0;

            if(pkt == NULL)
            {
                break;
            }
            threadprintbuf((unsigned char*)buf,count,current_sql);
            status = handle_pkt(pkt,client_info,current_sql);
            
            FREE(pkt);
            
            if(status == PKT_HANDLED && client_info->client->need_return)
            {
                send_pkt(client_info->client);
            }
            if(client_info->client->status == ERROR_STATUS)
            {
                done = 1;
                return done;
            }
        }*/
      /* Write the buffer to standard output */  
      
    }  
    return done;
}

void send_encrypted_data(unsigned char* data, int length, int encrypt_type,unsigned short messageID,Client* client)
{
    unsigned char tmp[(MAX_PKT_PAYLOAD_SIZE+20)*2];
    protocol_pkt pkt;
    int needTransfer;
    int i,j;
    int currentPkt;
    int pktNum;
    int fd;

    fd = client->fd;

    needTransfer = 0;

    pktNum = (length - 1)/MAX_PKT_PAYLOAD_SIZE + 1;

    for(currentPkt = 0;currentPkt < pktNum;currentPkt++)
    {
        unsigned char* ppkt;
        int endIndex;
        unsigned char checksum;
        checksum = 0;
        
        endIndex = (currentPkt + 1)*MAX_PKT_PAYLOAD_SIZE;
        if(endIndex > length)
        {
            endIndex = length;
        }
        
        pkt.head.messageID = htons(messageID);
        pkt.head.property  = htons((endIndex-currentPkt*MAX_PKT_PAYLOAD_SIZE) |
                                     ((encrypt_type & 0x07) << 10) |
                                     (1<<13));
        memcpy(pkt.head.ID,client->identifyID,PUBLIC_ID_LENGTH);
//        pkt->head.ID        = htons(client->return_type);
        pkt.head.messageNum = htons(client->recvMessageNum);
        pkt.head.totalPktNum = htons((unsigned short)pktNum);
        pkt.head.currentPktNum = htons((unsigned short)currentPkt + 1);
        i=0;
        j=0;
        tmp[j++]=0x7e;

        ppkt = (unsigned char*)(&pkt);
        for(i=0;i<sizeof(pkt);i++)
        {
            if(ppkt[i] == 0x7e)
            {
                tmp[j++]=0x7d;
                tmp[j++]=0x02;
            } else if(ppkt[i] == 0x7d)
            {
                tmp[j++]=0x7d;
                tmp[j++]=0x01;
            } else
            {
                tmp[j++]=ppkt[i];
            }
            checksum^=ppkt[i];
        }

        for(i=currentPkt*MAX_PKT_PAYLOAD_SIZE;i<endIndex;i++)
        {
            if(data[i] == 0x7e)
            {
                tmp[j++]=0x7d;
                tmp[j++]=0x02;
            } else if(data[i] == 0x7d)
            {
                tmp[j++]=0x7d;
                tmp[j++]=0x01;
            } else
            {
                tmp[j++]=data[i];
            }
            checksum^=data[i];
        }
        if(checksum == 0x7e)
        {
            tmp[j++]=0x7d;
            tmp[j++]=0x02;
        } else if(checksum == 0x7d)
        {
            tmp[j++]=0x7d;
            tmp[j++]=0x01;
        } else
        {
            tmp[j++]=checksum;
        }
        tmp[j++]=0x7e;
//        printbuf(tmp,j);
        send(fd,tmp,j,0);
    }
}

void send_RSA_key(Client_hash_node* node)
{
    make_RSA_data((unsigned char*)node->client->sendBuf);
    node->client->return_data_length = sizeof(PROTOCOL_RSA_DATA);
    node->client->return_encrypt_type = NONE;
    node->client->return_type = 0x0000;
    node->client->need_return = 1;
    send_pkt(node->client);
}

int send_pkt(Client* client)
{
    int fd;
    char tmpbuf[MAX_SEND_BUF*10];
    int final_length;
    int* tmp;
    
    fd = client->fd;
#if ENCRYPTDATA
//    client->return_encrypt_type = NONE;
#endif
    
    switch(client->return_encrypt_type)
    {
        case NONE:
            //send_final_pkt(fd,client->sendBuf,client->return_data_length,client);
            send_encrypted_data((unsigned char*)client->sendBuf,
                                client->return_data_length,
                                client->return_encrypt_type,
                                client->return_type,
                                client);
            break;
        case TYPE_RSA:
            tmp = (int*)(client->sendBuf - sizeof(int));
            *tmp = htonl(client->return_data_length);
            final_length = RSA_encrypt((char*)tmp,tmpbuf,client->return_data_length+sizeof(int));
            if(final_length == RSA_ERROR)
            {
                close(fd);
                DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"2close fd:%d\n",fd);
                rm_client_hash_node_by_fd(fd);
            }
            send_encrypted_data((unsigned char*)tmpbuf,
                                final_length,
                                client->return_encrypt_type,
                                client->return_type,
                                client);
            break;
        case TYPE_AES:

            
        key_expansion(client->AESKey,
                      client->AESKeyW,
                      16);
            
            final_length = AES_encrypt((unsigned char*)client->sendBuf,(unsigned char*)tmpbuf,client->return_data_length,client->AESKeyW);

            

                DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"aes encrypt:%s %d %d %s\n",__FUNCTION__,__LINE__,fd,client->sendBuf);
            printbuf(client->AESKey,32);
            printbuf(client->sendBuf,client->return_data_length);
            printbuf(tmpbuf,final_length);
            
            
            send_encrypted_data((unsigned char*)tmpbuf,
                                final_length,
                                client->return_encrypt_type,
                                client->return_type,
                                client);
            break;
    }
}

//
void* handle_client_info_thread(void* arg,SQL_STRUCT* current_sql)
{
    Client_hash_node* client_info;

    int done;
    int fd;

    client_info = (Client_hash_node*) arg;
    
    if(client_info->client->client_over)
    {
        return NULL;
    }
    
    fd = client_info->client->fd;
    
    done = handle_fd(fd,client_info,current_sql);

    if (done)
    {
#if 0
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"Closed connection on descriptor %d\n",  
                        fd);

        close (fd);
        /* Closing the descriptor will make epoll remove it 
        from the set of descriptors which are monitored. */  
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"1close fd:%d\n",fd);
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"1close fd:%d\n",fd);
        rm_client_hash_node_by_fd(fd);
        return NULL;
#endif
        client_info->client->client_over = 1;
    }

    if(client_info->client->status == LOGINED_STATUS)
    {
        client_info->client->time = seconds + LOGINED_CLIENT_TIME_OUT_SECOND;
    }
    else
    {
        client_info->client->time = seconds + NO_LOGIN_CLIENT_TIME_OUT_SECOND;
    }
out:
    client_info->client->in_thread_flag = false;
    return NULL;
}

int handle_client_info(int fd,Client_hash_node* client_info)
{
    if(client_info->client->in_thread_flag || client_info->client->client_over)
    {
        return 0;
    }
    client_info->client->in_thread_flag = 1;
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"add to thread pool %d,%d,%lx\n",  
                        client_info->client->fd,fd,client_info);
    tpool_add_work(handle_client_info_thread,client_info);
    return 1;
}

void should_close_socket(Client* client)
{
    if(client->in_thread_flag)
    {
        return;
    }
    if(client->time<=seconds || client->client_over)
    {
        close(client->fd);
        DEBUGPRINT;
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d\n",  
                            client->connectInfo,client->status);

        if(client->time<=seconds)
        {
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"close socket cause time out\n");
        }
        rm_client_hash_node_by_fd(client->fd);
    }
}

void search_epoll_should_close_socket_and_handle()
{
    for_every_client_connected(should_close_socket);
}

extern int Client_num;

//
void*  
epoll ()
{  
    int sfd, s;  
    int efd;
    struct epoll_event event;  
    struct epoll_event *events;
    time_t last_check_time;

    sfd = create_and_bind ();
    if (sfd == -1)  
        abort ();  

    if(clients_hash_init(max_events)==-1)
    {
        abort();
    };

    s = make_socket_non_blocking (sfd);  
    if (s == -1)  
        abort ();  

    s = listen (sfd, max_so_connect);  

    fprintf(stderr,"max_so_connect:%d\n",max_so_connect);
    
    if (s == -1)  
    {  
        perror ("listen");  
        abort ();  
    }  

    //除了参数size被忽略外,此函数和epoll_create完全相同  
    efd = epoll_create1 (0);  
    if (efd == -1)  
    {  
        perror ("epoll_create");  
        abort ();  
    }  

    event.data.fd = sfd;  
    //EPOLLIN 数据写入触发，需要一直读取到eagain
    //EPOLLET 边沿触发
    event.events = EPOLLIN; //| EPOLLET;//读入
    s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);  
    if (s == -1)  
    {  
        perror ("epoll_ctl");  
        abort ();  
    }  

    /* Buffer where events are returned */  
    events = (struct epoll_event *)calloc (max_events, sizeof event);

    last_check_time = time((time_t*)NULL);
    
    /* The event loop */  
    while (1)  
    {  
        int n, i;
        n = epoll_wait (efd, events, max_events, NO_LOGIN_CLIENT_TIME_OUT_SECOND*1000);
        
        seconds = time((time_t*)NULL);

        //每过一段时间清理
        if(seconds - last_check_time >= (NO_LOGIN_CLIENT_TIME_OUT_SECOND/2))
        {
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"check time out %ld %ld\n",seconds,last_check_time);
            //断开超时设备以及需要关闭的设备的连接
            search_epoll_should_close_socket_and_handle();
            last_check_time = seconds;
        }
        
        for (i = 0; i < n; i++)  
        {
            if ((events[i].events & EPOLLERR) ||  
                (events[i].events & EPOLLHUP) ||  
                (!(events[i].events & EPOLLIN)))  
            {  
                /* 错误 */
                Client_hash_node* client_info = NULL;
                client_info = get_client_hash_node_by_fd(events[i].data.fd);
                if(client_info == NULL)
                {
                    close(events[i].data.fd);
                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"3close fd:%d\n",events[i].data.fd);
                    continue;
                }
                if(client_info->client->in_thread_flag == 0)
                {
                    close (events[i].data.fd);
                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"4close fd:%d\n",events[i].data.fd);
                    rm_client_hash_node_by_fd(events[i].data.fd);
                    continue;
                }
                continue;  
            }
            
            else if (sfd == events[i].data.fd)  
            {
                /* 新连接. */  
                while (1)  
                {
                    struct sockaddr in_addr;  
                    socklen_t in_len;  
                    int infd;  
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];  

                    in_len = sizeof in_addr;  
                    infd = accept (sfd, &in_addr, &in_len);  

                    if(infd <= 2)
                    {
                        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,
                                "%s %d accept error %d %d %d\n",__FUNCTION__,__LINE__,errno,sfd,infd);

                        //之前忘记什么原因，触发了一个神奇的bug，这个是为了规避这个神奇的bug
#if 0
                        if(errno == EBADF)
                        {

                            //将地址转化为主机名或者服务名  
                            s = getnameinfo (&in_addr, in_len,
                                            hbuf, sizeof hbuf,
                                            sbuf, sizeof sbuf,
                                            NI_NUMERICHOST | NI_NUMERICSERV);
                            //主机地址和服务地址
                            if (s == 0)
                            {
                                DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"Deny connection on descriptor %d "
                                "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                            }
                            continue;
                        }
#else
                        continue;
#endif
                    }
                    
                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,
                            "%s %d new connect %d\n",__FUNCTION__,__LINE__,sfd);
                    if (infd == -1)
                    {
                        if ((errno == EAGAIN) ||
                        (errno == EWOULDBLOCK))
                        {
                            DEBUGPRINT
                            /* 没空间了*/
                            break;
                        }
                        else
                        {
                            DEBUGPRINT
                            perror ("accept");
                            break;
                        }
                    }

                    //将地址转化为主机名或者服务名  
                    s = getnameinfo (&in_addr, in_len,
                                    hbuf, sizeof hbuf,
                                    sbuf, sizeof sbuf,
                                    NI_NUMERICHOST | NI_NUMERICSERV);//flag参数:以数字名返回
                    //主机地址和服务地址
                    if (s == 0)
                    {
                        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"Accepted connection on descriptor %d "
                        "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                    /* Make the incoming socket non-blocking and add it to the 
                    list of fds to monitor. */  
                    s = make_socket_non_blocking (infd);  

                    if (s == -1)
                    {
                        abort ();
                    }
                    //hehe
                    s = make_socket_no_time_wait (infd);  

                    if (s == -1)
                    {
                        abort ();
                    }

                    event.data.fd = infd;  
                    event.events = EPOLLIN ;//| EPOLLET;

                    add_fd_to_client_hash(event.data.fd,seconds);
                    
                    Client_hash_node* client_hash_node;

                    client_hash_node = get_client_hash_node_by_fd(event.data.fd);

                    snprintf(client_hash_node->client->connectInfo,2000,
                        "Accepted connection on descriptor %d "
                        "(host=%s, port=%s)", infd, hbuf, sbuf);

                    client_hash_node->client->connectInfo[2000] = 0;

                    client_hash_node->client->time = seconds + NO_LOGIN_CLIENT_TIME_OUT_SECOND * 2;
                    
                    send_RSA_key(client_hash_node);


                    s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);  
                    if (s == -1)  
                    {
                        perror ("epoll_ctl");  
                        abort ();  
                    }
                    
                }  
                continue;  
            }  
            else  
            {
                //来数据了
                int done = 0;  
                int fd = events[i].data.fd;
                
                Client_hash_node* client_info = NULL;
                
                pthread_mutex_lock(&hash_node_lock);
                
                client_info = get_client_hash_node_by_fd(fd);
                if(client_info == NULL)
                {
                    //在极低概率下，刚准备处理此fd，
                    //然后前一个处理此fd的线程就已经将其清理了，此处不需要再进行清理
                    
                    //didn't find any node
//                    close(fd);
//                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"3close fd:%d\n",fd);
//                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"ERROR!!! should not come to here %s %d!!!\n",__FUNCTION__,__LINE__);
                    pthread_mutex_unlock(&hash_node_lock);
//                    rm_client_hash_node_by_fd(fd);
                    continue;
                }

                handle_client_info(fd,client_info);
                pthread_mutex_unlock(&hash_node_lock);
#if 0
                //非线程池处理方式
                done = handle_fd(fd,client_info);
                if (done)  
                {  
                    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"Closed connection on descriptor %d\n",  
                                    fd);

                    /* Closing the descriptor will make epoll remove it 
                    from the set of descriptors which are monitored. */  
                    rm_client_hash_node_by_fd(fd);
                    close (fd);
                }
#endif
            }
        }
        usleep(300*1000);
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"running Client_num:%d\n",Client_num);
    }

    FREE (events);  

    close (sfd);  

    return EXIT_SUCCESS;  
}  
