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

#define BUFFER_SIZE 1025

int MAX_LIST = 2;


int main(int argc,char **argv)
{
    struct aiocb *listio[2];
    struct aiocb rd,wr;
    int fd,ret;

    //�첽���¼�
    fd = open("test1.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test1.txt");
    }

    bzero(&rd,sizeof(rd));

    rd.aio_buf = (char *)malloc(BUFFER_SIZE);
    if(rd.aio_buf == NULL)
    {
        perror("aio_buf");
    }

    rd.aio_fildes = fd;
    rd.aio_nbytes = 1024;
    rd.aio_offset = 0;
    rd.aio_lio_opcode = LIO_READ;   ///lio��������Ϊ�첽��

    //���첽���¼���ӵ�list��
    listio[0] = &rd;


    //�첽Щ�¼�
    fd = open("test2.txt",O_WRONLY | O_APPEND);
    if(fd < 0)
    {
        perror("test2.txt");
    }

    bzero(&wr,sizeof(wr));

    wr.aio_buf = (char *)malloc(BUFFER_SIZE);
    if(wr.aio_buf == NULL)
    {
        perror("aio_buf");
    }

    wr.aio_fildes = fd;
    wr.aio_nbytes = 1024;

    wr.aio_lio_opcode = LIO_WRITE;   ///lio��������Ϊ�첽д

    //���첽д�¼���ӵ�list��
    listio[1] = &wr;

    //ʹ��lio_listio����һϵ������
    ret = lio_listio(LIO_WAIT,listio,MAX_LIST,NULL);

    //���첽��д�����ʱ��ȡ���ǵķ���ֵ

    ret = aio_return(&rd);
    printf("\n������ֵ:%d",ret);

    ret = aio_return(&wr);
    printf("\nд����ֵ:%d",ret);



    return 0;
}
