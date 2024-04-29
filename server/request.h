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
};

#define WRAP_HTML_DBG(content, title) "<!DOCTYPE html><html><head><title>"#title"</title></head><body>"#content"</body></html>\n"
struct request* parse_request(int client_fd, char *req_str, int mode);
int is_valid_status_line(char *req_buff);

// int send_response(int target_fd, char *buff, size_t size);
#endif
