#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
struct msgbuf{//一个消息
	int type;
	char ptr[0];//为什么长度为0？待解决  ptr[0]这样就可以动态改变内存大小发送消息 windows里面不能分配大小为0的数组 linux里面不知道这个是什么写法，我觉得不好
};
int main(int argc,char *argv[]){
	key_t key;
	key=ftok(argv[1],100);//创建一个key，路径通过参数传进来
	int msgid;
	msgid=msgget(key,IPC_CREAT|0600);
	pid_t pid;
	pid=fork();
	if(pid==0){//子进程
		while(1){
			printf("pls input msg to send:");
			char buf[128];
			fgets(buf,128,stdin);//stdin为标准输入
			struct msgbuf *ptr=malloc(sizeof(struct msgbuf)+strlen(buf)+1);
			ptr->type=1;//设置发送消息类型为1
			memcpy(ptr->ptr,buf,strlen(buf)+1);
			msgsnd(msgid,ptr,strlen(buf)+1,0);//发送消息
			free(ptr);
		}
	}
	else{//父进程
	//这里重新构造msgbuf对象是为了开辟内存空间进行读取
		struct msgbuf{
			int type;
			char ptr[1024];
		};
		while(1){
			struct msgbuf mybuf;
			memset(&mybuf,'\0',sizeof(mybuf));
			msgrcv(msgid,&mybuf,32,2,0);	//接受type为2的值，因为父子type不一样，所以父子之间的进程不能通信 
			printf("recv msg:%s\n",mybuf.ptr);
		}
	}
}
