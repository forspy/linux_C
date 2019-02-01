#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
 
void sig_handler(int num)
{
    printf("receive the signal %d.\n", num);
    alarm(2);//如果我们想程序每2秒都定时一下，这样实现也很简单，我们在处理定时信号的函数中再次定时2秒；
}
 
int main()
{
    signal(SIGALRM, sig_handler);
    alarm(2);
    while(1){
        pause();//pause和sleep一样收到信号就会返回
        printf("pause is over.\n");
    }
    exit(0);
}
/*
alarm(time);执行之后告诉内核，让内核在time秒时间之后向该进程发送一个定时信号，然后该进程捕获该信号并处理；
pause()函数使该进程暂停让出CPU,但是该函数的暂停和前面的那个sleep函数的睡眠都是可被中断的睡眠，也就是说收到了中断信号之后再
重新执行该进程的时候就直接执行pause()和sleep()函数之后的语句；
*/