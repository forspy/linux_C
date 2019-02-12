/**************************************************************************
 *  Abaddon HTTP Server
 *  Copyright (C) 2014  Tamer Tas
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "ad_thread_pool.h"
#include "ad_server.h"

/* Constructs and populates a thread pool with the given mutex, 
 * condition variable and the queue that is used for holding 
 * received requests. Consists of a thread queue, a request queue
 * and a thread pool maintainer thread. 
 *
 * @param   mutex          the mutex for the thread queue.
 * @param   cond_var       the condition variable for the thread queue.
 * @param   request_queue  pointer to the queue holding requests.
 * @return                 constructed thread pool's pointer.
 */
ad_thread_pool *ad_thread_pool_construct(ad_thread_pool_mutex *mutex, 
                                         ad_thread_pool_cond *cond_var, 
                                         ad_queue *request_queue)//按照ad_queue_construct()的方法进行代码重用
{
    int i;
    ad_thread_pool *thread_pool = malloc(sizeof(ad_thread_pool));//分配线程池内存
 
    THREAD_QUEUE(thread_pool) = ad_queue_construct(mutex, cond_var);//线程池中任务池的初始化

    REQUEST_QUEUE(thread_pool) = request_queue;//绑定系统自带的任务池对象

    /* Populate the pool */
    //创建线程池，并且压入线程池队列  唤醒等待线程池的程序
    for (i = 0; i < AD_THREAD_POOL_MIN_THREADS; i++)//AD_THREAD_POOL_MIN_THREADS=5
    {
        ad_thread_pool_create_thread(thread_pool);//设置当前线程
    }

    return thread_pool;
}

/* Creates a thread for handling the requests and puts it
 * into the thread queue of the thread pool
 *
 * @param thread_pool the thread pool for pushing the created thread.
 */
void ad_thread_pool_create_thread(ad_thread_pool *thread_pool)
{
    ad_thread_attr attr;//当前线程的属性
    ad_thread *new_thread = malloc(sizeof(ad_thread));//开辟一个新的线程TID
    ad_thread_parameters *parameters = malloc(sizeof(ad_thread_parameters));//线程参数
/*
typedef struct {
    ad_queue_mutex *mutex;
    ad_queue_cond *cond;
    ad_queue *request_queue;
}ad_thread_parameters;
*/
/*
typedef struct {
    ad_thread *pool_maintainer;线程池的数组，线程链表  
    ad_queue *thread_queue;线程队 列--任务池  
    ad_queue *request_queue;系统自带线程队列   
}ad_thread_pool;
*/
/*
typedef struct {
    int node_cou nt;//节点数--有几个任务  
    ad_node *head; //指向头节点--头指针  
    ad_node *last; //指向尾节点--尾指针  
    ad_queue_mutex  *mutex;//互斥体  
    ad_queue_cond  *cond;//条件变量  
}ad_queue;//线 程队列--任务池  
*/

    MUTEX(parameters) = MUTEX(REQUEST_QUEUE(thread_pool));//把线程池里系统自带线程队列的互斥体 赋给新开线程参数的互斥体
    COND(parameters) = COND(REQUEST_QUEUE(thread_pool));//条件变量赋值
    REQUEST_QUEUE(parameters) = REQUEST_QUEUE(thread_pool);//系统自带线程队列赋值
        //创建线程为独立线程
    pthread_attr_init(&attr);//当前线程的属性初始化
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//将分离属性PTHREAD_CREATE_DETACHED设置到attr属性集里面，使得线程独立
    pthread_create(new_thread, &attr, ad_thread_handle_requests, (void *) parameters);//做好准备工作以后创建线程创建，ad_thread_handle_requests为回调函数parameters为传入函数

    ad_queue_push(THREAD_QUEUE(thread_pool), (void *) new_thread);
}

/* Pops a thread from the thread queue of the thread pool.
 * Cancels the thread and frees allocated space.
 *
 * @param thread_pool the thread pool to delete a thread from.
 * @return            integer indicating success or failure.
 */
int ad_thread_pool_delete_thread(ad_thread_pool *thread_pool)
{
    ad_thread *thread = (ad_thread *) ad_queue_pop(THREAD_QUEUE(thread_pool));

    if(thread)
    {
        pthread_cancel(*thread);
        free(thread);
        return 1;
    }
    return 0;
}

/* Returns the count of threads currently in 
 * the thread pool's thread queue
 *
 * @param thread_pool the thread pool to be queried.
 * @return            count of the threads in the thread pool.
 */
int ad_thread_pool_get_thread_count(ad_thread_pool *thread_pool)
{
    return ad_queue_get_node_count(THREAD_QUEUE(thread_pool));
}

/* Pops all the threads from the thread pool ,destructs
 * the thread queue and frees the space allocated for the
 * thread pool
 *
 * @param thread_pool the thread pool to be destructed.
 */
void ad_thread_pool_destruct(ad_thread_pool *thread_pool)
{
    while(ad_thread_pool_delete_thread(thread_pool));

    ad_queue_destruct(THREAD_QUEUE(thread_pool));
    free(thread_pool);
}

/* Experimental. Not finished yet!
 * Resizes the thread pool according to the server load.
 *
 * @param thread_pool_voidptr void pointer to the thread pool
 *                            to be mainted.
 */
void *ad_thread_pool_maintain(void *thread_pool_voidptr)
{
    int request_count, thread_count;
    ad_thread_pool *thread_pool = (ad_thread_pool *) thread_pool_voidptr;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    pthread_cleanup_push(ad_thread_cancel_cleanup, (void *) MUTEX(THREAD_QUEUE(thread_pool)));

    pthread_mutex_lock(MUTEX(THREAD_QUEUE(thread_pool)));
    
    for ( ; ; )
    {
        thread_count = ad_queue_get_node_count(THREAD_QUEUE(thread_pool));

        pthread_mutex_lock(MUTEX(REQUEST_QUEUE(thread_pool)));
        request_count = ad_queue_get_node_count(REQUEST_QUEUE(thread_pool));
        pthread_mutex_unlock(MUTEX(REQUEST_QUEUE(thread_pool)));

        if (request_count > AD_QUEUE_HIGH_LIMIT && thread_count < AD_THREAD_POOL_MAX_THREADS)
        {
            ad_thread_pool_create_thread(thread_pool);
        }
        else if (request_count < AD_QUEUE_LOW_LIMIT && thread_count > AD_THREAD_POOL_MIN_THREADS)
        {
            ad_thread_pool_delete_thread(thread_pool);
        }
        else
        {
            if(ad_server_is_terminating())
            {
                pthread_mutex_unlock(MUTEX(THREAD_QUEUE(thread_pool)));

                pthread_exit(NULL);
            }
            else
            {
                pthread_cond_wait(COND(THREAD_QUEUE(thread_pool)), MUTEX(THREAD_QUEUE(thread_pool)));
            }
        }
    }

    pthread_cleanup_pop(0);
}
