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

#ifndef AD_SERVER_H
#define AD_SERVER_H

#include <setjmp.h>

#define AD_SERVER_CONNECTION_BACKLOG 128
#define AD_SERVER_SOCKET_READ_TIMEOUT_SECONDS 10
#define AD_SERVER_SOCKET_WRITE_TIMEOUT_SECONDS 10

int ad_server_is_terminating(void);

/* SIGINT handler */
void ad_server_terminate(int signum);

int  ad_server_listen(unsigned short int server_port);

void ad_server_answer(int client_socket, jmp_buf error_jmp);

#endif
