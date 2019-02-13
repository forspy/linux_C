#ifndef _MYSQL_H_
#define _MYSQL_H_
#include <mysql/mysql.h>
#include "stdio.h"
#include "string.h"

//一条命令最多发2k数据
#define MAX_SQL_CMD_LENGTH 2*1024

typedef struct _SQL_CMD_QUEUE_ {
    char sqlCmd[MAX_SQL_CMD_LENGTH];
}SQL_CMD_QUEUE;


typedef struct _SQL_STRUCT_ {
    char sqlCmd[MAX_SQL_CMD_LENGTH];
    int logFd;
    MYSQL my_connection;
}SQL_STRUCT;


extern char sqlUserName[255];
extern char sqlPwd[255];
extern char sqlIP[255];
extern char sqlDatabaseName[255];
extern int sqlPort;
//extern char sqlCmd[MAX_SQL_CMD_LENGTH];
extern SQL_STRUCT main_sql;

int containIllegalCharacter(char string[]);
int init_SQL();
int stop_SQL();
int check_SQL_error(SQL_STRUCT*);
void* mysql_queue();
void insert_cmd_queue(unsigned char* cmd, int length);
void* connect_new_SQL(MYSQL* mysql_connection);

#define MAX_CMD_QUEUE_LENGTH                1024

#define MYSQL_CMD_BEGIN                     "begin;"

#define MYSQL_CMD_ROLLBACK                  "rollback;"

#define MYSQL_CMD_COMMIT                    "commit;"

#define MYSQL_CMD_FORMAT_GET_UPGRADE_INFO   "select versionA,versionB,versionC,url,name,MD5,cmd,TYPE from upgrade_info;"



#endif
