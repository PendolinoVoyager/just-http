#include "request.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../common.h"
#include <stdio.h>


static struct request  g_req;
static char   uri_buff[URI_BUFF_LEN];

int get_method(char *str) {
    //first 8 chars MUST BE a method.
    int count = 0;
    char *p = str;
    int retval = -1;
    while (*p++ != ' ' && count < MAX_STATUS_LINE) count++;
    *p = '\0';

    if      (strstr(str, "GET"))
        retval = GET;
    else if (strstr(str, "POST"))
        retval = POST;
    else if (strstr(str, "HEAD"))
        retval = HEAD;
    else if (strstr(str, "PUT"))
        retval = PUT;
    else if (strstr(str, "PATCH"))
        retval = PATCH;
    else if (strstr(str, "DELETE"))
        retval = DELETE;
    else if (strstr(str, "OPTIONS"))
        retval = OPTIONS;
    else if (strstr(str, "CONNECT"))
        retval = CONNECT;
    else if (strstr(str, "TRACE"))
        retval = TRACE;

    *p = '/';
    return retval;
}
/*
* return the end index of uri
*/
int get_uri(char *str, char *buff) {
    int count = 0;
    char *start = str;
    char *uri_b_p;
    while (*start++ != '/' && count < MAX_STATUS_LINE) count++;
    start--;
    uri_b_p = &buff[0];
    while (*start != ' ' && count < MAX_STATUS_LINE) {
        *uri_b_p++ = *start++;
        count++;
    }
    *uri_b_p = '\0';
    return count;
}
int get_v_http(char *str, int start) {
    return V_HTTP_11;
};
int     is_valid_host(char *str);
int     get_acc_type(char *str);

int parse_req_line(char *str, struct request *req) {
    int chars_parsed = 0;
    req->method = get_method(str);
    if (req->method == -1) return -1; 
    if ((chars_parsed = get_uri(str, uri_buff)) == MAX_STATUS_LINE - 1) return -1;
    req->url = uri_buff;
    req->v_http = get_v_http(str, chars_parsed);

    return 0;
}
void dump_req(struct request *req) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(stdout, "REQUEST DUMP %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    fprintf(stdout, "Method: %d\n",req->method);
    fprintf(stdout, "HTTP Version: %d\n",req->v_http);
    fprintf(stdout, "URI: %s\n",req->url);

}
struct request* parse_request(int client_fd, char *req_str, int encrypted, int mode) {
    //parsing req_line failed => not a valid HTTP request
    if (parse_req_line(req_str, &g_req) < 0) {
        return NULL;
    }
    g_req.v_http = V_HTTP_11;
    g_req.is_https = encrypted;
    if (mode == DEBUG) dump_req(&g_req);
    return &g_req;
}
