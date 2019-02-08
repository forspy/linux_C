#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main()
{
	pid_t pid;
	if((pid=fork())==-1)
		perror("fork");
	else if(pid==0)
	{
		printf("child_pid pid=%d\n",getpid());
		exit(0);
	}
	sleep(3);
	system("ps");//执行shell ps命令
	exit(0);
}
/*
child_pid pid=2790
   PID TTY          TIME CMD
  2462 pts/1    00:00:00 bash
  2789 pts/1    00:00:00 dead_p//父进程
  2790 pts/1    00:00:00 dead_p <defunct>//僵死的子进程 等wait函数之后僵死状态才会解除
  2791 pts/1    00:00:00 sh
  2792 pts/1    00:00:00 ps
*/