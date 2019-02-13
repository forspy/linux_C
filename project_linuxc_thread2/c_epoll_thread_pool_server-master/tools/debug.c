#include "debug.h"
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h> 
#include <stdlib.h>
#include <pthread.h>
#include <execinfo.h>

char log_file[MAX_LOG_FILE_NAME_LENGTH] = "";


int log_level = DEBUG_LOGLEVEL_ALL_LOG;      //DEBUG_LOGLEVEL_NONE:NO LOG
static int fd = 1;             
pthread_mutex_t log_mutex;

void threadLog(int level,int fd,const char* fmt,...)
{
    va_list args;
    int i;
    time_t now;   
    struct tm *timenow;   
    char strtemp[255];
    char log_printbuf[MAX_LOG_INFO_LENGTH];
      
    time(&now);   
    timenow = localtime(&now);
    char tmpbuf[512];
//    printf("recent time is : %s /n", asctime(timenow));

    if(level < log_level)
    {
        return;
    }
    pthread_mutex_lock(&log_mutex);
    va_start(args, fmt);
    
    sprintf(tmpbuf,"%s",asctime(timenow));
    i = write(fd,tmpbuf,strlen(tmpbuf));
    i = write(fd,log_printbuf,vsprintf(log_printbuf, fmt, args));
    va_end(args);
    pthread_mutex_unlock(&log_mutex);
}

void printLog(int level,const char* fmt,...)
{
    va_list args;
    int i;
    time_t now;   
    struct tm *timenow;   
    char strtemp[255];
    char log_printbuf[MAX_LOG_INFO_LENGTH];
      
    time(&now);   
    timenow = localtime(&now);
    char tmpbuf[512];
//    printf("recent time is : %s /n", asctime(timenow));

    if(level < log_level)
    {
        return;
    }
    pthread_mutex_lock(&log_mutex);
    va_start(args, fmt);
    
    sprintf(tmpbuf,"%s",asctime(timenow));
    i = write(fd,tmpbuf,strlen(tmpbuf));
    i = write(fd,log_printbuf,vsprintf(log_printbuf, fmt, args));
    va_end(args);
    pthread_mutex_unlock(&log_mutex);
}

int init_thread_log(int i)
{
    char fileName[MAX_LOG_FILE_NAME_LENGTH+20];
    int fd;
    sprintf(fileName,"%s%d.log",log_file,i);

    fd = open(fileName,O_WRONLY|O_CREAT,0755);
    if(fd < 0)
    {
        fd = 1;         //default stdout
    }
    return fd;
}

void init_log()
{
    if (pthread_mutex_init(&log_mutex, NULL) !=0) {
        exit(1);
    }
    
    if(strlen(log_file)==0)
    {
        printf("no file recv");
        fd = 1;         //default stdout
        return;
    }

    fd = open(log_file,O_WRONLY|O_CREAT,0755);
    if(fd < 0)
    {
        fd = 1;         //default stdout
    }
}


void debug_dump_stack() {
    void *array[10];
    int size;
    char **strings;
    int i, j;

//    signal(signum, SIG_DFL);

    size = backtrace (array, 10);
    strings = (char **)backtrace_symbols (array, size);

    fprintf(stderr, "debug_dump_stack received SIGSEGV! Stack trace:\n");
    for (i = 0; i < size; i++) {
        fprintf(stderr, "%d %s \n",i,strings[i]);
    }

    free (strings);
}

static int malloc_count = 0;

pthread_mutex_t malloc_debug_mutex;

void my_debug_free(void* point)
{
    pthread_mutex_lock(&malloc_debug_mutex);
    malloc_count--;
    free(point);
//    fprintf(stderr,"my_debug_free count:%d\n",malloc_count);
//    debug_dump_stack();
    pthread_mutex_unlock(&malloc_debug_mutex);
}

void* my_debug_malloc(size_t size)
{
    pthread_mutex_lock(&malloc_debug_mutex);
    void* tmp;
    tmp = malloc(size);
    malloc_count++;
//    fprintf(stderr,"my_debug_malloc count:%d\n",malloc_count);
//    debug_dump_stack();
    pthread_mutex_unlock(&malloc_debug_mutex);
    return tmp;
}

void my_malloc_debug_init()
{
    if (pthread_mutex_init(&malloc_debug_mutex, NULL) !=0) {
        exit(1);
    }
}

void print_malloc_count(FILE* fds)
{
    fprintf(fds,"malloc count:%d\n",malloc_count);
}
