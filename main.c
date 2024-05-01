#include <asm-generic/socket.h>
#include <complex.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#include "server/request.h"
#include "server/static.h"
#include "common.h"

short g_op_mode = DEBUG;
static struct request *g_req;

char static req_buff[REQ_BUFF_SIZE];
char static res_buff[RES_BUFF_SIZE];
char static *html_index;
static int client_fd, sock_fd, received, res_size;
static struct sockaddr_storage client_sockaddr;
static socklen_t size = sizeof(struct sockaddr_storage);

void sigchld_handler(int s) {
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
/*
* All relevant socket options go here
*/
int create_sock_listener(unsigned short port) {
    struct addrinfo hints, *host_addr, *p;
    char service[5];
    int optval = 1;
    int status, sock_fd;
    sprintf(service, "%hu", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(NULL, service, &hints, &host_addr)) < 0) {
        return  -1;
    }

    struct sockaddr_in *my = (struct sockaddr_in *)(host_addr->ai_addr);
    my->sin_port = htons(port);

    for(p = host_addr; p != NULL; p = p->ai_next ) {
       if ((sock_fd = socket(p->ai_family, 
                    p->ai_socktype,
                p->ai_protocol)) < 0) {
                        perror("socket: creation fail...");
                        continue;
                    } 

        if (setsockopt(sock_fd, 
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        &optval,
                        sizeof(int)) < 0) {
            perror("setsockopt");
            continue;
        }

        if (bind(sock_fd, p->ai_addr, p->ai_addrlen) < 0) {
            perror("bind");
            continue;
        }
        break;
    }
    if (p == NULL) {
        perror("socket");
        return -1;
    }
 
    freeaddrinfo(host_addr);
    return sock_fd;
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// THIS BLOCK WILL GO AWAY FOR PROPER RES CONSTRUCTOR //
const char *def_header = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %d\r\n"
                        "\r\n"
                        "%s\r\n";

int attach_header(char *content, char *res_buff) {
    return sprintf(res_buff, def_header, strlen(content), content);
}
int send_response(int target_fd, char *buff, size_t size) {
    size_t bytes_sent = 0;
    while (bytes_sent < size) {
        ssize_t sent_bytes = send(target_fd, res_buff + bytes_sent, size - bytes_sent, 0);
        if (sent_bytes < 0) {
            perror("Error sending data");
            return -1;
        }
    bytes_sent += sent_bytes;
    }
  return bytes_sent;
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void start_main_loop() {
     while(1) {
        client_fd = accept(sock_fd, 
        (struct sockaddr *)&client_sockaddr,
    &size);
           
       if (client_fd < 0) {
            perror("Accept failed");
            continue; 
        } 
        if (fork() == 0) {
            // Step 1. Receive
            int received = recv(client_fd, req_buff, sizeof(req_buff), 0);
            if (g_op_mode == DEBUG) 
              write(STDOUT_FILENO, req_buff, received);
            // http request too large, ignore
            if (sizeof(received) == sizeof(req_buff) && req_buff[REQ_BUFF_SIZE - 1] != '\0')
                goto conn_end;
            // Step 2. Parse request
            // Important! Request is immutable for malloc reasons
            g_req = parse_request(client_fd, req_buff, 0, g_op_mode);
            if (g_req == NULL) {
                goto conn_end;
            }
            // Step 3. construct response

            attach_header(html_index, res_buff);

            // Step 4. away it goes!
            send_response(client_fd, res_buff, RES_BUFF_SIZE);

            conn_end:
            close(client_fd);
            close(sock_fd);
            exit(0);
        }
        close(client_fd);
    }

}
int main(int argc, char **argv) {

    struct sigaction sa;
    html_index = preload_static("index.html");
    if (html_index == NULL) {
        fprintf(stderr, "Cannot open index.html, exiting...\n");
        return -1;
    }
    if (argc < 2) {
        printf("Please provide a port to listen on.\n");
        return 1;
    }
    int port = atoi(argv[1]);

    sock_fd = create_sock_listener(port);

    if (sock_fd < 0) {
        fprintf(stderr, "No socket, no program. Exiting...\n");
        exit(1);
    }
    
    
    if (listen(sock_fd, CONN_BACKLOG) < 0 ) {
        fprintf(stderr, "Listen failed. Exiting...\n");
        exit(1);
    }

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("\x1b[0;32mListening for connections on TCP %d\x1b[0m\n", port);

    start_main_loop();

    close(sock_fd);
    return 0;
}
