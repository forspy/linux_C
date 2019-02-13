#include "stdlib.h"
#include "stdio.h"
#include "RSA.h"
#include "epoll.h"
#include <execinfo.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mysql.h"
#include "cmd.h"
#include "upgrade.h"
#include "thread_pool.h"
#include <malloc.h>
#include <fcntl.h>


static void WidebrightSegvHandler(int signum) {
    void *array[10];
    int size;
    char **strings;
    int i, j;

    signal(signum, SIG_DFL);

    size = backtrace (array, 10);
    strings = (char **)backtrace_symbols (array, size);

    fprintf(stderr, "widebright received SIGSEGV! Stack trace:\n");
    for (i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n",i,strings[i]);
    }

    free (strings);
    exit(1);
}

int invalide_pointer_error(char * p)
{
    *p = 'd'; //让这里出现一个访问非法指针的错误
    return 0;
}


void error_2(char * p)
{
    invalide_pointer_error(p);
}

void error_1(char * p)
{
     error_2(p);
}

void error_0(char * p)
{
     error_1(p);
}

void printbuf(unsigned char* buf, int size)
{
    int i;
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"printbuf: %d",size);
    for(i=0;i<size;i++)
    {
        if(i % 20 == 0)
        {
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"\n");
        }
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"0x%02x ",buf[i]);
    }
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"\n");
}

void threadprintbuf(unsigned char* buf, int size,SQL_STRUCT* current_sql)
{
    int i;
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"thread printbuf: %d",size);
    for(i=0;i<size;i++)
    {
        if(i % 20 == 0)
        {
            THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"\n");
        }
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"0x%02x ",buf[i]);
    }
    THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_PKT_INFO,"\n");
}

void init_daemon(void)
{
    int pid;
    int i;
    int tmp;

    if(pid=fork())
        exit(0);//是父进程，结束父进程
    else if(pid< 0)
        exit(1);//fork失败，退出
    //是第一子进程，后台继续执行

    setsid();//第一子进程成为新的会话组长和进程组长
    //并和控制终端分离
    if(pid=fork())
        exit(0);//是第一子进程，结束第一子进程
    else if(pid< 0)
        exit(1);//fork失败，退出
    //是第二子进程，继续
    //第二子进程不再是会话组长
/*
    for(i=0;i< NOFILE; i)//关闭打开的文档描述符
        close(i);
*/
    tmp = chdir("/tmp");//改变工作目录到/tmp
    umask(0);//重设文档创建掩模
    return;
}

int need_deamon = 0;

static void getMemStatus()
{
    struct mallinfo info = mallinfo ();
    printf("arena = %d\n", info.arena);
    printf("ordblks = %d\n", info.ordblks);
    printf("smblks = %d\n", info.smblks);
    printf("hblks = %d\n", info.hblks);
    printf("hblkhd = %d\n", info.hblkhd);
    printf("usmblks = %d\n", info.usmblks);
    printf("fsmblks = %d\n", info.fsmblks);
    printf("uordblks = %d\n", info.uordblks);
    printf("fordblks = %d\n", info.fordblks);
    printf("keepcost = %d\n", info.keepcost);
}

static void dump_mall_info(int signum)
{
//    return ;
    time_t t = time(0);
    char tmp[128];
    int i;
    void* tmppoint;
    strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A 本年第%j天 %z",localtime(&t) );
    fprintf(stdout,"\n\n\n%s %s %d:\n",tmp,__FUNCTION__,__LINE__);
//    malloc_info(0, stdout);
    fprintf(stdout,"\n\n\n");
    getMemStatus();
//    i=10000;
//    tmppoint = MALLOC(i);
    memset(tmppoint,0,i);
    fprintf(stdout,"\n\n\n");
    getMemStatus();

    print_malloc_count(stderr);
    fprintf(stdout,"\n\n\n");
}


int main(int argc, char *argv[])
{
    pthread_t epoll_tid;
    pthread_t mysql_queue_tid;
    pthread_t cmd_listeh_thread_tid;
    pthread_t upgrade_info_tid;
    pthread_t memcheck_thread_tid;
//    pthread_t tid;

    my_malloc_debug_init();

    signal(SIGSEGV, WidebrightSegvHandler); // SIGSEGV      11       Core    Invalid memory reference
    signal(SIGABRT, WidebrightSegvHandler); // SIGABRT       6       Core    Abort signal from
//调试内存使用
//    signal(SIGRTMIN, dump_mall_info);       // SIGRTMIN     34

//    error_0(NULL);
    
    if (argc != 2)  
    {  
          fprintf (stderr, "Usage: %s [config file]\n", argv[0]);  
          exit (EXIT_FAILURE);  
    }
    load_config(argv[0],argv[1]);
    if(need_deamon)
    {
        init_daemon();
    }
    
    init_log();
    
    init_SQL();
    tpool_create(MAX_TPOOL_THREAD_NUM);
    
    srand( (unsigned int)time(0) );

//升级逻辑
//    if(pthread_create(&upgrade_info_tid, NULL, (void* (*)(void*))init_upgrade_info_thread, NULL))
//    {
//        fprintf (stderr, "upgrade_info_upgrade_failed\n");
//        exit(-1);
//    }
//    DEBUG_UPGRADE_DUMP();
//    exit(-1);
//mysql 命令队列
//    if(pthread_create(&mysql_queue_tid, NULL, (void* (*)(void*))mysql_queue, NULL))
//    {
//        fprintf (stderr, "epoll create error\n");  
//        exit(-1);
//    }
//    stop_SQL();
//    exit(-1);
//初始化RSA密码
//	init_RSA();

    if(pthread_create(&epoll_tid, NULL, (void* (*)(void*))epoll, NULL))
    {
        fprintf (stderr, "epoll create error\n");  
        exit(-1);
    }
    
//    if(pthread_create(&cmd_listeh_thread_tid, NULL, (void* (*)(void*))cmd_listen_thread, NULL))
//    {
//        fprintf (stderr, "cmd_thread create error\n");  
//        exit(-1);
//    }
    
    pthread_join (epoll_tid, NULL);
//    pthread_join (mysql_queue_tid, NULL);
//    pthread_join (cmd_listeh_thread_tid, NULL);
}
