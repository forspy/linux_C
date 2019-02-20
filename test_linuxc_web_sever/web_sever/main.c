#include "wrap.h" 
#include "parse.h"

#define PID_FILE  "pid.file"

static void doit(int fd);
static void writePid(int option);
static void get_requesthdrs(rio_t *rp);
static void post_requesthdrs(rio_t *rp,int *length);
static int parse_uri(char *uri, char *filename, char *cgiargs);
static void serve_static(int fd, char *filename, int filesize);
static void serve_dir(int fd,char *filename);
static void get_filetype(const char *filename, char *filetype);
static void get_dynamic(int fd, char *filename, char *cgiargs);
static void post_dynamic(int fd, char *filename, int contentLength,rio_t *rp); 
static void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

static void sigChldHandler(int signo);
/* $begin ssl */
#ifdef HTTPS 
static void ssl_init(void);
static void https_getlength(char* buf,int* length);
#endif

/* $end ssl */


static int isShowdir=1;
char* cwd;

#ifdef  HTTPS
static SSL_CTX* ssl_ctx;
static SSL* ssl;
static char* certfile;
static int ishttps=0;
static char httpspostdata[MAXLINE];
#endif


//面向对象，模块化编程
int main(int argc, char **argv) 
{
    int listenfd,connfd, port,clientlen;
    pid_t pid;
    struct sockaddr_in clientaddr;
    char isdaemon=0,*portp=NULL,*logp=NULL,tmpcwd[MAXLINE];

    #ifdef HTTPS 
    int sslport;
    char dossl=0,*sslportp=NULL;
    #endif

    openlog(argv[0],LOG_NDELAY|LOG_PID,LOG_DAEMON);	//打开日志系统， argv[0]为字符参数 日志存放在/ect/rsyslog.d/50-default.conf  表示当前路径下的文件名
    /* void openlog(const char *ident, int option, int facility);*/
    //需要用例如 syslog(LOG_INFO,"information message,pid=%d",getpid());//LOG_INFO表示级别
    cwd=(char*)get_current_dir_name();	
    strcpy(tmpcwd,cwd);
    strcat(tmpcwd,"/");
    /* parse argv */

    #ifdef  HTTPS
    parse_option(argc,argv,&isdaemon,&portp,&logp,&sslportp,&dossl);
    sslportp==NULL ?(sslport=atoi(Getconfig("https"))) : (sslport=atoi(sslportp));

    if(dossl==1||strcmp(Getconfig("dossl"),"yes")==0)
    	dossl=1;
    #else
    parse_option(argc,argv,&isdaemon,&portp,&logp);//信号解析
    #endif

    portp==NULL ?(port=atoi(Getconfig("http"))) : (port=atoi(portp));


    Signal(SIGCHLD,sigChldHandler);//使用sigChldHandler回调函数处理
//配置解析模块
//读取配置，根据配置进行系统解析
    /* init log */
    if(logp==NULL) 
    	logp=Getconfig("log");//字符串解析方法 传入log 模式 解析“log=access.log”
    initlog(strcat(tmpcwd,logp));//配置解析的过程

    /* whethe show dir */
    if(strcmp(Getconfig("dir"),"no")==0)
    	isShowdir=0;	
    	

    clientlen = sizeof(clientaddr);


    if(isdaemon==1||strcmp(Getconfig("daemon"),"yes")==0)//如果比对daemon是yes的话就启动Daemon守护进程
    	Daemon(1,1);//守护进程

    writePid(1);//写pid进程文件
    //例如ps -A|grep 3601     #grep是一种强大的文本搜索工具，它能使用正则表达式搜索文本，并把匹配的行打印出来。
//正则表达式通常被用来检索、替换那些符合某个模式(规则)的文本。  grep能够通过搜索部分关键子搜索文本
    /* $https start  */ 
    #ifdef HTTPS 
    if(dossl)//如果配置文件里面dossl=yes并且define了HTTPS则执行https协议
    {
    	if((pid=Fork())==0)//创建一个子进程，子进程为0，执行下面模块 >0为当前父进程(主进程)，执行其他模块
	{
		listenfd= Open_listenfd(sslport);//开启监听
        
		ssl_init();//ssl加密套接字初始化

		while(1)
		{
			connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);//接收客户端的连接
			if(access_ornot(inet_ntoa(clientaddr.sin_addr))==0)
			{
				clienterror(connfd,"maybe this web server not open to you!" , "403", "Forbidden", "Tiny couldn't read the file");
				continue;
			}

			if((pid=Fork())>0)
			{
				Close(connfd);
				continue;
			}
			else if(pid==0)
			{
				ishttps=1;	
				doit(connfd);//tcp和ssl套接字的绑定
				exit(1);
			}
		}
	}
    }
    #endif

    /* $end https */


    listenfd = Open_listenfd(port);
    while (1)
    {
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
	if(access_ornot(inet_ntoa(clientaddr.sin_addr))==0)//access_ornot()为web服务器的访问控制列表 功能为：让哪些ip访问服务器 不让哪些ip访问服务器 看看clientaddr.sin_addr在白名单还是黑名单
	{
	    //阶段：在SYN之后  决定是否发回SYN,ACK
	    clienterror(connfd,"maybe this web server not open to you!" , "403", "Forbidden", "Tiny couldn't read the file");
	    continue;
	}

	if((pid=Fork())>0)
	{
		Close(connfd);
		continue;
	}
	else if(pid==0)
	{
		doit(connfd);
		exit(1);
	}
    }
}
/* $end main */


