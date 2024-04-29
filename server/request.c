#include "request.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../common.h"
#include <stdio.h>


struct request static g_req;


int is_valid_status_line(char *req_buff) {
    char *p = req_buff;
    //G P D H O T - valid methods
    if (*p != 'G' && *p != 'P' && *p != 'D' && *p != 'H' && *p != 'O' && *p != 'T')
        return 0;
    //quick hack! we pretend that req_buff is only one line to analyze status line
    while(*p++ != '\n');
    *p = '\0';

    if (strstr(req_buff, "HTTP/1.1") == NULL && 
        strstr(req_buff, "HTTPS/1.1") == NULL) {
        printf("%s", req_buff);
        return 0;        
    }
    *p = '\n';
    return 1;
}

struct request* parse_request(int client_fd, char *req_str, int mode) {

    //Validate HTTP request ignore || 400 || proceed
    //Not a http request, ignoring;
    if (!is_valid_status_line(req_str)) {
      return NULL;  
    }
    g_req.method = GET;
    g_req.v_http = V_HTTP_11;

    return &g_req;
}
