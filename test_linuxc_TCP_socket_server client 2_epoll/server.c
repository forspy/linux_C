/******************************************************************************

                  ��Ȩ���� (C), 2017-2017, 

 ******************************************************************************
  �� �� ��   : server.c
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2017��7��11��
  ����޸�   :
  ��������   : ����˺���
  �����б�   :
*
*       1.                SRV_accpet_process
*       2.                SRV_creat_proc_server
*       3.                SRV_handle_client
*       4.                SRV_init
*       5.                SRV_msg_process
*       6.                SRV_recv_msg
*       7.                SRV_release
*
  �޸���ʷ   :
  1.��    ��   : 2017��7��11��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/
#include "server.h"

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
	//�������Ķ���
struct server_obj *g_ptSrvOBJ;




/*****************************************************************************
 �� �� ��  : SRV_init
 ��������  : �������ĳ�ʼ��,�������� ��ʼ������
 �������  : ��
 �������  : ��
 �� �� ֵ  : int 
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
int  SRV_init( )
{
   unsigned int i=0;
   
   g_ptSrvOBJ = (struct server_obj *)malloc(sizeof(struct server_obj));
   
   if(g_ptSrvOBJ ==NULL)
   {
	  return -1;
   }
   memset(g_ptSrvOBJ,0,sizeof(struct server_obj));
   
   g_ptSrvOBJ->current_clicnt = 0;
   /*��ʼ�����Ӷ���*/
   for(i=0;i<LISTSIZE;i++)
   {
	 g_ptSrvOBJ->clifds[i] = -1;
   }
   
   return 0;
}


/*****************************************************************************
 �� �� ��  : SRV_creat_proc_server
 ��������  : int
 �������  : const char *ip
             int port
 �������  : ��
 �� �� ֵ  : int
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
int SRV_creat_proc_server( const char *ip, int port )
{
   int  fd;
   fd=socket(AF_INET,SOCK_STREAM,0);
   
   if(fd==-1)
   {
	 printf("SRV_creat_proc_server socket error\n");
	 return -1;
   }
   
   //���÷������ĵ�ַ
   bzero(&g_ptSrvOBJ->seraddr,sizeof(g_ptSrvOBJ->seraddr));
   g_ptSrvOBJ->seraddr.sin_family = AF_INET;
   inet_pton(AF_INET,ip,&g_ptSrvOBJ->seraddr.sin_addr);
   g_ptSrvOBJ->seraddr.sin_port =htons(port);

   //�󶨵�ַ
   if(bind(fd,(struct sockaddr*)&(g_ptSrvOBJ->seraddr),sizeof(g_ptSrvOBJ->seraddr))==-1)
   {
   		perror("bind error\n");
		return -1;
   }

   //��������������
   listen(fd,LISTSIZE);
   
   return fd;
}


/*****************************************************************************
 �� �� ��  : SRV_release
 ��������  : �������ͷź���
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
void SRV_release( void )
{
    if(g_ptSrvOBJ)
    {
		free(g_ptSrvOBJ);
		g_ptSrvOBJ=NULL;
	}
}

/*****************************************************************************
 �� �� ��  : SRV_msg_process
 ��������  : �����߼�����ģ��
 �������  : int fd
             char *buf
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
static int SRV_msg_process( int fd, char *buf )
{
	/*���������*/
    assert(buf);
	printf("recv data:%s\n",buf);
	write(fd,buf,strlen(buf)+1);
	return 0;
	
}

/*****************************************************************************
 �� �� ��  : SRV_recv_msg
 ��������  : ������ͨ�ź���
 �������  : fd_set *readfds
 �������  : ��
 �� �� ֵ  : static void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
static void SRV_recv_msg( fd_set *readfds )
{
    unsigned int ulI = 0;
	int iRet=0;
	int clifd;
	char buf[BUFFSIZE]={0};
	for (ulI = 0; ulI < = g_ptSrvOBJ->current_clicnt; ulI++ )
	{
	    clifd = g_ptSrvOBJ->clifds[ulI];
		if(clifd <0)
		{
			continue;
		}
		/*�жϴ������Ƿ����µ����ݿɶ�*/
		if(FD_ISSET(clifd,readfds))
		{
			iRet = read(clifd,buf,BUFFSIZE);
			if(iRet <=0)
			{
				/*����Ϊ�� ����رտͻ���*/
				FD_CLR(clifd,&g_ptSrvOBJ->srv_fds);
				close(clifd);
				g_ptSrvOBJ->clifds[ulI] = -1;
				g_ptSrvOBJ->current_clicnt--;
				continue;
			}
			/*���ݵ��߼�����ģ��*/  
			SRV_msg_process(clifd,buf);
		}
	}
}