/*$sigChldHandler to protect zimble process */
static void sigChldHandler(int signo)//使用sigChldHandler回调函数处理
{
	Waitpid(-1,NULL,WNOHANG);
	return;
}
/*$end sigChldHandler */


/* $begin ssl init  */
#ifdef HTTPS 
static void ssl_init(void)//ssl加密套接字协议
{
	static char crypto[]="RC4-MD5";
	certfile=Getconfig("ca");
//加载加密算法----
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	//-----
	ssl_ctx = SSL_CTX_new( SSLv23_server_method() );//创建CTX

	if ( certfile[0] != '\0' )
	    //创建证书文件
		if ( SSL_CTX_use_certificate_file( ssl_ctx, certfile, SSL_FILETYPE_PEM ) == 0 || SSL_CTX_use_PrivateKey_file( ssl_ctx, certfile, SSL_FILETYPE_PEM ) == 0 /*载入私钥*/|| SSL_CTX_check_private_key( ssl_ctx ) == 0 /*校验证书私钥*/)
		{
			ERR_print_errors_fp( stderr );
			exit( 1 );
		}
		if ( crypto != (char*) 0 )
		{

			if ( SSL_CTX_set_cipher_list( ssl_ctx, crypto ) == 0 )//设置加密列表
			{
				ERR_print_errors_fp( stderr );
				exit( 1 );
			}
		}

}
#endif
/* $end ssl init */


/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
static void doit(int fd) //绑定ssl和tcp
{
    int is_static,contentLength=0,isGet=1;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE],httpspostdata[MAXLINE];
    rio_t rio;

    memset(buf,0,MAXLINE);	
    
    #ifdef HTTPS 
    if(ishttps)
    {
        ssl=SSL_new(ssl_ctx);
	    SSL_set_fd(ssl,fd);//进行ssl和socket fd绑定
    	if(SSL_accept(ssl)==0)//等待客户端的ssl connect
    	{
    		ERR_print_errors_fp(stderr);
    		exit(1);
    	}
	    SSL_read(ssl,buf,sizeof(buf));//利用openssl库对发过来的数据进行解密
    }
    else
    #endif
    {
    	/* Read request line and headers */
        Rio_readinitb(&rio, fd);
    	Rio_readlineb(&rio, buf, MAXLINE);
    }
    //根据http协议所规定 
    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")!=0&&strcasecmp(method,"POST")!=0) 
    { 
       clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
        return;
    }

    
    /* Parse URI from GET request */
    is_static = parse_uri(uri, filename, cgiargs);
    

    if (lstat(filename, &sbuf) < 0) 
    {   
	    clienterror(fd, filename, "404", "Not found",
		    "Tiny couldn't find this file");
	return;
    }


    if(S_ISDIR(sbuf.st_mode)&&isShowdir)
    	serve_dir(fd,filename);


    if (strcasecmp(method, "POST")==0) 
    	isGet=0;

    //普通的http https的访问
    if (is_static) 
    { /* Serve static content */

        #ifdef HTTPS 
    	    if(!ishttps) 
    	#endif
        		get_requesthdrs(&rio);  /* because https already read the headers -> SSL_read()  */

    	if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) 
    	{
    	    clienterror(fd, filename, "403", "Forbidden",
    			"Tiny couldn't read the file");
    	    return;
    	}
    	serve_static(fd, filename, sbuf.st_size);
    }
    //cgi模式的访问
    else 
    { /* Serve dynamic content */
    	if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) 
    	{
    	    clienterror(fd, filename, "403", "Forbidden",
    			"Tiny couldn't run the CGI program");
    	    return;
    	}

    	if(isGet)
    	{
    		#ifdef HTTPS 
    		if(!ishttps)
    		#endif
    			get_requesthdrs(&rio);   /* because https already read headers by SSL_read() */

    		get_dynamic(fd, filename, cgiargs);
    	}
    	else
    	{
    		#ifdef HTTPS 
    		if(ishttps)
    			https_getlength(buf,&contentLength);
    		else
    		#endif
    			post_requesthdrs(&rio,&contentLength);

    		post_dynamic(fd, filename,contentLength,&rio);
    	}
    }
}
/* $end doit */

