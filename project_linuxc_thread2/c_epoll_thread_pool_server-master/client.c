#include "client.h"
#include "debug.h"

Client_hash_node* clients_pre_login = NULL;     //根节点client无值
Client_hash_node* logined_clients = NULL;       //根节点client无值

unsigned int total_num;          //总共存储表大小,包括pool大小
//unsigned int cached_num = 100;
//操作Client_hash_node的锁
pthread_mutex_t hash_node_lock;

//预分配将要使用的Client_hash_node空间
static int max_pool_node_num;        //应该大于等于total_num
static void* Client_hash_node_pool = NULL;
static int lastClientHashNodeIndex = 0;
static int Client_hash_node_num = 0;

//预分配将要使用的Client空间
static void* Client_pool = NULL;
static int lastClientIndex = 0;
int Client_num = 0;

void init_Client_hash_node_pool();
void init_Client_pool();

int clients_hash_init(int number)
{
    pthread_mutex_init(&hash_node_lock, NULL);
    total_num = (number+1);
    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d have %d size\n",__FUNCTION__,__LINE__,total_num);
    logined_clients = (Client_hash_node*)MALLOC(total_num*sizeof(Client_hash_node));
    clients_pre_login = (Client_hash_node*)MALLOC(total_num*sizeof(Client_hash_node));

    if(logined_clients == NULL)
    {
        return -1;
    }

    memset((void*)logined_clients,0,total_num*sizeof(Client_hash_node));
    memset((void*)clients_pre_login,0,total_num*sizeof(Client_hash_node));

    max_pool_node_num = total_num * 1.5;    //预留一部分空间，使得分配内存的时候更快速。

    init_Client_hash_node_pool();
    init_Client_pool();
    
    return 0;
}

void init_Client_hash_node_pool()
{
    Client_hash_node_pool = MALLOC(max_pool_node_num*(sizeof(int)+sizeof(Client_hash_node)));
    memset(Client_hash_node_pool,0,max_pool_node_num*(sizeof(int)+sizeof(Client_hash_node)));
    lastClientHashNodeIndex = 0;
    Client_hash_node_num = 0;
}

void init_Client_pool()
{
    Client_pool = MALLOC(max_pool_node_num*(sizeof(int)+sizeof(Client)));
    memset(Client_pool,0,max_pool_node_num*(sizeof(int)+sizeof(Client)));
    lastClientIndex = 0;
    Client_num = 0;
}

void* get_Client_hash_node()
{
    int* flag;
    if(Client_hash_node_num >= max_pool_node_num)
    {
        return NULL;
    }
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"get_Client_hash_node\n");
    while(true)
    {
        DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"find Client_hash_node_num:%d,lastClientHashNodeIndex:%d,%lx\n",
                    Client_hash_node_num,lastClientHashNodeIndex);
        flag = (int*)(Client_hash_node_pool + lastClientHashNodeIndex*(sizeof(int)+sizeof(Client_hash_node)));
        if(*flag == 0)
        {
            lastClientHashNodeIndex = (lastClientHashNodeIndex+1) % max_pool_node_num;
            Client_hash_node_num ++;
            *flag = 1;
            return (void*)(flag+1);
        }
        lastClientHashNodeIndex = (lastClientHashNodeIndex+1) % max_pool_node_num;
    }
}

void free_Client_hash_node(void* point)
{
    int* flag;
    flag = point-sizeof(int);
    if(*flag == 1)
    {
        *flag = 0;
        Client_hash_node_num --;
//        lastClientHashNodeIndex --;
    }
}

void* get_Client()
{
    int* flag;
    if(Client_num >= max_pool_node_num)
    {
        return NULL;
    }
    while(true)
    {
        flag = (int*)(((char*)Client_pool) + lastClientIndex*(sizeof(int)+sizeof(Client)));
        if(*flag == 0)
        {
            lastClientIndex = (lastClientIndex+1) % max_pool_node_num;
            Client_num ++;
            *flag = 1;
            
            return (void*)(flag+1);
        }
        lastClientIndex = (lastClientIndex+1) % max_pool_node_num;
    }
}

