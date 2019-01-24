/******************************************************************************

                  ��Ȩ���� (C), 2017-2017, 

 ******************************************************************************
  �� �� ��   : server.h
  �� �� ��   : ����
  ��    ��   : 
  ��������   : 2017��7��11��
  ����޸�   :
  ��������   : server.c ��ͷ�ļ�
  �����б�   :
*
*

  �޸���ʷ   :
  1.��    ��   : 2017��7��11��
    ��    ��   : 
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * �ⲿ����˵��                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ⲿ����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ȫ�ֱ���                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#ifndef __SERVER_H__

#define __SERVER_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/*==============================================*
 *      include header files                    *
 *----------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/type.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <assert.h>


/*==============================================*
 *      constants or macros define              *
 *----------------------------------------------*/


/*==============================================*
 *      project-wide global variables           *
 *----------------------------------------------*/
#define LISTSIZE 100
#define BUFFSIZE 2048
#define  IPADDR 127.0.0.1
#define  PORT   1234


struct server_obj
{
	struct sockaddr_in seraddr;
	fd_set srv_fds;
	int    max_fd;
	int    clifds[LISTSIZE];
	int    current_clicnt;
};



/*==============================================*
 *      routines' or functions' implementations *
 *----------------------------------------------*/

extern static int SRV_accpet_process( int srvfd );
extern int SRV_creat_proc_server( const char *ip, int port );
extern void SRV_handle_client( int srvfd );
extern int  SRV_init( );
extern void SRV_release( void );
extern struct server_obj *g_ptSrvOBJ;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __SERVER_H__ */
