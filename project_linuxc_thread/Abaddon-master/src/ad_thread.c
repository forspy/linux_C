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
        pthread_mutex_unlock((ad_queue_mutex *) mutex);//�����߳����������������һ������Դ
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

    int client = VOIDPTR_TO_INT(request);//���client����socket�е�fd���
    free(request);

    /* Simulating try catch */
    jmp_status = setjmp(error_jmp);

    if(jmp_status != 1) {ad_server_answer(client, error_jmp);}//���������߼�������
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
	//��thread_parameters��ȡ��Ϣ
    ad_queue_mutex *queue_mutex = MUTEX((ad_thread_parameters *) thread_parameters);
    ad_queue_cond *queue_cond   = COND((ad_thread_parameters *) thread_parameters);
    ad_queue *request_queue     = REQUEST_QUEUE((ad_thread_parameters *) thread_parameters);
    free(thread_parameters);//Ϊ�˽�ʡ�ռ�����ͷŵ��������Ѿ���ȡ�ˣ�Ҳ���Բ��ͷ�

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//���õ�ǰ�߳̿��Ա������߳�ȡ����
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);//����ȡ����ʽ

    pthread_cleanup_push(ad_thread_cancel_cleanup, (void *) queue_mutex);//ջʽ����Ļص�����push��ʱ�򲻴���
    //��pop���߳��˳�ʱ�������õ����������ص�ad_thread_cancel_cleanup������������queue_mutex

    pthread_mutex_lock(queue_mutex);

    for ( ; ; )
    {
        request = ad_queue_pop(request_queue);//�ȳ���������е���һ���ڵ����񣬵õ����ǽڵ���void*���͵�data��
        //dataʲô���Ͷ��ܷű���cretsocket��fd����ô����߳̾ʹ������socket

        if (request)//����нڵ�������̴���
        {
            /* Let other threads access the request queue while handling a request */
            pthread_mutex_unlock(queue_mutex);//����

            /* Hook can be changed easily to be used in different setups */
            ad_thread_handle_requests_hook(request);//�����������

            pthread_mutex_lock(queue_mutex);//����
        }
        else//���requestΪ�վ�˯��
        {
            pthread_cond_wait(queue_cond, queue_mutex);
        }
    }

    pthread_cleanup_pop(0);//popʱ��������
}
