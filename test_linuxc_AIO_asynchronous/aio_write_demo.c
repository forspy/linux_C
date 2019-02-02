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

int main(int argc,char **argv)
{
    //����aio���ƿ�ṹ��
    struct aiocb wr;

    int ret,fd;

    char str[20] = {"hello,world"};

    //����wr�ṹ��
    bzero(&wr,sizeof(wr));

    fd = open("test.txt",O_WRONLY | O_APPEND);
    if(fd < 0)
    {
        perror("test.txt");
    }


    wr.aio_buf = str;

    //���aiocb�ṹ
    wr.aio_fildes = fd;
    wr.aio_nbytes = sizeof(str);

    //�첽д����
    ret = aio_write(&wr);
    if(ret < 0)
    {
        perror("aio_write");
    }

    //�ȴ��첽д���
    while(aio_error(&wr) == EINPROGRESS)
    {
        printf("hello,world\n");
    }

    //����첽д�ķ���ֵ
    ret = aio_return(&wr);//retΪ��ʵд��ȥ�ĳ���
    printf("\n\n\n����ֵΪ:%d\n",ret);

    return 0;
}