/* $begin https_getlength */
#ifdef HTTPS 
static void https_getlength(char* buf,int* length)
{
	char *p,line[MAXLINE];
	char *tmpbuf=buf;
	int lengthfind=0;

	while(*tmpbuf!='\0')
	{
		p=line;
		while(*tmpbuf!='\n' && *tmpbuf!='\0')
			*p++=*tmpbuf++;
		*p='\0';
		if(!lengthfind)
		{
			if(strncasecmp(line,"Content-Length:",15)==0)
			{
				p=&line[15];	
				p+=strspn(p," \t");
				*length=atoi(p);
				lengthfind=1;
			}
		}

		if(strncasecmp(line,"\r",1)==0)
		{
			strcpy(httpspostdata,++tmpbuf);
			break;
		}
		++tmpbuf;
	}
	return;
}
#endif
/* $end https_getlength  */

/*
 * read_requesthdrs - read and parse HTTP request headers
 */
/* $begin get_requesthdrs */
static void get_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    writetime();  /* write access time in log file */
    //printf("%s", buf);
    while(strcmp(buf, "\r\n")) 
    {
	Rio_readlineb(rp, buf, MAXLINE);
	writelog(buf);
	//printf("%s", buf);
    }
    return;
}
/* $end get_requesthdrs */

/* $begin post_requesthdrs */
static void post_requesthdrs(rio_t *rp,int *length) 
{
    //1.接收当前http请求的头
    //2.通过解析请求头中的长度选项，解析出http请求的数据长度
    char buf[MAXLINE];
    char *p;

    Rio_readlineb(rp, buf, MAXLINE);
    writetime();  /* write access time in log file */
    while(strcmp(buf, "\r\n")) 
    {
	Rio_readlineb(rp, buf, MAXLINE);
	if(strncasecmp(buf,"Content-Length:",15)==0)
	{
		p=&buf[15];	
		p+=strspn(p," \t");
		*length=atol(p);
	}
	writelog(buf);
	//printf("%s", buf);
    }
    return;
}
/* $end post_requesthdrs */


