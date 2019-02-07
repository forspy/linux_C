#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
int main(int argc,char *argv[])
{
	int fd,status;
	pid_t pid;
	fd=open("test.txt",O_RDWR|O_APPEND|O_CREAT,0644);//打开方式可读可写可追加
	if(fd==-1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	
	//fcntl(fd,F_SETFD,FD_CLOEXEC); //include this ,will error
	printf("befor child process write\n");
	system("cat test.txt");
	
	if((pid=fork())==-1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(pid==0)
	{	
		char buf[128];
		sprintf(buf,"%d",fd);
		execl("./newcode","newcode",buf,(char *)0);//子进程中执行其他程序argv[0]为执行程序名传入，最后一个参数可以为null
	}
	else
	{
		wait(&status);
		printf("after child_process write\n");
		system("cat test.txt");//system() 执行shell命令
	}
}
