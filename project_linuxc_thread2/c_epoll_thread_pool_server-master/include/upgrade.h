#ifndef _UPGRADE_H_
#define _UPGRADE_H_

#include "global.h"

#define MAIN_VERSION_LEN (32)
#define MD5_STRING_LENGTH (32+1)
#define MAX_URL_LENGTH (255+1)
#define MAX_FILE_NAME_LENGTH (255+1)
#define MAX_SIMPLE_CMD_LENGTH (255+1)

#define IDLE_TIME 60        //60s���һ���Ƿ����������Ϣ

#define MAX_HARD_VERSION_NUM 10             //���Ӳ���汾�ţ�����ͨ���޸Ĵ�ֵ����

struct UPGRADE_INFO {
    int versionA;
    int versionB;
    int versionC;
    unsigned char mainVersion[MAIN_VERSION_LEN];
    unsigned char MD5[MD5_STRING_LENGTH];
    unsigned char URL[MAX_URL_LENGTH];
    unsigned char name[MAX_FILE_NAME_LENGTH];   //�洢�������ļ�����cmd������Ĳ�����Ӧ���ļ���Ϊ��ֵ
    unsigned char cmd[MAX_SIMPLE_CMD_LENGTH];
    int file_type;                              //1:���ļ�,2:�����ļ�,3:����ִ�е���������,4:���һ��������Ҫִ�е�����
                                                //ps:��Ҫ����1��2��3��
                                                //4����ֻ��Ϊ���Է���һ����ò�Ҫ����
                                                //3:����˼�ǣ�����3��Ӧ���Ǹ��汾��һ����Ҫִ�д����
                                                //����ĳ����������Ҫ��������̫��̫�������ҴӵͰ汾�������߰汾һ��Ҫִ�д�����
                                                //�����ʹ��3��
                                                //��ĳ��������Ҫ����һ���ļ��У�֮��İ汾����Ҫ��Щ�ļ��У�
                                                //ԭ����Ȳ�������ɶ�������3���͡�
                                                //4:��ʱû�뵽�õ����ӡ�
                                                
    int isLast;                                 //1:���ļ������°汾
};

struct VERSION_INDEX {
    int index;                          //��Ӧupgrade_info���е�λ��
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