/* $begin post_dynamic */
static void serve_dir(int fd,char *filename)
{
	DIR *dp;
	struct dirent *dirp;
    	struct stat sbuf;
	struct passwd *filepasswd;
	int num=1;
	char files[MAXLINE],buf[MAXLINE],name[MAXLINE],img[MAXLINE],modifyTime[MAXLINE],dir[MAXLINE];
	char *p;

	/*
	* Start get the dir   
	* for example: /home/yihaibo/kerner/web/doc/dir -> dir[]="dir/";
	*/
	p=strrchr(filename,'/');
	++p;
	strcpy(dir,p);
	strcat(dir,"/");
	/* End get the dir */

	if((dp=opendir(filename))==NULL)
		syslog(LOG_ERR,"cannot open dir:%s",filename);

    sprintf(files, "<html><title>Dir Browser</title>");
	sprintf(files,"%s<style type=""text/css""> a:link{text-decoration:none;} </style>",files);
	sprintf(files, "%s<body bgcolor=""ffffff"" font-family=Arial color=#fff font-size=14px>\r\n", files);

	while((dirp=readdir(dp))!=NULL)
	{
		if(strcmp(dirp->d_name,".")==0||strcmp(dirp->d_name,"..")==0)
			continue;
		sprintf(name,"%s/%s",filename,dirp->d_name);
		Stat(name,&sbuf);
		filepasswd=getpwuid(sbuf.st_uid);

		if(S_ISDIR(sbuf.st_mode))
		{
			sprintf(img,"<img src=""dir.png"" width=""24px"" height=""24px"">");
		}
		else if(S_ISFIFO(sbuf.st_mode))
		{
			sprintf(img,"<img src=""fifo.png"" width=""24px"" height=""24px"">");
		}
		else if(S_ISLNK(sbuf.st_mode))
		{
			sprintf(img,"<img src=""link.png"" width=""24px"" height=""24px"">");
		}
		else if(S_ISSOCK(sbuf.st_mode))
		{
			sprintf(img,"<img src=""sock.png"" width=""24px"" height=""24px"">");
		}
		else
			sprintf(img,"<img src=""file.png"" width=""24px"" height=""24px"">");


		sprintf(files,"%s<p><pre>%-2d %s ""<a href=%s%s"">%-15s</a>%-10s%10d %24s</pre></p>\r\n",files,num++,img,dir,dirp->d_name,dirp->d_name,filepasswd->pw_name,(int)sbuf.st_size,timeModify(sbuf.st_mtime,modifyTime));
	}
	closedir(dp);
	sprintf(files,"%s</body></html>",files);

	/* Send response headers to client */
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
	sprintf(buf, "%sContent-length: %d\r\n", buf, strlen(files));
	sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, "text/html");

	#ifdef HTTPS
	if(ishttps)
	{
		SSL_write(ssl,buf,strlen(buf));
		SSL_write(ssl,files,strlen(files));
	}
	else
	#endif
	{
		Rio_writen(fd, buf, strlen(buf));
		Rio_writen(fd, files, strlen(files));
	}
	exit(0);

}
/* $end serve_dir */


/* $begin post_dynamic */
static void post_dynamic(int fd, char *filename, int contentLength,rio_t *rp)
{
    char buf[MAXLINE],length[32], *emptylist[] = { NULL },data[MAXLINE];
    int p[2];

    #ifdef HTTPS 
    int httpsp[2];
    #endif

    sprintf(length,"%d",contentLength);
    memset(data,0,MAXLINE);

    Pipe(p);

    /*       The post data is sended by client,we need to redirct the data to cgi stdin.
    *  	 so, child read contentLength bytes data from fp,and write to p[1];
    *    parent should redirct p[0] to stdin. As a result, the cgi script can
    *    read the post data from the stdin. 
    */

    /* https already read all data ,include post data  by SSL_read() */
   
    	if (Fork() == 0)
	{                     /* child  */ 
		Close(p[0]);
		#ifdef HTTPS 
		if(ishttps)
		{
			Write(p[1],httpspostdata,contentLength);	
		}
		else
		#endif
		{
			Rio_readnb(rp,data,contentLength);
			Rio_writen(p[1],data,contentLength);
		}
		exit(0)	;
	}
    
    /* Send response headers to client */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n",buf);

    #ifdef HTTPS 
    if(ishttps)
    	SSL_write(ssl,buf,strlen(buf));
    else
    #endif
        Rio_writen(fd, buf, strlen(buf));

    //Wait(NULL);
    Dup2(p[0],STDIN_FILENO);  /* Redirct p[0] to stdin */
    Close(p[0]);

    Close(p[1]);
    setenv("CONTENT-LENGTH",length , 1); 

    #ifdef HTTPS 
    if(ishttps)  /* if ishttps,we couldnot redirct stdout to client,we must use SSL_write */
    {
    	Pipe(httpsp);

	if(Fork()==0)
	{
    	Dup2(httpsp[1],STDOUT_FILENO);        /* Redirct stdout to https[1] */ 
		Execve(filename, emptylist, environ); 
	}
	//Wait(NULL);
	Read(httpsp[0],data,MAXLINE);
	SSL_write(ssl,data,strlen(data));
    }
    else
    #endif
    {
    	Dup2(fd,STDOUT_FILENO);        /* Redirct stdout to client */ 
	    Execve(filename, emptylist, environ); 
    }
}
/* $end post_dynamic */





/*
 * parse_uri - parse URI into filename and CGI args
 *             return 0 if dynamic content, 1 if static
 */
