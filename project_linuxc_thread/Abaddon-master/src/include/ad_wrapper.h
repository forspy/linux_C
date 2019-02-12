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

#ifndef AD_WRAPPER_H
#define AD_WRAPPER_H
#include <sys/socket.h>

void system_error(const char *error_msg);

void kill_server(int return_value, char *func_name);

int Socket(int family, int type, int protocol);

int Bind(int sockfd, const struct sockaddr *myaddr, socklen_t addrlen);

int Listen(int sockfd, int backlog);

int Accept(int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen);

#endif
