#include <stdio.h>
#include <stdlib.h>

#include "ad_queue.h"

/* Constructs a queue using the given mutex and condition variable
 *
 * @param mutex     the mutual exlusion structure for locking 
 *                  and unlocking the queue.
 * @param cond_var  the condition variable for waiting
 *                  the queue when it is not available.
 * @return          pointer to the constructed queue.
 */
ad_queue *ad_queue_construct(ad_queue_mutex *mutex, ad_queue_cond *cond_var)//创建任务池对象
{
    ad_queue *queue = malloc(sizeof(ad_queue));//分配任务池内存
//按照ad_queu结构体的定义初始化
    NODE_COUNT(queue) = 0;
    HEAD(queue)  = NULL;
    LAST(queue)  = NULL;
    MUTEX(queue) = mutex;
    COND(queue)  = cond_var;

    return queue;
}

/* Pushes the given void pointer to the given queue.
 *
 * @param queue  the queue data is going to be pushed on.
 * @param data   the void pointer of the data that needs
 *               to be queued.
 */
void ad_queue_push(ad_queue *queue, void *data)
{

    ad_node *node = malloc(sizeof(ad_node)); 
    
    DATA(node) = data;
    NEXT(node) = NULL;

    pthread_mutex_lock(MUTEX(queue));

    if(!IS_EMPTY(queue))
    {
        NEXT(LAST(queue)) = node;
        LAST(queue) = node;
    }
    else
    {
        HEAD(queue) = node;
        LAST(queue) = node;
    }

    NODE_COUNT(queue)++;

    pthread_mutex_unlock(MUTEX(queue));
    
    /* New node in the queue to be handled. Send the signal */
    pthread_cond_signal(COND(queue));
}

/* Pops the oldest node pointer residing in the queue.
 *
 * @param  queue the queue to be used for popping.
 * @return       void pointer to the data or NULL if the
 *               queue is empty.
 */
void *ad_queue_pop(ad_queue *queue)
{
    void *data = NULL;
    ad_node *node = NULL;

    pthread_mutex_lock(MUTEX(queue));//先锁定

    node = HEAD(queue);

    if(!IS_EMPTY(queue))//如果链表不为空就弹出一个
    {
        if (NODE_COUNT(queue) == 1)
        {
            LAST(queue) = NULL;
        }
        HEAD(queue) = NEXT(HEAD(queue));
        NODE_COUNT(queue)--;
    }

    pthread_mutex_unlock(MUTEX(queue));

    data = node ? DATA(node) : NULL;//看看node里面有没有data类型的指针？有的话data=node->data 没有data就为null
    free(node);

    return data;
}

/* Returns the node count currently in the queue.
 *
 * @param queue the queue being queried the node count.
 * @return      node count of the given queue.
 */
int ad_queue_get_node_count(ad_queue *queue)
{
    /* Most modern architectures process
       integer reads atomically */
    return NODE_COUNT(queue);
}

/* Pops every node in the queue and frees
 * the contents. Then, frees the queue.
 *
 * @param queue the queue to be destructed.
 */
void ad_queue_destruct(ad_queue *queue)
{
    void *data;

    while(data = ad_queue_pop(queue))
    {
        free(data);
    }

    free(queue);
}
