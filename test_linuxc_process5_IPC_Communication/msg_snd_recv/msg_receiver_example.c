#include<stdio.h>
#include<stdlib.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<string.h>
struct msgbuf{
	int type;
	char ptr[0];
};
int main(int argc,char *argv[])
{
	key_t key;
	key=ftok(argv[1],100);

	int msgid;
		msgid=msgget(key,IPC_CREAT|0666);

	pid_t pid;
	pid=fork();
	if(pid==0)	//子进程
	{
		
		while(1)
		{
			sleep(1);
			printf("pls input msg to send:");
			char buf[128];
			fgets(buf,128,stdin);
			struct msgbuf *ptr=malloc(sizeof(struct msgbuf)+strlen(buf)+1);
			ptr->type=2;	//send msg type=2
			memcpy(ptr->ptr,buf,strlen(buf)+1);
			msgsnd(msgid,ptr,strlen(buf)+1,0);//发送 msg type=2
			free(ptr);
		}
	}
	else
	{
		struct msgbuf{
			int type;
			char ptr[1024];
		};
		while(1)
		{
			struct msgbuf mybuf;
			memset(&mybuf,'\0',sizeof(mybuf));
			msgrcv(msgid,&mybuf,32,1,0);	//接收 msg type=1
			printf("recv msg:%s\n",mybuf.ptr);
		}
	}
	//这样就在sender和receiver之间根据发送消息的类型不同形成了全双工通信
}