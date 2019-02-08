#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc,char *argv[])
{
	pid_t pid;
	printf("before fork,have enter\n");
	printf("before fork,no enter:pid=%d\t",getpid());
	//fflush(0);//需要在printf完清理缓存，防止子进程再打一遍，fflush(0)就可以防止流缓存区泄漏
	pid=fork();//子进程会复制父进程的内容，就像是用叉子把牛排从哟个盘子里放到另一个盘子里
	if(pid==0)
	{
		printf("\nchild,after fork:pid=%d\n",getpid());		
	}
	else
		printf("\nparent,after fork:pid=%d\n",getpid());
}

/*
before fork,have enter
before fork,no enter:pid=4357
parent,after fork:pid=4357
forspy@forspy1:~/linuxc/test_linuxc_process_thread2$ before fork,no enter:pid=4357 //这里开始创建子进程，由于没有fflush()清理缓存，打印了上一条的缓存区内容
//缓存区默认存放最后一次的内容
child,after fork:pid=4358
*/