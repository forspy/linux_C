#ifndef _UPGRADE_H_
#define _UPGRADE_H_

#include "global.h"

#define MAIN_VERSION_LEN (32)
#define MD5_STRING_LENGTH (32+1)
#define MAX_URL_LENGTH (255+1)
#define MAX_FILE_NAME_LENGTH (255+1)
#define MAX_SIMPLE_CMD_LENGTH (255+1)

#define IDLE_TIME 60        //60s检查一次是否更新升级信息

#define MAX_HARD_VERSION_NUM 10             //最大硬件版本号，可以通过修改此值扩大

struct UPGRADE_INFO {
    int versionA;
    int versionB;
    int versionC;
    unsigned char mainVersion[MAIN_VERSION_LEN];
    unsigned char MD5[MD5_STRING_LENGTH];
    unsigned char URL[MAX_URL_LENGTH];
    unsigned char name[MAX_FILE_NAME_LENGTH];   //存储下来的文件名，cmd里面带的参数对应的文件名为此值
    unsigned char cmd[MAX_SIMPLE_CMD_LENGTH];
    int file_type;                              //1:主文件,2:其他文件,3:必须执行的升级命令,4:最近一次升级需要执行的命令
                                                //ps:主要都是1，2，3。
                                                //4类型只是为了以防万一，最好不要出现
                                                //3:的意思是，经过3对应的那个版本，一定需要执行此命令，
                                                //比如某次升级所需要输入命令太多太长，并且从低版本升级到高版本一定要执行此命令
                                                //则可以使用3。
                                                //如某次升级需要创建一堆文件夹，之后的版本都需要这些文件夹，
                                                //原命令长度不够，则可额外增加3类型。
                                                //4:暂时没想到好的例子。
                                                
    int isLast;                                 //1:此文件的最新版本
};

struct VERSION_INDEX {
    int index;                          //对应upgrade_info表中的位置
    struct VERSION_INDEX* next;
};


struct UPGRADE_INFO * getUpgradeInfo(int index);
void getUpgradeVersionList(int versionA,int versionB,int versionC,int type,int maxVersionListIndex,
                           int* versionList,int* versionListSize);
void getVersionNumByInt(int inputVersionA,int* versionA,int* versionB,int* versionC);
int getVersionNumSize();
void init_upgrade_info();
void DEBUG_UPGRADE_DUMP();
void* init_upgrade_info_thread();

#endif
