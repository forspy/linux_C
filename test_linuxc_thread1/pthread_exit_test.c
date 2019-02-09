#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
void *helloworld(char *argc);
int main(int argc,int argv[])
{
	int error;
	int *temptr;
	
	pthread_t thread_id;
	
	pthread_create(&thread_id,NULL,(void *)*helloworld,"helloworld"/*表示传入参数*/);
	printf("*p=%x,p=%x\n",*helloworld,helloworld);//*helloworld表示函数名也是地址 helloworld也是地址，两个是一样的
	if(error=pthread_join(thread_id,(void **)&temptr))//主线程阻塞在这儿，等待子线程的结束，(void **)&temptr接收线程结束的返回值，即p的地址
	{
		perror("pthread_join");
		exit(EXIT_FAILURE);	
	}
	printf("temp=%x,*temp=%c\n",temptr,*temptr);
	*temptr='d';
	printf("%c\n",*temptr);
	free(temptr);
	return 0;
}

void *helloworld(char *argc)
{
	int *p;
	p=(int *)malloc(10*sizeof(int));
	printf("the message is %s\n",argc);
	printf("the child id is %u\n",pthread_self());
	memset(p,'c',10);
	printf("p=%x\n",p);
	pthread_exit(p);//线程退出
	//return 0;
}
