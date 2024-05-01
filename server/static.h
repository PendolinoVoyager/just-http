#ifndef STATIC_H
#define STATIC_H

#define CHUNK_SIZE  1024
#define STATIC_DIR  "/home/klaudiusz/Desktop/Projects/c_http/"

char    *preload_static(char *file);
int      read_static(char *file, char *buff);

#endif