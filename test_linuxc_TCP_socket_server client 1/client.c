#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>

static void usage(const char *proc)
{
    printf("please use : %s [ip] [port]\n",proc);
}
int main(int argc,char *argv[])
{
    if( argc != 3 ){
        usage(argv[0]);
        exit(1);
    }
    
    int sock = socket(AF_INET,SOCK_STREAM,0);
    if(sock < 0){
        perror("socket error");
        return 1;
    }

    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_port = htons(atoi(argv[2]));
    remote.sin_addr.s_addr = inet_addr(argv[1]);


    int ret = connect(sock,(struct sockaddr*)&remote,sizeof(remote));
    if(ret < 0){
        printf("connect failed:%s\n",strerror(errno));
        return 2;
    }
    
    printf("connect success!\n");

    char buf[1024];
    while(1){
        memset(buf,'\0',sizeof(buf));
        printf("please enter:");
        fflush(stdout);
        ssize_t _s = read(0/*表示从标准输入里面读取数据*/,buf,sizeof(buf)-1);//read
        if(_s > 0){
            buf[_s - 1] = '\0';
            write(sock,buf,strlen(buf));//write，写入socket里面
            _s = read(sock,buf,sizeof(buf)-1);//read也是个阻塞函数，接受服务器的数据，如果收到会立刻返回
            if(_s > 0){
                if(strncasecmp(buf,"quit",4) == 0){
                     printf("qiut\n");
                     break;
                 }
                buf[_s -1] = '\0';
                printf("%s\n",buf);
            }
        }
    }
    close(sock);
    return 0;
}
