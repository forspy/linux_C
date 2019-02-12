#include <stdlib.h>
#include <stdio.h>

#include "ad_server.h"

/* Example Abaddon usage */
int main(int argc, char *argv[]) 
{
    /* 9229 is the default port of Abaddon */
    int port = argc > 1 ? atoi(argv[1]) : 9229;

    return ad_server_listen(port);
}