void for_every_client_connected(void (*func) (Client *))
{
    int i;
    int *flag;
    for(i=0;i<max_pool_node_num;i++)
    {
        flag = (int*)(Client_pool + i*(sizeof(int)+sizeof(Client)));
        if(*flag == 1)
        {
            func((Client*)(flag+1));
        }
    }
}

void free_Client(void* point)
{
    int* flag;
    flag = point-sizeof(int);
    if(*flag == 1)
    {
        *flag = 0;
        Client_num --;
//        lastClientIndex --;
    }
}

int add_client_hash_node_to_logined_clients(Client_hash_node* node,SQL_STRUCT* current_sql)
{
    unsigned int index;
    unsigned int key;
    unsigned int* tmp_int;
    
    Client_hash_node* tmp = NULL;
    
    if(!node || !(node->client))
    {
        return -1;
    }

    pthread_mutex_lock(&hash_node_lock);
    
    tmp_int = (unsigned int *)&(node->client->identifyID[PUBLIC_ID_LENGTH-sizeof(int)]);
    index = (*tmp_int)%total_num;

    tmp = &logined_clients[index];
    while(tmp)
    {
        if(tmp->client && (memcmp(tmp->client->identifyID,node->client->identifyID,PUBLIC_ID_LENGTH)==0)
                && (!tmp->client->client_over))
        {
            THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d add conflict\n",__FUNCTION__,__LINE__);

            pthread_mutex_unlock(&hash_node_lock);
            return -1;
        }
        if(tmp->next_login_table)
        {
            THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d add next\n",__FUNCTION__,__LINE__);
            tmp = tmp->next_login_table;
            continue;
        }
        
        THREADDEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d add at %lx\n",__FUNCTION__,__LINE__,tmp);
        tmp->next_login_table = node;
        node->prev_login_table = tmp;
        node->next_login_table = NULL;
        break;
    }
    pthread_mutex_unlock(&hash_node_lock);
    return index;
}

Client_hash_node* get_client_hash_node_in_logined_clients(unsigned char* pubID,SQL_STRUCT* current_sql)
{
    unsigned int index;
    unsigned int key;
    unsigned int* tmp_int;
    
    Client_hash_node* tmp = NULL;
    if(!pubID)
    {
        return NULL;
    }
    tmp_int = (unsigned int *)&(pubID[PUBLIC_ID_LENGTH-sizeof(int)]);
    index = (*tmp_int)%total_num;

//    DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d get hash index%d\n",__FUNCTION__,__LINE__,index);

    tmp = &logined_clients[index];
    while(1)
    {
        if((tmp->client) && (memcmp(pubID,tmp->client->identifyID,PUBLIC_ID_LENGTH) == 0))
        {
//            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d get it %x\n",__FUNCTION__,__LINE__,tmp);
            return tmp;
        }
        else if(tmp->next_login_table)
        {
//            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d not %lx\n",__FUNCTION__,__LINE__,tmp);
            tmp = tmp->next_login_table;
        }
        else
        {
//            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"%s %d null\n",__FUNCTION__,__LINE__);
            return NULL;
        }
    }
}


