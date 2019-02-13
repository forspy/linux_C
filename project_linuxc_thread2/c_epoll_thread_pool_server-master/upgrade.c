#include "upgrade.h"
#include "mysql.h"
#include "stdlib.h"
#include <unistd.h>
#include <pthread.h>
#include "debug.h"

/*
 * ���ʱ�汾��Ҫ��Ϊ������a.b.c��aΪӲ���汾��bΪ��汾��
 * cΪС�汾��ʵ��ʵ�ֵ�ʱ��b��c�Ѿ�û��������ˣ���Ϊ����ʲôԭ�򶼻�ǿ��������
 */
struct UPGRADE_INFO *upgrade_info = NULL;
int upgrade_info_size = 0;

int should_upgrade_upgrade_info = 1;

int max_versionB[MAX_HARD_VERSION_NUM + 1];//��ͬӲ���汾�ŵ����apk�汾��
int max_versionC[MAX_HARD_VERSION_NUM + 1];//��ͬӲ���汾�ŵ����file�汾��

//struct VERSION_INDEX* versionBIndex;//versionBIndex[b]����x.b.x�����ļ���index
//struct VERSION_INDEX* versionCIndex;//versionCIndex[c]����x.x.c�ļ���index�б�

int versionBIndexSize = 0;
int versionCIndexSize = 0;
//����������
static pthread_mutex_t upgrade_lock;
/*
static void dump_VERSION_INDEX() {
    int i;
    struct VERSION_INDEX* tmp;
    printf("dump_VERSIONB_INDEX:\n");
    for(i=0;i<versionBIndexSize;i++)
    {
        printf("    versionB:%d\n",i);
        tmp = &versionBIndex[i];
        while(tmp!=NULL)
        {
            printf("        index:%d\n",tmp->index);
            tmp = tmp->next;
        }
    }
    
    printf("dump_VERSIONC_INDEX:\n");
    for(i=0;i<versionCIndexSize;i++)
    {
        printf("    versionC:%d\n",i);
        tmp = &versionCIndex[i];
        while(tmp!=NULL)
        {
            printf("        index:%d\n",tmp->index);
            tmp = tmp->next;
        }
    }
}
*/
static void dump_upgrade_info()
{
    int i;
    printf("\ndump_upgrade_info:\n");
    printf("size:%d\n",upgrade_info_size);
    for(i=0;i<upgrade_info_size;i++)
    {
    /*
    int versionA;
    int versionB;
    int versionC;
    unsigned char mainVersion[MAIN_VERSION_LEN];
    unsigned char MD5[MD5_STRING_LENGTH];
    unsigned char URL[MAX_URL_LENGTH];
    unsigned char name[MAX_FILE_NAME_LENGTH];
    unsigned char cmd[MAX_SIMPLE_CMD_LENGTH];
    int file_type;
    int isLast;
    */
        printf("    index:%d:\n",i);
        printf("    versionA:%d\n",upgrade_info[i].versionA);
        printf("    versionB:%d\n",upgrade_info[i].versionB);
        printf("    versionC:%d\n",upgrade_info[i].versionC);
        printf("    mainVersion:%s\n",upgrade_info[i].mainVersion);
        printf("    MD5:%s\n",upgrade_info[i].MD5);
        printf("    URL:%s\n",upgrade_info[i].URL);
        printf("    name:%s\n",upgrade_info[i].name);
        printf("    cmd:%s\n",upgrade_info[i].cmd);
        printf("    file_type:%d\n",upgrade_info[i].file_type);
        printf("    isLast:%d\n",upgrade_info[i].isLast);
        printf("    \n");
    }
}

static void dump_max_version() {
    int i;
    printf("dump_max_version:\n");
    printf("max_versionB:\n");
    for(i=0;i<MAX_HARD_VERSION_NUM+1;i++)
    {
        printf("    %d:%d\n",i,max_versionB[i]);
    }
    printf("max_versionC:\n");
    for(i=0;i<MAX_HARD_VERSION_NUM+1;i++)
    {
        printf("    %d:%d\n",i,max_versionC[i]);
    }
}

