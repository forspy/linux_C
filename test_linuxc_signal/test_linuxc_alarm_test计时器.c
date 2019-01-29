#include<signal.h>
#include<stdio.h>
int main(void)
{
	printf("first time return:%d\n",alarm(4));//第一次设置返回0（成功了），失败返回-1
	sleep(1);	
	printf("after sleep(1),remain:%d\n",alarm(2));//第二次返回第一次位置计时开始后剩余的时间，即3秒
	printf("renew alarm,remain:%d\n",alarm(1));//第三次返回第二次设置后剩余的时间，即2秒
}
