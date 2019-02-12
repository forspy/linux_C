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

#ifndef AD_QUEUE_H
#define AD_QUEUE_H

#include <pthread.h>

/* Queue data is in the form of a void ptr.
 * The queue can be used for many purposes.
 * It's the developer's responsibility to send
 * void ptr and typecast when popped from the queue. */
typedef struct node {
    void *data;//数据段
    struct node *next;//后向指针
    struct node *previous;//前向指针
}ad_node;

#define DATA(n)     ((n)->data)
#define NEXT(n)     ((n)->next)
#define PREVIOUS(n) ((n)->next)

typedef pthread_mutex_t ad_queue_mutex;
typedef pthread_cond_t ad_queue_cond;

typedef struct {
    int node_count;//节点数--有几个任务?
    ad_node *head;//指向头节点--头指针
    ad_node *last;//指向尾节点--尾指针
    ad_queue_mutex *mutex;//互斥体
    ad_queue_cond *cond;//条件变量
}ad_queue;//线程队列--任务池

#define NODE_COUNT(q) ((q)->node_count)
#define IS_EMPTY(q)   ((q)->node_count ? 0 : 1)
#define HEAD(q)       ((q)->head)
#define LAST(q)       ((q)->last)

#define MUTEX(q) ((q)->mutex)
#define COND(q)  ((q)->cond)

#define AD_QUEUE_LOW_LIMIT 3
#define AD_QUEUE_HIGH_LIMIT 15

ad_queue *ad_queue_construct(ad_queue_mutex *mutex, ad_queue_cond *cond_var);

void ad_queue_push(ad_queue *queue, void *data);

void *ad_queue_pop(ad_queue *queue);

int ad_queue_get_node_count(ad_queue *queue);

void ad_queue_destruct(ad_queue *queue);

#endif
