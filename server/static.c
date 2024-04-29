#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *preload_static(char *file) {
    FILE *st_file = fopen(file, "r");
    if (st_file == NULL) 
        return NULL; 
    
    fseek(st_file, 0L, SEEK_END);
    long size = ftell(st_file); // Get the file size
    fseek(st_file, 0L, SEEK_SET); // Reset file pointer to the beginning
    
    char *ret = malloc((size + 1) * sizeof(char)); // Allocate memory for content (+1 for null terminator)
    if (ret == NULL) {
        fclose(st_file);
        return NULL; // Memory allocation failed
    }

    size_t bytes_read = fread(ret, 1, size, st_file);
    if (bytes_read != size) {
        fclose(st_file);
        free(ret);
        return NULL; // Error reading file
    }
    ret[size] = '\0'; // Null-terminate the string

    fclose(st_file);
    return ret;
}

char *dbg_large_res() {
    char *large = (char *)malloc(10000);
    char *ptr = large;
    for(int i = 0; i < 10000; i++) {
        *ptr++ = 'A';
    }
    return large;
}