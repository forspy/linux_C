#include<stdio.h>
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>

int main()
{
	
    int pipe_fd[2] = {-1,-1};//建立管道两端，父子进程共享，是一个特殊的 文件
	
	
    if(pipe(pipe_fd) < 0){//利用pipe()创建管道进行进程间的通信
        printf("pipe error, %s\n",strerror(errno));
        return 1;
    }

    pid_t id = fork();//创建一个进程
	
    if(id == 0){
        //child write
        close(pipe_fd[0]);//关闭了文件就关闭了管道的一端，所以子进程使用pipe_fd[1]

        char *msg = "child write:enenshiwo\n";
        while(1){
            write(pipe_fd[1],msg,strlen(msg));//往pipe_fd[1]写入
            sleep(1);
        }
		
		
    }else{
        //father read
        close(pipe_fd[1]);//父进程使用pipe_fd[0]

        char buf[1024];
        while(1){
            buf[0] = '\0';
			 char *msg_f = "father write:enenshiwo\n";
            ssize_t _sz = read(pipe_fd[0],buf,sizeof(buf) - 1);//从pipe_fd[0]里面读取，因为管道是相通的
            if(_sz > 0){
                buf[_sz] = '\0';
            }//read会返回读的位置，这样在最后面添'\0'

            printf("father read : %s\n",buf);
        }
        wait(NULL);//等待子进程结束
    }
    return 0;
	//在两个进程都退出后会pipe管道会自动释放
	//pipe管道本质是内核的内存缓冲区，因此不支持大量数据传输
	//用途：比如客户端突然断开，就会导致管道的一段被关闭（重新连接会发生错误），这时候进程会收到一个SIGPIPE的信号，可以根据这个信号继续接上通信
	//pipe的通信是单向的，如果需要双向通信需要两条pipe，只支持亲缘进程
}
