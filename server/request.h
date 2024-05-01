#include <sys/types.h>
#ifndef REQUEST_H
#define REQUEST_H

enum METHODS {
    GET,
    HEAD,
    POST,
    PUT,
    PATCH,
    DELETE,
    OPTIONS,
    CONNECT,
    TRACE
};
enum V_HTTP {
    V_HTTP_09,
    V_HTTP_10,
    V_HTTP_11,
    V_HTTP_20,
    V_HTTP_30
};

struct request {
    int     method;
    int     v_http;
    int     is_https;
    char*   url;
};

// Describes max length of status line (method, url, http version)
#define MAX_STATUS_LINE 128
#define URI_BUFF_LEN    100
#define WRAP_HTML_DBG(content, title) "<!DOCTYPE html><html><head><title>"#title"</title></head><body>"#content"</body></html>\n"
/*
*  plaintext
*/
struct request* parse_request(int client_fd, char *req_str, int encrypted, int mode);
int     get_method(char *str);
int     get_v_http(char *str, int start);
int     get_uri(char *str, char *buff);
int     is_valid_host(char *str);
int     get_acc_type(char *str);
void    dump_req(struct request *req);

#endif
