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
#include<unistd.h>

#define BUFFER_SIZE 1025


void aio_completion_handler(sigval_t sigval)
{
    //������ȡ��aiocb�ṹ��ָ��
    struct aiocb *prd;
    int ret;

    prd = (struct aiocb *)sigval.sival_ptr;

    printf("hello\n");

    //�ж������Ƿ�ɹ�
    if(aio_error(prd) == 0)
    {
        //��ȡ����ֵ
        ret = aio_return(prd);
        printf("������ֵΪ:%d\n",ret);
    }
}

int main(int argc,char **argv)
{
    int fd,ret;
    struct aiocb rd;

    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }



    //���aiocb�Ļ�������
    bzero(&rd,sizeof(rd));

    rd.aio_fildes = fd;
    rd.aio_buf = (char *)malloc(sizeof(BUFFER_SIZE + 1));
    rd.aio_nbytes = BUFFER_SIZE;
    rd.aio_offset = 0;

    //���aiocb���йػص�֪ͨ�Ľṹ��sigevent
    rd.aio_sigevent.sigev_notify = SIGEV_THREAD;//ʹ���̻߳ص�֪ͨ
    rd.aio_sigevent.sigev_notify_function = aio_completion_handler;//���ûص�����
    rd.aio_sigevent.sigev_notify_attributes = NULL;//ʹ��Ĭ������
    rd.aio_sigevent.sigev_value.sival_ptr = &rd;//��aiocb���ƿ��м����Լ�������

    //�첽��ȡ�ļ�
    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
    }

    printf("�첽���Կ�ʼ\n");
    sleep(1);
    printf("�첽������\n");



    return 0;
}
