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

#ifndef AD_HTTP_REQUEST_H
#define AD_HTTP_REQUEST_H

#define CLRF "\r\n"
#define NULL_CHAR '\0';
#define AD_HTTP_REQUEST_MAX_SIZE 4096

typedef struct {
    char *name;
    char *field;
}ad_http_header;

#define NAME(h) ((h) -> name)
#define FIELD(h) ((h) -> field)

typedef struct {
    char *method;
    char *uri;
    char *version;
    ad_http_header **headers;
}ad_http_request;

#define METHOD(r)   ((r) -> method)
#define URI(r)      ((r) -> uri)
#define VERSION(r)  ((r) -> version)
#define HEADERS(r)  ((r) -> headers)

int ad_http_request_is_valid(ad_http_request request);

ad_http_request *ad_http_request_parse(char *request);

void ad_http_request_parse_header(ad_http_request *http_request, char *header_str) ;

void ad_http_request_free(ad_http_request *http_request);

#endif
