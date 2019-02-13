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
        pthread_mutex_unlock((ad_queue_mutex *) mutex);//½âËøÏß³ÌÔÚÈÎÎñ³ØÖĞËø¶¨µÄÒ»²¿·Ö×ÊÔ´
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

    int client = VOIDPTR_TO_INT(request);//Õâ¸öclient¾ÍÏñsocketÖĞµÄfd¾ä±ú
    free(request);//ÒòÎªÈÎÎñÒÑ¾­±»Ïß³ÌÈ¡³öÀ´ÁË£¬ËùÒÔ¿ÉÒÔfreeµô£¬freeµôµÄÖ»ÊÇ½ÚµãÖĞµÄdata×Ö¶Î£ ÓÖÒòÎª£¬data×Ö¶ÎµÄµØÖ·¾ÍÊÇ½ÚµãµÄÍ·µØÖ·£¬ËùÒÔÊÇfreeÕû¸ö½Úµã

    /* Simulating try catch */
    jmp_status = setjmp(error_jmp);//²¶×½Òì³£

    if(jmp_status != 1) {ad_server_answer(client, error_jmp);}//ÕæÕıÈÎÎñÂß¼­´¦Àíº¯Êı
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
	//´Óthread_parametersÌáÈ¡ĞÅÏ¢
    ad_queue_mutex *queue_mutex = MUTEX((ad_thread_parameters *) thread_parameters);
    ad_queue_cond *queue_cond   = COND((ad_thread_parameters *) thread_parameters);
    ad_queue *request_queue     = REQUEST_QUEUE((ad_thread_parameters *) thread_parameters);
    free(thread_parameters);//ÎªÁË½ÚÊ¡¿Õ¼ä¿ÉÒÔÊÍ·Åµô£¬·´ÕıÒÑ¾­ÌáÈ¡ÁË£¬Ò²¿ÉÒÔ²»ÊÍ·Å

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//ÉèÖÃµ±Ç°Ïß³Ì¿ÉÒÔ±»ÆäËûÏß³ÌÈ¡Ïûµô
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);//ÉèÖÃÈ¡Ïû·½Ê½

    pthread_cleanup_push(ad_thread_cancel_cleanup, (void *) queue_mutex);//Õ»Ê½´¦ÀíµÄ»Øµ÷º¯ÊıpushµÄÊ±ºò²»´¦Àí
    //µ±pop¼´Ïß³ÌÍË³öÊ±²úÉú×÷ÓÃµÄÇåÀíº¯Êı£¬»Øµ÷ad_thread_cancel_cleanup½øĞĞÇåÀí£¬²ÎÊıqueue_mutex

    pthread_mutex_lock(queue_mutex);

    for ( ; ; )
    {
        request = ad_queue_pop(request_queue);//ÏÈ³öÈÎÎñ¶ÓÁĞÖĞµ¯³öÒ»¸ö½ÚµãÈÎÎñ£¬µÃµ½µÄÊÇ½ÚµãÖĞvoid*ÀàĞÍµÄdata£¬
        //dataÊ²Ã´ÀàĞÍ¶¼ÄÜ·Å±ÈÈçcretsocketµÄfd£¬ÄÇÃ´Õâ¸öÏß³Ì¾Í´¦ÀíÕâ¸ösocket

        if (request)//Èç¹ûÓĞ½ÚµãÈÎÎñ½ø³Ì´¦Àí
        {
            /* Let other threads access the request queue while handling a request */
            pthread_mutex_unlock(queue_mutex);//½âËø

            /* Hook can be changed easily to be used in different setups */
            ad_thread_handle_requests_hook(request);//´¦ÀíÕâ¸öÈÎÎñ

            pthread_mutex_lock(queue_mutex);//Ëø¶¨
        }
        else//Èç¹ûrequestÎª¿Õ¾ÍË¯Ãß
        {
            pthread_cond_wait(queue_cond, queue_mutex);
        }
    }

    pthread_cleanup_pop(0);//popÊ±½øĞĞÇåÀí
}