int add_fd_to_client_hash(int fd,time_t seconds)
{
    int index;
    Client_hash_node* tmp = NULL;
    if(clients_pre_login == NULL)
    {
        return -1;
    }

    index = fd % total_num;
    tmp = &clients_pre_login[index];
    
    pthread_mutex_lock(&hash_node_lock);
    while(1)
    {
        if(tmp->next)
        {
            tmp = tmp->next;
            continue;
        }
        else
        {
            tmp->next = (Client_hash_node*)get_Client_hash_node();
            //malloc(sizeof(Client_hash_node));

            DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"malloc fd:%d,%lx\n",fd,tmp->next);

            
            tmp->next->next = NULL;
            tmp->next->prev = tmp;
            tmp->next->client = NULL;
            tmp->next->prev_login_table = NULL;
            tmp->next->next_login_table = NULL;
            
            tmp->next->client = (Client*)get_Client();
            //malloc(sizeof(Client));
            tmp->next->client->fd = fd;
            tmp->next->client->controlSocket = 0;
            tmp->next->client->dataLength = 0;
            tmp->next->client->status = CONNECTED_STATUS;
            tmp->next->client->recvMessageNum = 0;
            tmp->next->client->pktNum = 0;
            tmp->next->client->identifyed = 0;
            tmp->next->client->carID = 0;
            tmp->next->client->in_thread_flag = 0;
            tmp->next->client->client_over = 0;
            tmp->next->client->time = seconds + NO_LOGIN_CLIENT_TIME_OUT_SECOND;
            break;
        }
    }
    pthread_mutex_unlock(&hash_node_lock);
    return index;
}

Client_hash_node* get_client_hash_node_by_fd(int fd)
{
    int index;
    Client_hash_node* tmp = NULL;

    if(clients_pre_login == NULL)
    {
        return false;
    }

    index = fd % total_num;
    tmp = &clients_pre_login[index];
    while(1)
    {
        if(tmp == NULL)
        {
        DEBUGPRINT;
            return NULL;
        }
        if(tmp->client && tmp->client->fd == fd)
        {
        DEBUGPRINT;
            return tmp;
        }
        if(tmp->next)
        {
        DEBUGPRINT;
            tmp = tmp->next;
            continue;
        }
        return NULL;
    }
}


int rm_client_hash_node_by_fd(int fd)
{
    int index;
    Client_hash_node* tmp = NULL;
    Client_hash_node* tmp2 = NULL;

    pthread_mutex_lock(&hash_node_lock);
    if(clients_pre_login == NULL)
    {
        pthread_mutex_unlock(&hash_node_lock);
        return false;
    }

    index = fd % total_num;
    tmp = &clients_pre_login[index];
    while(tmp)
    {
        if(tmp->client && tmp->client->fd == fd)
        {
            //找到了
            
            if(tmp->prev_login_table)
            {
                tmp->prev_login_table->next_login_table = tmp->next_login_table;
                if(tmp->next_login_table)
                {
                    tmp->next_login_table->prev_login_table = tmp->prev_login_table;
                }
            }
        
            if(tmp->prev)
            {
                tmp->prev->next = tmp->next;
                if(tmp->next)
                {
                    tmp->next->prev = tmp->prev;
                }
                DEBUGLOGPRINTF(DEBUG_LOGLEVEL_DEBUG_INFO,"free fd:%d,%lx\n",fd,tmp);
                close(tmp->client->controlSocket);
                free_Client(tmp->client);
//                free(tmp->client);
                free_Client_hash_node(tmp);
//                free(tmp);
                tmp = NULL;
            }
            else
            {
                close(tmp->client->controlSocket);
                free_Client(tmp->client);
//                free(tmp->client);
                tmp->client = NULL;
                Client_hash_node* T;
                if(tmp->next)
                {
                    T=tmp->next;
                    tmp->client = T->client;
                    tmp->next = T->next;
                    if(T->next)
                    {
                        T->next->prev = tmp;
                    }
                    free_Client_hash_node(T);
//                    free(T);
                }
            }
            pthread_mutex_unlock(&hash_node_lock);
            return true;
        }
        else if(tmp->next)
        {
            //未匹配
            tmp = tmp->next;
        }
        else
        {
            //没找到且没有了
            pthread_mutex_unlock(&hash_node_lock);
            return false;
        }
    }
    pthread_mutex_unlock(&hash_node_lock);
    return false;
}