void DEBUG_UPGRADE_DUMP()
{
    int versionList[100];
    int size;
    int i;
    int versionA;
    int versionB;
    int versionC;
    
    dump_upgrade_info();
    dump_max_version();
    
    getUpgradeVersionList(1,0,1,1,99,versionList,&size);

    printf("test getversionlist:\n");
    printf("1 0 1 size:%d\n",size);
    for(i=0;i<size;i++)
    {
        printf(" %d ",versionList[i]);
    }
    printf("\n");

    
    getUpgradeVersionList(1,0,0,1,99,versionList,&size);

    printf("test getversionlist:\n");
    printf("1 0 0 size:%d\n",size);
    for(i=0;i<size;i++)
    {
        printf(" %d ",versionList[i]);
    }
    printf("\n");
    
    getUpgradeVersionList(1,1,0,1,99,versionList,&size);

    printf("test getversionlist:\n");
    printf("1 1 0 size:%d\n",size);
    for(i=0;i<size;i++)
    {
        printf(" %d ",versionList[i]);
    }
    printf("\n");
    
    getUpgradeVersionList(1,1,0,2,99,versionList,&size);

    printf("test getversionlist:\n");
    printf("1 1 0 size:%d\n",size);
    for(i=0;i<size;i++)
    {
        printf(" %d ",versionList[i]);
    }
    printf("\n");
    
    getUpgradeVersionList(1,8,3,99,1,versionList,&size);

    printf("test getversionlist:\n");
    printf("1 8 3 size:%d\n",size);
    for(i=0;i<size;i++)
    {
        printf(" %d ",versionList[i]);
    }
    printf("\n");

    getVersionNumByInt(1,&versionA,&versionB,&versionC);

    printf("%d %d %d\n",versionA,versionB,versionC);
    
//    dump_VERSION_INDEX();
}

//����Ӳ���汾�ţ���ȡ��ǰӲ���汾����������汾��
//���� inputVersionA
//��� versionA,versionB,versionC
void getVersionNumByInt(int inputVersionA,int* versionA,int* versionB,int* versionC)
{
    if(inputVersionA > MAX_HARD_VERSION_NUM)
    {
        return;
    }
    *versionA = inputVersionA;
    *versionB = max_versionB[inputVersionA];
    *versionC = max_versionC[inputVersionA];
}

//��������汾�ţ���ȡ����Щ�汾
//���� versionA,versionB,versionC,type,maxVersionListSize
//��� versionList,versionListSize
//��versionListSize > maxVersionListSizeʱ��Ӧ����Ϊ��versionList�ռ䲻��
//maxVersionListIndex����versionList[maxVersionListIndex]�ǺϷ���
//versionListSize����versionList��0��versionListSize-1���������
//typeΪ1��ʾapk��飬��1��ʾ������������
void getUpgradeVersionList(int versionA,int versionB,int versionC,
                           int type,int maxVersionListIndex,
                           int* versionList,int* versionListSize)
{
    int i,j,k;
    int l;
//    struct VERSION_INDEX* tmp;
    l = 0;

    for(i=0;i<upgrade_info_size;i++)
    {
        if((upgrade_info[i].isLast) && (upgrade_info[i].versionA == versionA))
        {
            if((type==1) && (upgrade_info[i].file_type == 1) && 
                (upgrade_info[i].versionB > versionB))
            {
                if(l <= maxVersionListIndex)
                {
                    versionList[l] = i;
                    l++;
                }
                else
                {
                    *versionListSize = maxVersionListIndex + 1;
                    return;
                }
            }
            if((type!=1)&&(upgrade_info[i].file_type != 1)&&(upgrade_info[i].versionC > versionC))
            {
                if(l <= maxVersionListIndex)
                {
                    versionList[l] = i;
                    l++;
                }
                else
                {
                    *versionListSize = maxVersionListIndex + 1;
                    return;
                }
            }
        }
        /*
        if( (upgrade_info[i].isLast) && 
            (upgrade_info[i].versionA == versionA) &&
            (((type==1)&&(upgrade_info[i].file_type == 1)) || ((type!=1)&&(upgrade_info[i].file_type != 1)))&&
            ((upgrade_info[i].versionB > versionB) ||
            (upgrade_info[i].versionC > versionC))
            )
        {
            if(l <= maxVersionListIndex)
            {
                versionList[l] = i;
                l++;
            }
            else
            {
                *versionListSize = maxVersionListIndex + 1;
                return;
            }
        }*/
    }
    *versionListSize = l;
    
    /*
    if(versionB < max_versionB[versionA])
    {
        if(l <= maxVersionListIndex)
        {
            versionList[l] = versionBIndex[versionA].index;
            l++;
        }
        else
        {
            *versionListSize = maxVersionListIndex + 1;
            return;
        }
    }
    for(i=versionC+1;i<max_versionC[versionA];i++)
    {
        tmp = &versionCIndex[i];
        while(tmp!=NULL)
        {
            if(upgrade_info[tmp->index].isLast)
            {
                if(l <= maxVersionListIndex)
                {
                    versionList[l] = tmp->index;
                    l++;
                }
                else
                {
                    *versionListSize = maxVersionListIndex + 1;
                    return;
                }
            }
            tmp = tmp->next;
        }
    }
    *versionListSize = l;
    */
}

