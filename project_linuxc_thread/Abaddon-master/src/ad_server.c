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
#define __USE_GNU
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "ad_server.h"
#include "ad_http_request.h"
#include "ad_response.h"
#include "ad_utils.h"
#include "ad_method.h"

#include <pthread.h>

#include "ad_queue.h"
#include "ad_thread.h"
#include "ad_thread_pool.h"

/* Signal flag indicating the state of the server */
volatile sig_atomic_t ad_server_terminating = 0;

/* Returns the signal flag that is indicating 
 * whether the server is terminating or not.
 *
 * @return value of the signal flag indicating 
 *         termination status
 */
int ad_server_is_terminating(void)
{
    return ad_server_terminating;
}

/* Acts as a SIGINT Handler. Turns on
 * the server termination flag
 *
 * @param signum integer value of the received signal
 */
void ad_server_terminate(int signum) 
{ 
    ad_server_terminating = 1;
    fprintf(stdout, "\nAbaddon HTTP server is terminating please wait!\n");
    fflush(stdout);
}

/* Parses the http request and responds accordingly to 
 * the connected client. Jumps out of the routine if 
 * there are errors during the transactions with the client.
 *
 * @param client_socket file descriptor of the client socket
 * @param error_jmp safe state to jump back to after encountering errors
 */
void ad_server_answer(int client_socket, jmp_buf error_jmp)
{
    int requested_file;
    char path[512];
    char buffer[AD_HTTP_REQUEST_MAX_SIZE];
    ad_http_request *http_request = NULL;

    ad_response_receive(client_socket, buffer, AD_HTTP_REQUEST_MAX_SIZE, error_jmp);

    http_request = ad_http_request_parse(buffer);

    if (http_request == NULL || METHOD(http_request) == NULL || !ad_method_is_valid(METHOD(http_request)))
    {
        ad_http_request_free(http_request);
        ad_response_send(client_socket, AD_RESPONSE_CLIENT_BAD_REQUEST, error_jmp);
    }
    else if (ad_utils_strcmp_ic(METHOD(http_request), "GET"))
    {
        ad_http_request_free(http_request);
        ad_response_send(client_socket, AD_RESPONSE_SERVER_NOT_IMPLEMENTED, error_jmp);
    }
    else if (!ad_utils_strcmp_ic(METHOD(http_request),"GET"))
    {
        sprintf(path, "htdocs%s", URI(http_request));
        if (ad_utils_is_directory(path))
        {
            strcat(path, "index.html");
        }

        ad_http_request_free(http_request);

        if ((requested_file = open(path, O_RDONLY)) == -1)
        {
            perror(path);
            ad_response_send(client_socket, AD_RESPONSE_CLIENT_NOT_FOUND, error_jmp);
        }
        else 
        {
            ad_response_send(client_socket, AD_RESPONSE_HTTP_OK, error_jmp);

            ad_response_sendfile(client_socket, requested_file, error_jmp);

            close(requested_file);
        }

    }

    shutdown(client_socket, SHUT_WR);

    ad_response_receive(client_socket, buffer, AD_HTTP_REQUEST_MAX_SIZE, error_jmp);

    close(client_socket);
}

/* Listens for connections on the given port number.
 * After accepting a connection, places it in to the 
 * requests queue for further processing.
 *
 * @param   server_port port to be binded with the server socket.
 * @return  EXIT_SUCCESS on a successful termination.
 */
int ad_server_listen(unsigned short int server_port)
{
    int  server_socket = -1;
    int  client_socket = -1;
    int *client_ptr = NULL;

    struct sigaction sig_struct;

    socklen_t server_len, client_len;
    struct sockaddr_in server_addr, client_addr;

    ad_queue *request_queue;//任务池指针
    ad_thread_pool *thread_pool;//线程池指针

    ad_queue_mutex request_mutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;//使用宏定义创建ad_queue_mutex线程互斥体并初始化，相当于创建并使用pthread_mutex_init(*request_mutex);初始化
    ad_thread_pool_mutex pool_mutex= PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;//也是ad_queue_mutex互斥体，只是命名变了

    ad_queue_cond add_request_cond = PTHREAD_COND_INITIALIZER;//条件变量初始化
    ad_thread_pool_cond add_thread_cond = PTHREAD_COND_INITIALIZER;//条件变量初始化

    signal(SIGPIPE, SIG_IGN);//信号屏蔽屏蔽掉SIGPIPE信号

    /* system calls (e.g. accept()) are restarted when we use sigaction *///设置信号屏蔽集合
    sigemptyset(&sig_struct.sa_mask);//清空信号集
    sig_struct.sa_handler = ad_server_terminate;//加入信号操作函数，回调函数
    sig_struct.sa_flags = 0;//设置当前信号状态
    sigaction(SIGINT, &sig_struct, NULL);//绑定信号SIGINT(ctrl+c)

    //任务队列与线程队列的创建 
    request_queue = ad_queue_construct(&request_mutex, &add_request_cond);//创建任务池对象。其实本质也是线程池，返回的是任务队列
    thread_pool = ad_thread_pool_construct(&pool_mutex, &add_thread_cond, request_queue);//创建线程池对象 ，需要传入任务池对象，返回的是池

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_len = sizeof(server_addr);

    /* Initialize and configure server socket address */
    bzero(&server_addr, server_len);
    server_addr.sin_port = htons(server_port);
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    if((bind(server_socket, (struct sockaddr *) &server_addr, server_len)) == -1)
    {
        perror("Abaddon HTTP server couldn't start");
        return EXIT_FAILURE;
    }

    listen(server_socket, AD_SERVER_CONNECTION_BACKLOG);
    printf("Abaddon HTTP server is listening on port %d\n", server_port);

    while (!ad_server_terminating)
    {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);

        if (client_socket == -1) 
        { 
            if(errno == EINTR) { /* The server received SIGINT */ }
            else               { perror("Client connection wasn't established\n"); }
        }
        else
        { 
            client_ptr = malloc(sizeof(int));
           *client_ptr = client_socket;
            //基于接收到的客户端监听句柄，创建新的任务结点，并且将任务节点入队
            ad_queue_push(request_queue, (void *)client_ptr); 

            /* Signals the thread pool to allow resizing of the pool */
            pthread_cond_signal(COND(THREAD_QUEUE(thread_pool)));
        }
    }
    
    close(server_socket);

    ad_queue_destruct(request_queue);
    ad_thread_pool_destruct(thread_pool);
    
    printf("Abaddon HTTP server terminated successfully\n");

    return EXIT_SUCCESS;
}
