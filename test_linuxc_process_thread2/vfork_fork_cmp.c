#include<unistd.h>
#include<error.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
int glob=6;
int main()
{
	int var;
	pid_t pid;
	var=88;//var对于子进程来说是全局变量，共享
	printf("in beginning:\tglob=%d\tvar=%d\n",glob,var);
	if((pid=vfork())<0)
	{
		perror("vfork");
		exit(EXIT_FAILURE);
	}
	else if(pid==0)
	{
		//int a=1;//a对于父进程来说不共享，只属于子进程
		printf("in child,modify the var:glob++,var++\n");
		glob++;
		var++;
		printf("in child:\tglob=%d\tvar=%d\n",glob,var);
		_exit(0);
	}
	else
	{	
		wait(NULL);//阻塞等待直到子进程退出，一般来说当前的程序为父进程所以应该是父进程先printf，但是由于wait的阻塞导致子进程先printf
		printf("in parent:\tglob=%d\tvar=%d\n",glob,var);
		return 0;
	}
}
/*
结果：
一开始：glob=6 var=88
++后，子进程：glob=7 var=89
父进程：glob=7 var=89
*/

/*
如果是fork创建子进程的话
一开始：glob=6 var=88
++后，子进程：glob=7 var=89
父进程：glob=6 var=88
*/