//��ȡ���еİ汾�ţ�������buf����ȡ���������汾��
int getVersionNumSize()
{
    return upgrade_info_size;
}

//��ȡ��index��������Ϣ�����ɶԻ����Ϣ���и���
struct UPGRADE_INFO * getUpgradeInfo(int index)
{
    return upgrade_info+index;
}

//��versionIndex���е�version���У�����һ��Ԫ��index
static void addVersionIndex(struct VERSION_INDEX* versionIndex,int version,int index,int size)
{
    struct VERSION_INDEX* tmpIndex;
    if(size <= version)
    {
        return;
    }
    if(versionIndex[version].index == -1)
    {
        versionIndex[version].index = index;
        return;
    }
    tmpIndex = &versionIndex[version];
    while(tmpIndex->next!=NULL)
    {
        tmpIndex = tmpIndex->next;
    }
    tmpIndex->next = (struct VERSION_INDEX*)MALLOC(sizeof(struct VERSION_INDEX));
    tmpIndex->next->next = NULL;
    tmpIndex->next->index = index;
}

//�жϵ�ǰ������Ϣ��Ҫ�������ļ��Ƿ������°汾������name���ж�
static void findLatestFile()
{
    int i;
    int j;
    int versionA;
    int type;
    int findOlder;
    
    for(i=0;i<upgrade_info_size;i++)
    {
        //���ļ����Ƚ��ļ���
        if(upgrade_info[i].file_type == 1)
        {
            findOlder = 0;
            for(j=i+1;j<upgrade_info_size;j++)
            {
                if((upgrade_info[i].versionA == upgrade_info[j].versionA) &&
                   (upgrade_info[i].file_type == upgrade_info[j].file_type) &&
                   (upgrade_info[i].versionB < upgrade_info[j].versionB))
                {
                    findOlder = 1;
                    break;
                }
            }
        }
        else if(upgrade_info[i].file_type == 2)
        {
            findOlder = 0;
            for(j=i+1;j<upgrade_info_size;j++)
            {
                if((upgrade_info[i].versionA == upgrade_info[j].versionA) &&
                   (upgrade_info[i].file_type == upgrade_info[j].file_type) &&
                   (upgrade_info[i].versionC < upgrade_info[j].versionC) &&
                   (strcmp((const char*)upgrade_info[i].name,(const char*)upgrade_info[j].name) == 0))
                {
                    findOlder = 1;
                    break;
                }
            }
        }
        else if(upgrade_info[i].file_type == 3)
        {
            findOlder = 0;
        }
        else if(upgrade_info[i].file_type == 4)
        {
            versionA = upgrade_info[i].versionA;
            if((upgrade_info[i].versionB == max_versionB[versionA]) &&
               (upgrade_info[i].versionC == max_versionC[versionA]))
            {
                findOlder = 0;
            }
            else
            {
                findOlder = 1;
            }
        }
        upgrade_info[i].isLast = !findOlder;
    }
}


//Ԥ����������Ϣ��ʹ�ò��Ҹ�����
static void preHandleInfo()
{
    int i;
    findLatestFile();
}

//�����߳�
void* init_upgrade_info_thread()
{
    pthread_mutex_init(&upgrade_lock, NULL);
    while(1)
    {
        if(should_upgrade_upgrade_info)
        {
            upgrade_info_size = 0;
            memset(max_versionB,0,sizeof(max_versionB));
            memset(max_versionC,0,sizeof(max_versionC));
            versionBIndexSize = 0;
            versionCIndexSize = 0;
            init_upgrade_info();
            should_upgrade_upgrade_info = 0;
        }
        else {
            DEBUGPRINT;
            sleep(IDLE_TIME);
            DEBUGPRINT;
        }
    }
}


