#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
	pid_t pid;
	if((pid=fork())==-1)//注意，这里一定要加()和因为linux和window的连=判定方式不一样
		perror("fork failed!");
	if(pid==0)
	{
		printf("pid=%d,ppid=%d\n",getpid(),getppid());
		sleep(2);
		printf("pid=%d,ppid=%d\n",getpid(),getppid());//由于这里父进程已经退出了，所以将有系统进程接管
	}
	else
	{
		printf("father process!\n");
		sleep(1);
	}
	exit(0);
}
/*
father process!
pid=3575,ppid=3574
forspy@forspy1:~/linuxc/test_linuxc_process_thread3$ pid=3575,ppid=1
*/