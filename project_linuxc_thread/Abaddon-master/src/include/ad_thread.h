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

#ifndef AD_THREAD_H
#define AD_THREAD_H

#include <pthread.h>
#include <setjmp.h>

#include "ad_queue.h"

typedef struct {
    void *data;
    jmp_buf error_jmp;
}ad_thread_request;

#define ERROR_JMP(r) ((r) -> error_jmp)

typedef struct {
    ad_queue_mutex *mutex;
    ad_queue_cond *cond;
    ad_queue *request_queue;
}ad_thread_parameters;

#define REQUEST_QUEUE(p) ((p) -> request_queue)

#define VOIDPTR_TO_INT(ptr) (*((int *) (ptr)))

void ad_thread_cancel_cleanup(void *mutex);

void ad_thread_handle_requests_hook(ad_thread_request *request);

void *ad_thread_handle_requests(void *thread_parameters);

#endif
