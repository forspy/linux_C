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
    struct aiocb rd;//����һ��read�Ķ���

    int fd,ret,couter;

    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }



    //��rd�ṹ�����
    bzero(&rd,sizeof(rd));


    //Ϊrd.aio_buf����ռ�
    rd.aio_buf = malloc(BUFFER_SIZE + 1);//����1024���ַ�+'\0'
��
    //���rd�ṹ��
    rd.aio_fildes = fd;
    rd.aio_nbytes = BUFFER_SIZE;//�������ֽ�
    rd.aio_offset = 0;//���ļ���ͷƫ�ƶ����ֽڿ�ʼ��

    //�����첽������
    ret = aio_read(&rd);//���Ϸ��أ���������������10G���ļ������������̻߳��Ƿ�֧�߳�ʹ��read()��ȡ������������ȴ�
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }
	//do other things
	
    couter = 0;
//  ѭ���ȴ��첽����������
    while(aio_error(&rd) == EINPROGRESS)//��ѯ�ں˶�ȡ�Ƿ����
		//���aio_error(&rd)�Ľ��ΪEINPROGRESS��ô�ͱ�ʾ����û���ں�û�ж�ȡ�꣬��߿����첽������������
    {
       // printf("��%d��\n",++couter);
	   //...�������첽�����ĳ���
    }
	
    //��ȡ�첽������ֵ
    ret = aio_return(&rd);
	
    printf("\n\n����ֵΪ:%d\n",ret);
	printf("%s\n",rd.aio_buf);//rd.aio_buf������ڴ������д������
	
	free(rd.aio_buf);
	close(fd);
    return 0;
}
//gcc -g -o ...-lrt(���ļ�)