/*****************************************************************************
 �� �� ��  : SRV_accpet_process
 ��������  : ����������
 �������  : int srvfd
 �������  : ��
 �� �� ֵ  : static int
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��11��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
static int SRV_accpet_process( int srvfd )
{
	struct sockaddr_in tCliaddr;
	socklen_t 		   tCliaddlen;
	tCliaddlen = sizeof(tCliaddr);
	int confd  = -1;
	unsigned int i =0;
ACCEPT:
    /*�����µ�����*/
	confd=accept(srvfd,(struct sockaddr*)(&tCliaddr),&tCliaddlen);
	if(confd == -1)
	{
		if(errno==EINTR)
		{
			goto ACCEPT;
		}
		else
		{
			fprintf(stderr,"accept error,error:%s\n",strerror(errno));
			return -1;
		}
	}
	/*���Ի�ȡ�µ�����*/
	fprintf(stdout,"accept new client:%s:%d\n",inet_ntoa(tCliaddr.sin_addr),tCliaddr.sin_port);

	/*���µľ����ӵ�fd_set*/
	for(i=0;i<LISTSIZE;i++)
	{
		/*�ҵ���λ*/
		if(g_ptSrvOBJ->clifds[i]<0)
		{
			g_ptSrvOBJ->clifds[i] = confd;
			g_ptSrvOBJ->current_clicnt++;
			break;
		}
	}

	if(LISTSIZE == i)
	{
		/*��������*/
		fprintf(stderr,"Max client.\n");
		return -1;
	}
	return 1;
}

/*****************************************************************************
 �� �� ��  : SRV_handle_client
 ��������  : ����������ͻ�������
 �������  : int srvfd
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2017��7��9��
    ��    ��   : 
    �޸�����   : �����ɺ���

*****************************************************************************/
void SRV_handle_client( int srvfd )
{
	int iRet=0;
	fd_set *readfds=&(g_ptSrvOBJ->srv_fds);
	struct timeval tv;
	unsigned int i =0;
	int clifd=-1;
    while(1)
    {
    	//��ʼ���ļ���������
    	FD_ZERO(readfds);
		//�����ļ�����������
		FD_SET(srvfd,readfds);
		g_ptSrvOBJ->max_fd=srvfd;
		
		//����SELECT��ʱʱ��
		tv.tv_sec=30;
		tv.tv_usec=0;
		
		/*�������ӵĿͻ������Ӿ���������ļ����������*/
		for(i=0;i<g_ptSrvOBJ->current_clicnt;i++)
		{
   			clifd = g_ptSrvOBJ->clifds[i];
   			/*�Ƿ�Ϊ���Ӿ��*/
			if(clifd!=-1)
			{
				FD_SET(clifd,readfds);
			}
			/*���������ֵ*/
			 g_ptSrvOBJ->max_fd = (clifd>g_ptSrvOBJ->max_fd ?clifd:  g_ptSrvOBJ->max_fd);
		}
		
		/*select��ʼ��ѵ����������ļ�����������Ӿ��*/
		iRet=select(g_ptSrvOBJ->max_fd+1,readfds,NULL,NULL,&tv);
		if(iRet ==-1)
		{
			perror("select error\n");
			return;
		}
		
		if(iRet ==0)
		{
			fprintf(stdout,"select is timeout.\n");
			continue;
		}
		
		/*�жϵ�ǰ�Ƿ�Ϊ�������*/
		if(FD_ISSET(srvfd,readfds))
		{
			/*����accpet����*/
			SRV_accpet_process(srvfd);//����ʹ��accept()�������ܿͻ�����Ϣ
		}
		else
		{
			/*����read����*/
			SRV_recv_msg(readfds);
		}
	}
}
