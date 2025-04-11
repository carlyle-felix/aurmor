#ifndef MEMORY_H
#define MEMORY_H

#define NAME_LEN 100

typedef struct package List;
typedef struct curl Json_buffer;

void str_alloc(char **ptr, int size);
List *list_malloc(void);
void clear_list(List *list);
Json_buffer *json_buffer_malloc(void);  

#endif