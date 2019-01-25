#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

#define _BACKLOG_ 5 //监听队列里允许等待的最大值

int fds[20];//用来存放需要处理的IO事件

int creat_sock(char *ip,char *port)
{
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0){
        perror("creat_sock error");
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(port));
    local.sin_addr.s_addr = inet_addr(ip);
    
    if( bind(sock,(struct sockaddr*)&local,sizeof(local)) < 0){
        perror("bind");
        exit(2);
     }

    if(listen(sock,_BACKLOG_) < 0 ){
        perror("listen");
        exit(4);
     }

    return sock;
}

int main(int argc,char* argv[])
{
    if(argc != 3){
        printf("Please use : %s [ip] [port]\n",argv[0]);
        exit(3);
    }
    int listen_sock = creat_sock(argv[1],argv[2]);

    size_t fds_num = sizeof(fds)/sizeof(fds[0]);
    size_t i = 0;
    for(;i < fds_num;++i)
    {
        fds[i] = -1;//-1代表没有，而且accept没有收到返回的链接句柄为-1，存放的是链接句柄
    }
    
    int max_fd = listen_sock;
    fd_set rset;//rset文件集合里存放的是监听句柄
    
    while(1){
        FD_ZERO(&rset);
        FD_SET(listen_sock,&rset);//将监听句柄放入rset中
        struct timeval timeout = {10 , 0};//设置超时时间

        size_t i = 0;
        for(;i < fds_num;++i)
        {
            if(fds[i] > 0 ){
                FD_SET(fds[i] ,&rset);
                if(max_fd < fds[i])
				{
                    max_fd = fds[i];//max_fd为fds数组中的最大数，为啥要取最大数 待解决
                }
            }
        }

        switch(select(max_fd+1,&rset,NULL,NULL,&timeout))//max_fd+1表示听第几次
        {
            case -1:
                perror("select");//select出错
                break;
            case 0:
                printf("time out..\n");
            default://正数 
            {
                size_t i = 0;
                for(;i < fds_num;++i)
                {
                 //当为listen_socket事件就绪的时候，就表明有新的连接请求
                    if(FD_ISSET(fds[i],&rset) && fds[i] == listen_sock)//如果FD_ISSET(fds[i],&rset)为1代表有I/O事件，并且fds[i]等于监听句柄--》监听到事件
                    {//这里没有看到fds有放过listen_sock，为什么要判定呢？待解决
                        struct sockaddr_in client;
                        int accept_sock = accept(listen_sock,(struct sockaddr*)&client,sizeof(client));//accept接受这个监听句柄
                        if(accept_sock < 0){
                            perror("accept");
                            exit(5);
                        }

                        printf("connect by a client, ip:%s port:%d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port));

                        size_t i = 0;
                        for(;i < fds_num;++i)//将新接受的描述符存入集合中
                        {
                            if(fds[i] == -1){
                                fds[i] = accept_sock;//将链接句柄放置到fds里面，也就是说fds里面既有监听句柄又有链接句柄 待解决
                                break;
                            }
                        }
                        if(i == fds_num)//如果fds满了，先关一个socket，释放一个监听和接收句柄
                        {
                            close(accept_sock);
                        }
                    }
                    //普通请求
                    else if(FD_ISSET(fds[i],&rset) && (fds[i] > 0))
                    {
                        char buf[1024];
                        memset(buf,'\0',sizeof(buf));
                        ssize_t size = read(fds[i],buf,sizeof(buf)-1);
                        if(size < 0){
                            perror("read");
                            exit(6);
                        }else if(size == 0){
                            printf("client close..\n");
                            close(fds[i]);
                            fds[i] = -1;
                        }else{
                            printf("client say: %s\n",buf);
                        }
                        
                    }
                    else{}
                }
                
            }
            break;
        }
    }
    return 0;
}
