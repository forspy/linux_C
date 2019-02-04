#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	pid_t pid;
	printf("before fork,have enter\n");
	printf("before fork,no enter:pid=%d\t",getpid());
	fflush(0);
	pid=fork();
	if(pid==0)
	{
		printf("\nchild,after fork:pid=%d\n",getpid());		
	}
	else
		printf("\nparent,after fork:pid=%d\n",getpid());
}
/*
before fork,have enter
before fork,no enter:pid=4310
parent,after fork:pid=4310
forspy@forspy1:~/linuxc/test_linuxc_process_thread2$ //这里开始创建子进程，并没有打印缓存区的内容
child,after fork:pid=4311
*/