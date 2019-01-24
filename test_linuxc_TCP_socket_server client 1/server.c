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
        ssize_t _s = read(fd,buf,sizeof(buf) - 1);//�ȴ��ͻ��˵�writeд�뷢��
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

    //1.creat socket ����socket

    int sock = socket(AF_INET,/*IPV4*/SOCK_STREAM,/*��ʽ�׽���*/0);
    if(sock < 0){
        perror("creat socket error\n");
        return 1;
    }
	//��
    struct sockaddr_in local;  //sockaddr_in�кܶ��ֶΣ��ܹ������������ ��ʱ��sockaddr_in* ǿתΪsockaddr*����
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[2]));//htons�ѱ����ֽ���ת���������ֽ���(�ֽ�����ڴ��/С��֮�֣����غ����粻һ��)atoi��Ϊ�˿���ͨ������argv[]�������ģ���Ҫת����int
    local.sin_addr.s_addr = inet_addr(argv[1]);//inet_addr�����ʮ����ת��Ϊ�����õ�int��ַ192.168.1.1-->19216811

    //2.bind

    if(bind(sock,(struct sockaddr*)&local,/*������Ҫ�󶨵�IP��ַ*/sizeof(local)) < 0){
        perror("bind error\n");
        close(sock);
        return 2;
    }
   
    //3.listen��ʼ����

    if(listen(sock,10) < 0){
        perror("listen error\n");
        close(sock);
        return 3;
    }

    printf("bind and listen success!wait accept...\n");
    
    //4.acceptʹ��accept���ܿͻ��˵�����

    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    while(1){

        int fd = accept(sock,(struct sockaddr*)&peer /*��ȡ�Զ˵�ַ*/,&len);//����������fdΪ���Ӿ��

        if(fd < 0){
            perror("accept error\n");
            close(sock);
            return 4;
        }
        
        printf("get connect,ip is : %s port is : %d\n",inet_ntoa(peer.sin_addr),/*��19216811ת��Ϊ192.168.1.1*/ntohs(peer.sin_port)/*�������ֽ���ת��Ϊ�����ֽ���*/);
		//�������Ĵ���״̬---����һЩ�߳�
        pthread_t id;
        pthread_create(&id,NULL,thread_run,(void*)fd/*���¿����̴߳���accept�ľ��fd(���Ӿ��)*/);//Ȼ�����̼߳�������79��while(1)��accpet�����������ȴ���
		//����ǰ���ӵĿͻ�����pthread_create���߳�������ͨ��

        pthread_detach(id);
		//---------
    }
    close(sock);
    return 0;
}
