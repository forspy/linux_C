#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<aio.h>


#define BUFFER_SIZE 1024

int MAX_LIST = 2;

int main(int argc,char **argv)
{
    //aio��������ṹ��
    struct aiocb rd;

    int fd,ret,couter;

    //cblist����
    struct aiocb *aiocb_list[2];



    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }



    //��rd�ṹ�����
    bzero(&rd,sizeof(rd));


    //Ϊrd.aio_buf����ռ�
    rd.aio_buf = malloc(BUFFER_SIZE + 1);

    //���rd�ṹ��
    rd.aio_fildes = fd;
    rd.aio_nbytes =  BUFFER_SIZE;
    rd.aio_offset = 0;

    //����fd���¼�ע��
    aiocb_list[0] = &rd;

    //�����첽������
    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }

    printf("��Ҫ��ʼ�ȴ��첽���¼����\n");
    //�����ȴ��첽���¼����
    ret = aio_suspend(aiocb_list,MAX_LIST,NULL);
	
	
    //��ȡ�첽������ֵ
    ret = aio_return(&rd);

    printf("\n\n����ֵΪ:%d\n",ret);


    return 0;
}
