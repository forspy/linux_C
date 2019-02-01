#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>

void func(int signo, siginfo_t *info, void *p)
{
	printf("signo=%d\n",signo);
	printf("sender pid=%d\n",info->si_pid);//打印发送这个指令的程序的pid ，一般为bash的pid 表示是bash进程发出的指令
}

int main(int argc,char *argv[])
{ 
  	struct sigaction act, oact;

  	sigemptyset(&act.sa_mask); /*initial. to empty mask*/
  	act.sa_flags = SA_SIGINFO;//设置这个才可以打印
	act.sa_sigaction=func;//绑定回调函数
  	sigaction(SIGUSR1, &act, &oact); 
    while (1) 
	{ 
		printf("pid is %d Hello world.\n",getpid()); 
		pause();
	}
}
//注意要使用kill -SIGUSR1 pid 需要使这个程序现在后台运行 方法： ./文件名 &
