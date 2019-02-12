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
#include <setjmp.h>

#include "ad_thread.h"
#include "ad_server.h"

/* Makes the necessary cleaning operations of the thread
 * if the thread is canceled.
 *
 * @param mutex the mutex that is used by the thread
 */
void ad_thread_cancel_cleanup(void *mutex)
{
    if (mutex)
    {
        pthread_mutex_unlock((ad_queue_mutex *) mutex);//解锁线程在任务池中锁定的一部分资源
    }
}

/* Is used for extending the tasks during request handling.
 *
 * @param request the pointer of the request to be handled.
 */
void ad_thread_handle_requests_hook(ad_thread_request *request)
{
    jmp_buf error_jmp;
    int jmp_status;

    int client = VOIDPTR_TO_INT(request);//这个client就像socket中的fd句柄
    free(request);

    /* Simulating try catch */
    jmp_status = setjmp(error_jmp);

    if(jmp_status != 1) {ad_server_answer(client, error_jmp);}//真正任务逻辑处理函数
}

/* Locks the mutex of the given queue pops a request
 * and handles the request. Yields the CPU when there are no
 * requests in the request queue
 *
 * @param thread_parameters parameter structure for passing info to the thread.
 */
void *ad_thread_handle_requests(void *thread_parameters)
{
    void *request = NULL;
	//从thread_parameters提取信息
    ad_queue_mutex *queue_mutex = MUTEX((ad_thread_parameters *) thread_parameters);
    ad_queue_cond *queue_cond   = COND((ad_thread_parameters *) thread_parameters);
    ad_queue *request_queue     = REQUEST_QUEUE((ad_thread_parameters *) thread_parameters);
    free(thread_parameters);//为了节省空间可以释放掉，反正已经提取了，也可以不释放

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//设置当前线程可以被其他线程取消掉
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);//设置取消方式

    pthread_cleanup_push(ad_thread_cancel_cleanup, (void *) queue_mutex);//栈式处理的回调函数push的时候不处理
    //当pop即线程退出时产生作用的清理函数，回调ad_thread_cancel_cleanup进行清理，参数queue_mutex

    pthread_mutex_lock(queue_mutex);

    for ( ; ; )
    {
        request = ad_queue_pop(request_queue);//先出任务队列中弹出一个节点任务，得到的是节点中void*类型的data，
        //data什么类型都能放比如cretsocket的fd，那么这个线程就处理这个socket

        if (request)//如果有节点任务进程处理
        {
            /* Let other threads access the request queue while handling a request */
            pthread_mutex_unlock(queue_mutex);//解锁

            /* Hook can be changed easily to be used in different setups */
            ad_thread_handle_requests_hook(request);//处理这个任务

            pthread_mutex_lock(queue_mutex);//锁定
        }
        else//如果request为空就睡眠
        {
            pthread_cond_wait(queue_cond, queue_mutex);
        }
    }

    pthread_cleanup_pop(0);//pop时进行清理
}
