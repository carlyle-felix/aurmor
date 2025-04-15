#ifndef MEMORY_H
#define MEMORY_H

#define NAME_LEN 100
#define MAX_BUFFER 1024

typedef struct package List;
typedef struct curl Json_buffer;
typedef struct srcinfo Srcinfo;
typedef struct depends Depends;

void str_alloc(char **ptr, int size);
List *list_malloc(void);
void clear_list(List *list);
Json_buffer *json_buffer_malloc(void); 
void list_free(char *data);		// for alpm_list_fn_free 
Srcinfo *pkg_srcinfo_malloc(void);
void clear_pkg_srcinfo(Srcinfo *pkg);
Depends *depends_malloc(void);
void clear_depends(Depends *list);


#endif