#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
void myHandler(int sig);
int main(int argc,char *argv[])
{ 
  	struct sigaction act, oact;

  	act.sa_handler = myHandler;//设置一个回调函数
  	sigemptyset(&act.sa_mask); /*initial. to empty mask*/ //去清空当前信号的集合
  	act.sa_flags = 0;
  	sigaction(SIGINT, &act, &oact); //安装SIGUSR1信号与act绑定，并且备份到oact里面去，当以后收到SIGUSR1信号就会执行myHandler回调函数
	//ctrl-c可以返回一个SIGINT信号
    while (1) 
	{ 
		printf("Hello world.\n"); 
		pause();//收到信号后pause会解除一次
	}//要结束程序运行可以ctrl-z 或者kill -SIGKILL 进程号 ，进程号由ps指令获得
  }

  void myHandler(int sig)
  {
    printf("I got signal: %d.\n", sig);
  }
  // to end program, <Ctrl + \> to generate SIGQUIT