/* $begin parse_uri */
static int parse_uri(char *uri, char *filename, char *cgiargs) 
{
    char *ptr;
    char tmpcwd[MAXLINE];
    strcpy(tmpcwd,cwd);
    strcat(tmpcwd,"/");

    if (!strstr(uri, "cgi-bin")) 
    {  /* Static content */
    	strcpy(cgiargs, "");
    	strcpy(filename, strcat(tmpcwd,Getconfig("root")));
    	strcat(filename, uri);
    	if (uri[strlen(uri)-1] == '/')
    	    strcat(filename, "home.html");
    	return 1;
    }
    else 
    {  
        /* Dynamic content */
    	ptr = index(uri, '?');
    	if (ptr) 
    	{
    	    strcpy(cgiargs, ptr+1);
    	    *ptr = '\0';
    	}
    	else 
    	    strcpy(cgiargs, "");
    	strcpy(filename, cwd);
    	strcat(filename, uri);
    	return 0;
    }
}
/* $end parse_uri */

/*
 * serve_static - copy a file back to the client 
 */
/* $begin serve_static */
static void serve_static(int fd, char *filename, int filesize) 
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];
 
    /* Send response headers to client */
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);

    /* Send response body to client */
    srcfd = Open(filename, O_RDONLY, 0);
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);

    #ifdef HTTPS 
    if(ishttps)
    {
    	SSL_write(ssl, buf, strlen(buf));
	    SSL_write(ssl, srcp, filesize);
    }
    else
    #endif
    {
    	Rio_writen(fd, buf, strlen(buf));
    	Rio_writen(fd, srcp, filesize);
    }
    Munmap(srcp, filesize);
}

/*
 * get_filetype - derive file type from file name
 */
static void get_filetype(const char *filename, char *filetype) 
{
    if (strstr(filename, ".html"))
	strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
	strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
	strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".png"))
	strcpy(filetype, "image/png");
    else
	strcpy(filetype, "text/plain");
}  
/* $end serve_static */

/*
 * serve_dynamic - run a CGI program on behalf of the client
 */
/* $begin get_dynamic */
void get_dynamic(int fd, char *filename, char *cgiargs) 
{
    char buf[MAXLINE], *emptylist[] = { NULL },httpsbuf[MAXLINE];
    int p[2];

    /* Return first part of HTTP response */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n",buf);

    #ifdef HTTPS 
    if(ishttps)
    	SSL_write(ssl,buf,strlen(buf));
    else
    #endif
       	Rio_writen(fd, buf, strlen(buf));
	
    #ifdef HTTPS 
    if(ishttps)
    {
    	Pipe(p);
        if (Fork() == 0)
    	{  /* child  */ 
    		Close(p[0]);
            //cgi get模式的参数传递
    		setenv("QUERY_STRING", cgiargs, 1); 
            //CGI程序标准输出父进程的管道上
    		Dup2(p[1], STDOUT_FILENO);         /* Redirect stdout to p[1] */
    		Execve(filename, emptylist, environ); /* Run CGI program */	
    	}
    	Close(p[1]);
    	Read(p[0],httpsbuf,MAXLINE);   /* parent read from p[0] */
	    SSL_write(ssl,httpsbuf,strlen(httpsbuf));
    }
    else
    #endif
    {
    	if (Fork() == 0) 
    	{ /* child */
    		/* Real server would set all CGI vars here */
    		setenv("QUERY_STRING", cgiargs, 1); 
    		Dup2(fd, STDOUT_FILENO);         /* Redirect stdout to client */
    		Execve(filename, emptylist, environ); /* Run CGI program */
    	}
    }
    //Wait(NULL); /* Parent waits for and reaps child */
}
/* $end get_dynamic */

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror ip管控错误响应*/
static void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body 封装成html页面*/
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response 封装成http返回头*/
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    sprintf(buf, "%sContent-type: text/html\r\n",buf);
    sprintf(buf, "%sContent-length: %d\r\n\r\n",buf,(int)strlen(body));

    #ifdef HTTPS 
    if(ishttps)
    {
    	SSL_write(ssl,buf,strlen(buf));//如果是ssl，使用加密算法后发送
    	SSL_write(ssl,body,strlen(body));
    }
    else
    #endif
    {
    	Rio_writen(fd, buf, strlen(buf));//普通发送
	    Rio_writen(fd, body, strlen(body));
    }
}
/* $end clienterror */



/* $begin writePid  */ 
/* if the process is running, the interger in the pid file is the pid, else is -1  */
static void writePid(int option)
{
	int pid;
	FILE *fp=Fopen(PID_FILE,"w+");
	if(option)
		pid=(int)getpid();
	else
		pid=-1;
	fprintf(fp,"%d",pid);
	Fclose(fp);
}
/* $end writePid  */ 
