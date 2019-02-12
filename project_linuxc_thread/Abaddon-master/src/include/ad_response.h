#ifndef AD_RESPONSE_H
#define AD_RESPONSE_H

#include <setjmp.h>

#define CLRF "\r\n"
#define AD_RESPONSE_SERVER "Server: Abaddon" CLRF CLRF
#define AD_RESPONSE_HTTP_OK "HTTP/1.0 200 OK" CLRF CLRF
#define AD_RESPONSE_CLIENT_BAD_REQUEST "HTTP/1.0 400 Bad Request" CLRF CLRF
#define AD_RESPONSE_CLIENT_NOT_FOUND "HTTP/1.0 404 Not Found" CLRF CLRF
#define AD_RESPONSE_SERVER_INTERNAL_ERROR "HTTP/1.0 500 Internal Server Error" CLRF CLRF
#define AD_RESPONSE_SERVER_NOT_IMPLEMENTED "HTTP/1.0 501 Not Implemented" CLRF CLRF

typedef struct {
    int status_code;
    char *response;
}ad_response;

void ad_response_receive(int client, char *buff, size_t buff_len, jmp_buf error_jmp);

void ad_response_send(int client, char *response, jmp_buf error_jmp);

void ad_response_sendfile(int client, int file, jmp_buf error_jmp);

#endif

