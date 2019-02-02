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
    //aio操作所需结构体
    struct aiocb rd;//定义一个read的对象

    int fd,ret,couter;

    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }



    //将rd结构体清空
    bzero(&rd,sizeof(rd));


    //为rd.aio_buf分配空间
    rd.aio_buf = malloc(BUFFER_SIZE + 1);//输入1024个字符+'\0'
‘
    //填充rd结构体
    rd.aio_fildes = fd;
    rd.aio_nbytes = BUFFER_SIZE;//读多少字节
    rd.aio_offset = 0;//从文件开头偏移多少字节开始读

    //进行异步读操作
    ret = aio_read(&rd);//马上返回，非阻塞，比如有10G的文件，不管是主线程还是分支线程使用read()读取都会进行阻塞等待
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }
	//do other things
	
    couter = 0;
//  循环等待异步读操作结束
    while(aio_error(&rd) == EINPROGRESS)//查询内核读取是否完成
		//如果aio_error(&rd)的结果为EINPROGRESS那么就表示程序没有内核没有读取完，这边可以异步操作其他进程
    {
       // printf("第%d次\n",++couter);
	   //...这里是异步操作的程序
    }
	
    //获取异步读返回值
    ret = aio_return(&rd);
	
    printf("\n\n返回值为:%d\n",ret);
	printf("%s\n",rd.aio_buf);//rd.aio_buf存的是内存里面读写的内容
	
	free(rd.aio_buf);
	close(fd);
    return 0;
}
//gcc -g -o ...-lrt(库文件)