#ifndef RPC_H
#define RPC_H

typedef struct {
    char *response;
    int size;
} Json_buffer;

typedef struct node List;

Json_buffer *buffer_malloc(void);  // in memory.c
char *curl(char *url);
void print_search(char *pkgname);
List *get_rpc_data(char *url);
List *json(char *buffer); 

#endif