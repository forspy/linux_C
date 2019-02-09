#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h> 
struct message
{
	int i;
	int j;
};


void hello(struct message *str)
{
	printf("child,the tid=%lu,pid=%ld\n",pthread_self(),syscall(SYS_gettid));
	printf("the arg.i is %d,arg.j is %d\n",str->i,str->j);
	while(1);//这边回调函数一直运行
}

int main(int argc,char *agrv[])
{
	struct message test;
	pthread_t thread_id;
	test.i=10;
	test.j=20;
	pthread_create(&thread_id,NULL,(void *)hello,&test/*表示传入参数*/);
	printf("parent,the tid=%lu,pid=%ld\n",pthread_self(),syscall(SYS_gettid));
	pthread_join(thread_id,NULL);
}
/*
parent,the tid=140138128164672,pid=2297
child,the tid=140138119649024,pid=2298  //pid=2298其实是个虚拟进程与pid=2297共享资源
the arg.i is 10,arg.j is 20
*/
//cd /proc/  里面可以查看进程状态  进程文件夹
//cd task 查看进程组 可以看到2297 2298
//pmap -x 2297 >a.out  可以保存该进程的信息
//pmap -x 2298 >b.out
//diff a.out b.out  比对两个文件的内容 发现内存空间是一样的