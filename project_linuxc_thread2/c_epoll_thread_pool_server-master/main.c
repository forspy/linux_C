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
    *p = 'd'; //���������һ�����ʷǷ�ָ��Ĵ���
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
        exit(0);//�Ǹ����̣�����������
    else if(pid< 0)
        exit(1);//forkʧ�ܣ��˳�
    //�ǵ�һ�ӽ��̣���̨����ִ��

    setsid();//��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤
    //���Ϳ����ն˷���
    if(pid=fork())
        exit(0);//�ǵ�һ�ӽ��̣�������һ�ӽ���
    else if(pid< 0)
        exit(1);//forkʧ�ܣ��˳�
    //�ǵڶ��ӽ��̣�����
    //�ڶ��ӽ��̲����ǻỰ�鳤
/*
    for(i=0;i< NOFILE; i)//�رմ򿪵��ĵ�������
        close(i);
*/
    tmp = chdir("/tmp");//�ı乤��Ŀ¼��/tmp
    umask(0);//�����ĵ�������ģ
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
    strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A �����%j�� %z",localtime(&t) );
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
//�����ڴ�ʹ��
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

//�����߼�
//    if(pthread_create(&upgrade_info_tid, NULL, (void* (*)(void*))init_upgrade_info_thread, NULL))
//    {
//        fprintf (stderr, "upgrade_info_upgrade_failed\n");
//        exit(-1);
//    }
//    DEBUG_UPGRADE_DUMP();
//    exit(-1);
//mysql �������
//    if(pthread_create(&mysql_queue_tid, NULL, (void* (*)(void*))mysql_queue, NULL))
//    {
//        fprintf (stderr, "epoll create error\n");  
//        exit(-1);
//    }
//    stop_SQL();
//    exit(-1);
//��ʼ��RSA����
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
