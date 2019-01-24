/******************************************************************************

                  版权所有 (C), 2017-2017, 

 ******************************************************************************
  文 件 名   : server.c
  版 本 号   : 初稿
  作    者   : 
  生成日期   : 2017年7月11日
  最近修改   :
  功能描述   : 服务端函数
  函数列表   :
*
*       1.                SRV_accpet_process
*       2.                SRV_creat_proc_server
*       3.                SRV_handle_client
*       4.                SRV_init
*       5.                SRV_msg_process
*       6.                SRV_recv_msg
*       7.                SRV_release
*
  修改历史   :
  1.日    期   : 2017年7月11日
    作    者   : 
    修改内容   : 创建文件

******************************************************************************/
#include "server.h"

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
	//服务器的对象
struct server_obj *g_ptSrvOBJ;




/*****************************************************************************
 函 数 名  : SRV_init
 功能描述  : 服务器的初始化,创建对象 初始化对象
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月9日
    作    者   : 
    修改内容   : 新生成函数

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
   /*初始化链接队列*/
   for(i=0;i<LISTSIZE;i++)
   {
	 g_ptSrvOBJ->clifds[i] = -1;
   }
   
   return 0;
}


/*****************************************************************************
 函 数 名  : SRV_creat_proc_server
 功能描述  : int
 输入参数  : const char *ip
             int port
 输出参数  : 无
 返 回 值  : int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月9日
    作    者   : 
    修改内容   : 新生成函数

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
   
   //设置服务器的地址
   bzero(&g_ptSrvOBJ->seraddr,sizeof(g_ptSrvOBJ->seraddr));
   g_ptSrvOBJ->seraddr.sin_family = AF_INET;
   inet_pton(AF_INET,ip,&g_ptSrvOBJ->seraddr.sin_addr);
   g_ptSrvOBJ->seraddr.sin_port =htons(port);

   //绑定地址
   if(bind(fd,(struct sockaddr*)&(g_ptSrvOBJ->seraddr),sizeof(g_ptSrvOBJ->seraddr))==-1)
   {
   		perror("bind error\n");
		return -1;
   }

   //开启服务器监听
   listen(fd,LISTSIZE);
   
   return fd;
}


/*****************************************************************************
 函 数 名  : SRV_release
 功能描述  : 服务器释放函数
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月9日
    作    者   : 
    修改内容   : 新生成函数

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
 函 数 名  : SRV_msg_process
 功能描述  : 数据逻辑处理模块
 输入参数  : int fd
             char *buf
 输出参数  : 无
 返 回 值  : static int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月11日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
static int SRV_msg_process( int fd, char *buf )
{
	/*做参数检测*/
    assert(buf);
	printf("recv data:%s\n",buf);
	write(fd,buf,strlen(buf)+1);
	return 0;
	
}

/*****************************************************************************
 函 数 名  : SRV_recv_msg
 功能描述  : 新链接通信函数
 输入参数  : fd_set *readfds
 输出参数  : 无
 返 回 值  : static void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月11日
    作    者   : 
    修改内容   : 新生成函数

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
		/*判断此链接是否有新的数据可读*/
		if(FD_ISSET(clifd,readfds))
		{
			iRet = read(clifd,buf,BUFFSIZE);
			if(iRet <=0)
			{
				/*数据为空 代表关闭客户端*/
				FD_CLR(clifd,&g_ptSrvOBJ->srv_fds);
				close(clifd);
				g_ptSrvOBJ->clifds[ulI] = -1;
				g_ptSrvOBJ->current_clicnt--;
				continue;
			}
			/*数据的逻辑处理模块*/  
			SRV_msg_process(clifd,buf);
		}
	}
}


/*****************************************************************************
 函 数 名  : SRV_accpet_process
 功能描述  : 处理监听句柄
 输入参数  : int srvfd
 输出参数  : 无
 返 回 值  : static int
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月11日
    作    者   : 
    修改内容   : 新生成函数

*****************************************************************************/
static int SRV_accpet_process( int srvfd )
{
	struct sockaddr_in tCliaddr;
	socklen_t 		   tCliaddlen;
	tCliaddlen = sizeof(tCliaddr);
	int confd  = -1;
	unsigned int i =0;
ACCEPT:
    /*接收新的链接*/
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
	/*可以获取新的链接*/
	fprintf(stdout,"accept new client:%s:%d\n",inet_ntoa(tCliaddr.sin_addr),tCliaddr.sin_port);

	/*把新的句柄添加到fd_set*/
	for(i=0;i<LISTSIZE;i++)
	{
		/*找到空位*/
		if(g_ptSrvOBJ->clifds[i]<0)
		{
			g_ptSrvOBJ->clifds[i] = confd;
			g_ptSrvOBJ->current_clicnt++;
			break;
		}
	}

	if(LISTSIZE == i)
	{
		/*链接已满*/
		fprintf(stderr,"Max client.\n");
		return -1;
	}
	return 1;
}

/*****************************************************************************
 函 数 名  : SRV_handle_client
 功能描述  : 服务器处理客户端请求
 输入参数  : int srvfd
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2017年7月9日
    作    者   : 
    修改内容   : 新生成函数

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
    	//初始化文件描述符集
    	FD_ZERO(readfds);
		//设置文件描述符集合
		FD_SET(srvfd,readfds);
		g_ptSrvOBJ->max_fd=srvfd;
		
		//设置SELECT超时时间
		tv.tv_sec=30;
		tv.tv_usec=0;
		
		/*把新链接的客户端连接句柄，放入文件句柄集合中*/
		for(i=0;i<g_ptSrvOBJ->current_clicnt;i++)
		{
   			clifd = g_ptSrvOBJ->clifds[i];
   			/*是否为链接句柄*/
			if(clifd!=-1)
			{
				FD_SET(clifd,readfds);
			}
			/*更新最大句柄值*/
			 g_ptSrvOBJ->max_fd = (clifd>g_ptSrvOBJ->max_fd ?clifd:  g_ptSrvOBJ->max_fd);
		}
		
		/*select开始轮训处理服务器的监听句柄和链接句柄*/
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
		
		/*判断当前是否为监听句柄*/
		if(FD_ISSET(srvfd,readfds))
		{
			/*处理accpet程序*/
			SRV_accpet_process(srvfd);//里面使用accept()函数接受客户端消息
		}
		else
		{
			/*处理read程序*/
			SRV_recv_msg(readfds);
		}
	}
}
