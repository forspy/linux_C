#include<unistd.h>
#include<stdio.h>
#include<string.h>
int main(int argc,char *argv[])
{
	int i;
	int fd;
	char *ptr="helloworld\n";
	fd=atoi(argv[1]);//这里的fd是共享的就是fcntl_example 里面传入的fd参数，因为是字符串，所以改为int
	i=write(fd,ptr,strlen(ptr));//写入文件
	if(i<=0)
		perror("write");
	close(fd);	
}
