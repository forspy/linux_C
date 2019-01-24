#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<pthread.h>

static void usage(const char *proc)
{
    printf("Please use :%s [IP] [port]\n",proc);
}

void thread_run(void *arg)
{
    printf("creat a new thread\n");
    int fd = (int)arg;
    char buf[1024];

    while(1){
        memset(buf,'\0',sizeof(buf));
        ssize_t _s = read(fd,buf,sizeof(buf) - 1);//等待客户端的write写入发送
        if(_s > 0){
            buf[_s] = '\0';
            printf("client say : %s\n",buf);
        }    
        memset(buf,'\0',sizeof(buf));
        printf("please Enter: ");
        fflush(stdout);
        ssize_t _s2 = read(0,buf,sizeof(buf) - 1);
        if(_s2 > 0){
            write(fd,buf,strlen(buf));
        }
    }
}

int main(int argc,char *argv[])
{
    if(argc != 3){
        usage(argv[0]);
        exit(1);
    }

    //1.creat socket 创建socket

    int sock = socket(AF_INET,/*IPV4*/SOCK_STREAM,/*流式套接字*/0);
    if(sock < 0){
        perror("creat socket error\n");
        return 1;
    }
	//绑定
    struct sockaddr_in local;  //sockaddr_in有很多字段，能够辅助你的设置 到时候将sockaddr_in* 强转为sockaddr*即可
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));//htons把本地字节序转换成网络字节序(字节序存在大端/小端之分，本地和网络不一样)atoi因为端口是通过参数argv[]传进来的，需要转化成int
    local.sin_addr.s_addr = inet_addr(argv[1]);//inet_addr将点分十进制转化为网络用的int地址192.168.1.1-->19216811

    //2.bind

    if(bind(sock,(struct sockaddr*)&local,/*生成需要绑定地IP地址*/sizeof(local)) < 0){
        perror("bind error\n");
        close(sock);
        return 2;
    }
   
    //3.listen开始监听

    if(listen(sock,10) < 0){
        perror("listen error\n");
        close(sock);
        return 3;
    }

    printf("bind and listen success!wait accept...\n");
    
    //4.accept使用accept接受客户端的数据

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    while(1){

        int fd = accept(sock,(struct sockaddr*)&peer /*获取对端地址*/,&len);//阻塞函数，fd为链接句柄

        if(fd < 0){
            perror("accept error\n");
            close(sock);
            return 4;
        }
        
        printf("get connect,ip is : %s port is : %d\n",inet_ntoa(peer.sin_addr),/*将19216811转化为192.168.1.1*/ntohs(peer.sin_port)/*将网络字节序转化为本地字节序*/);
		//服务器的处理状态---开启一些线程
        pthread_t id;
        pthread_create(&id,NULL,thread_run,(void*)fd/*由新开的线程处理accept的句柄fd(链接句柄)*/);//然后主线程继续返回79行while(1)在accpet函数里面做等待，
		//而当前链接的客户端由pthread_create的线程与它做通信

        pthread_detach(id);
		//---------
    }
    close(sock);
    return 0;
}
