#include "mysql.h"
#include <pthread.h>
#include <unistd.h>
#include "stdlib.h"
#include "global.h"

//最终版此连接只能在更新升级信息的时候调用，其他线程不使用

SQL_STRUCT main_sql;
SQL_STRUCT queue_sql;

//最终版此连接只在准备上传数据的时候使用
//MYSQL my_queue_connection;
char sqlUserName[255];
char sqlPwd[255];
char sqlIP[255];
char sqlDatabaseName[255];

//char sqlCmd[MAX_SQL_CMD_LENGTH];

//mysql cmd queue lock
pthread_mutex_t queue_lock;

//
pthread_mutex_t init_mysql_lock;
//MYSQL* my_connections;

SQL_CMD_QUEUE sql_cmd_queue[MAX_CMD_QUEUE_LENGTH];
int sql_cmd_queue_head = 0;             //head代表将要发送此处数据
int sql_cmd_queue_tail = 0;             //tail无数据，代表将要在此填入数据

int sqlPort = 3306;


int containIllegalCharacter(char string[])
{
    static char illegalCharacter[] = {'\'','\"','\n','\r','\\',0x1a};
    char* tmp;
    int i;
    tmp = string;

    while(*tmp)
    {
        for(i=0;i<sizeof(illegalCharacter);i++)
            if(*tmp == illegalCharacter[i])
                return 1;
        tmp++;
    }
    
    return 0;
}


//TODO 定时检查数据库连接，在断开后需要重新连接数据库。

void* connect_SQL()
{
    pthread_mutex_lock(&init_mysql_lock);
    
    mysql_init(&(main_sql.my_connection));
    mysql_init(&(queue_sql.my_connection));
    
    if(mysql_real_connect(&(main_sql.my_connection),sqlIP,sqlUserName,sqlPwd,sqlDatabaseName,sqlPort,NULL,0))
    {
        if(mysql_real_connect(&(queue_sql.my_connection),sqlIP,sqlUserName,sqlPwd,sqlDatabaseName,sqlPort,NULL,0))
        {
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"connected database\n");
            
            pthread_mutex_unlock(&init_mysql_lock);
            mysql_query(&(main_sql.my_connection),"set names 'UTF8'");
            mysql_query(&(queue_sql.my_connection),"set names 'UTF8'");
            return NULL;
        }
    }
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"connect database failed\n");
    pthread_mutex_unlock(&init_mysql_lock);
    return NULL;
}

//建立一个新的数据库连接
void* connect_new_SQL(MYSQL* mysql_connection)
{
    pthread_mutex_lock(&init_mysql_lock);
    mysql_init(mysql_connection);
    
//    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"ip:%s,username:%s,pwd:%s,port:%d\n",sqlIP,sqlUserName,sqlPwd,sqlPort);
    if(mysql_real_connect(mysql_connection,sqlIP,sqlUserName,sqlPwd,"car_app",sqlPort,NULL,0))
    {
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"connected database\n");
        pthread_mutex_unlock(&init_mysql_lock);
        mysql_query(mysql_connection,"set names 'UTF8'");
        return NULL;
    }
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"connect database failed\n");
    pthread_mutex_unlock(&init_mysql_lock);
    return NULL;
}


int init_SQL()
{
    pthread_t sqlID;
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %s\n",__FUNCTION__,__LINE__,sqlUserName);
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %s\n",__FUNCTION__,__LINE__,sqlPwd);
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %s\n",__FUNCTION__,__LINE__,sqlIP);
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d %d\n",__FUNCTION__,__LINE__,sqlPort);
    memset(main_sql.sqlCmd,0,MAX_SQL_CMD_LENGTH);
    
    pthread_mutex_init(&queue_lock, NULL);
    pthread_mutex_init(&init_mysql_lock, NULL);
    connect_SQL();
}

int stop_SQL()
{
    mysql_close(&(main_sql.my_connection));
    mysql_close(&(queue_sql.my_connection));
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"disconnect database\n");
}

int check_SQL_error(SQL_STRUCT* current_sql)
{
    int errn;
    errn = mysql_errno(&(current_sql->my_connection));
    
    if(errn == 2006 || errn == 2013)
    {
        mysql_close(&(current_sql->my_connection));
        mysql_real_connect(&(current_sql->my_connection),sqlIP,sqlUserName,sqlPwd,"car_app",sqlPort,NULL,0);
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"try connect database again\n");
        mysql_query(&(current_sql->my_connection),"set names 'UTF8'");
        return 0;
    }
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_ERROR,"connect database failed\n");
    return -1;
}

//所有不需要返回值的命令均使用此函数来传递代码。
void insert_cmd_queue(unsigned char* cmd, int length)
{
    pthread_mutex_lock(&queue_lock);
    if((sql_cmd_queue_tail+1)% MAX_CMD_QUEUE_LENGTH == sql_cmd_queue_head)
    {
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"mysql queue full");
        pthread_mutex_unlock(&queue_lock);
        return ;
    }else if (length <= MAX_SQL_CMD_LENGTH)
    {
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"mysql queue add: %d %d cmd:",sql_cmd_queue_tail,sql_cmd_queue_head);
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s\n",cmd);
        snprintf(sql_cmd_queue[sql_cmd_queue_tail].sqlCmd,length+1,"%s",cmd);
        sql_cmd_queue_tail = (sql_cmd_queue_tail + 1) % MAX_CMD_QUEUE_LENGTH;
    }
    
    pthread_mutex_unlock(&queue_lock);
}


//执行sql_cmd_queue中的命令
void* mysql_queue()
{
    int status;
    int errn;
    while(1)
    {
        if(sql_cmd_queue_tail == sql_cmd_queue_head)
        {
            sleep(1);
            continue;
        }
        status = mysql_query(&(queue_sql.my_connection),sql_cmd_queue[sql_cmd_queue_head].sqlCmd);
        if(status == 0)
        {
            sql_cmd_queue_head = (sql_cmd_queue_head + 1) % MAX_CMD_QUEUE_LENGTH;
            continue;
        }
        errn = mysql_errno(&(queue_sql.my_connection));
        if(errn == 2006 || errn == 2013)
        {
            if(mysql_real_connect(&(queue_sql.my_connection),sqlIP,sqlUserName,sqlPwd,"car_app",sqlPort,NULL,0))
            {
                DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"connected database\n");
                mysql_query(&(queue_sql.my_connection),"set names 'UTF8'");
            }
        }
        else
        {
            //ignore this cmd
            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"cmd error:%s\n",sql_cmd_queue[sql_cmd_queue_head].sqlCmd);
            sql_cmd_queue_head = (sql_cmd_queue_head + 1) % MAX_CMD_QUEUE_LENGTH;
            continue;
        }
    }
}

