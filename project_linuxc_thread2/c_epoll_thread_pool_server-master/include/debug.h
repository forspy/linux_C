#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h> 
#include <stdlib.h>
#include <pthread.h>

#define MAX_LOG_FILE_NAME_LENGTH 1024
#define MAX_LOG_INFO_LENGTH 1024*50

extern char log_file[MAX_LOG_FILE_NAME_LENGTH];
extern int log_level;

void printLog(int level,const char* fmt,...);
void threadLog(int level,int fd,const char* fmt,...);
void init_log();
int init_thread_log(int i);
void my_malloc_debug_init();
void print_malloc_count(FILE* fds);

enum DEBUG_LOGLEVEL{
    DEBUG_LOGLEVEL_ALL_LOG = 0,
    DEBUG_LOGLEVEL_PKT_INFO,
    DEBUG_LOGLEVEL_DEBUG_INFO,
    DEBUG_LOGLEVEL_ERROR,
    DEBUG_LOGLEVEL_NONE,
};

#if 1

#define DEBUGPRINT printLog(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d\n",__FUNCTION__,__LINE__);
#define DEBUGPRINTF(fmt,...) printf(fmt, ##__VA_ARGS__)
#define DEBUGLOGPRINTF(level,fmt,...) printLog(level,fmt, ##__VA_ARGS__)
#define THREADDEBUGLOG(level,fd,fmt,...) threadLog(level,fd,fmt, ##__VA_ARGS__)

#define THREADDEBUGLOGPRINTF(level,fmt,...) threadLog(level,current_sql->logFd,fmt, ##__VA_ARGS__)

#define THREADDEBUGPRINT threadLog(DEBUG_LOGLEVEL_DEBUG_INFO,current_sql->logFd,"%s %d\n",__FUNCTION__,__LINE__);
#else

#define DEBUGPRINT 
#define DEBUGPRINTF(fmt,...) 
#define DEBUGLOGPRINTF(level,fmt,...) 
#define THREADDEBUGLOG(level,fd,fmt,...) 

#define THREADDEBUGLOGPRINTF(level,fmt,...) 

#define THREADDEBUGPRINT 

#endif
//#define THREADDEBUGPRINTF(fmt,...) printf(fmt, ##__VA_ARGS__)

#endif