//�ڳ�ʼ�����ݿ�����
//ֻ�д˴�����дupgrade_info�����Ϣ
//�˺�����ҪΪ��ȡ������Ϣ��Ȼ�����Ԥ������
void init_upgrade_info()
{
    MYSQL_RES *res_set;
    MYSQL_ROW row;

    int i;
    int l;

    int versionA;
    int versionB;
    int versionC;

    unsigned char mainVersion[MAIN_VERSION_LEN];
    unsigned char MD5[MD5_STRING_LENGTH];
    unsigned char URL[MAX_URL_LENGTH];
    unsigned char name[MAX_FILE_NAME_LENGTH];
    unsigned char cmd[MAX_SIMPLE_CMD_LENGTH];
    unsigned char file_type_string[100];                //"APK":1,"FILE":2,"CMD":3,"LATEST_CMD":4
    int isLast;
    int n;

    pthread_mutex_lock(&upgrade_lock);
    
    mysql_query(&(main_sql.my_connection),MYSQL_CMD_FORMAT_GET_UPGRADE_INFO);
    //MYSQL_CMD_FORMAT_GET_UPGRADE_INFO;
    
    res_set = mysql_store_result(&(main_sql.my_connection));
    if(res_set == NULL)
    {
        int status;
        status = check_SQL_error(&main_sql);
        if(status == 0)
        {
            mysql_query(&(main_sql.my_connection),MYSQL_CMD_FORMAT_GET_UPGRADE_INFO);
            res_set = mysql_store_result(&(main_sql.my_connection));
            if(res_set == NULL)
            {
                pthread_mutex_unlock(&upgrade_lock);
                return ;
            }
        }
        else
        {
            pthread_mutex_unlock(&upgrade_lock);
            return ;
        }
    }
    l = mysql_num_rows(res_set);

    if(upgrade_info!=NULL)
    {
        FREE(upgrade_info);
        upgrade_info = NULL;
    }
    
    upgrade_info = (struct UPGRADE_INFO *)MALLOC(sizeof(struct UPGRADE_INFO) * l);
    memset(upgrade_info,0,sizeof(struct UPGRADE_INFO) * l);
    upgrade_info_size = l;
    
    memset(max_versionB,0,sizeof(max_versionB));
    memset(max_versionB,0,sizeof(max_versionC));
    
    for(i=0;i<l;i++)
    {
        n=0;
        row = mysql_fetch_row(res_set);
        if(row[0]!=NULL)
            n += sscanf(row[0],"%d",&versionA);
        if(row[1]!=NULL)
            n += sscanf(row[1],"%d",&versionB);
        if(row[2]!=NULL)
            n += sscanf(row[2],"%d",&versionC);
        if(row[3]!=NULL)
            n += sscanf(row[3],"%s",URL);
        if(row[4]!=NULL)
            n += sscanf(row[4],"%s",name);
        if(row[5]!=NULL)
            n += sscanf(row[5],"%s",MD5);
        /*
        n += sscanf(row[6],"%s",cmd);*/
        n++;
        if(row[6]!=NULL)
        {
            memcpy(cmd,row[6],strlen(row[6])+1);
        }
        if(row[7]!=NULL);
            n += sscanf(row[7],"%s",file_type_string);
        if(n < 8)
        {
            continue;
        }

        if(versionB > max_versionB[versionA])
        {
            max_versionB[versionA] = versionB;
        }

        if(versionC > max_versionC[versionA])
        {
            max_versionC[versionA] = versionC;
        }

        upgrade_info[i].versionA = versionA;
        upgrade_info[i].versionB = versionB;
        upgrade_info[i].versionC = versionC;
        snprintf((char*)upgrade_info[i].mainVersion,MAIN_VERSION_LEN,"%d.%d.%d",versionA,versionB,versionC);
        snprintf((char*)upgrade_info[i].MD5,MD5_STRING_LENGTH,"%s",MD5);
        snprintf((char*)upgrade_info[i].URL,MAX_URL_LENGTH,"%s",URL);
        snprintf((char*)upgrade_info[i].name,MAX_FILE_NAME_LENGTH,"%s",name);
        snprintf((char*)upgrade_info[i].cmd,MAX_SIMPLE_CMD_LENGTH,"%s",cmd);

        if(strcmp((char*)file_type_string,"APK") == 0)
        {
            upgrade_info[i].file_type = 1;
        }
        else if(strcmp((char*)file_type_string,"FILE") == 0)
        {
            upgrade_info[i].file_type = 2;
        }
        else if(strcmp((char*)file_type_string,"CMD") == 0)
        {
            upgrade_info[i].file_type = 3;
        }
        else if(strcmp((char*)file_type_string,"LATEST_CMD") == 0)
        {
            upgrade_info[i].file_type = 4;
        }
        else
        {
            upgrade_info[i].file_type = 0;
        }
    }
    mysql_free_result(res_set);
    preHandleInfo();

    DEBUG_UPGRADE_DUMP();
    pthread_mutex_unlock(&upgrade_lock);
}
