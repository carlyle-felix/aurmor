#ifndef MEMORY_H
#define MEMORY_H

#define NAME_LEN 100

// Console colours
#define RESET "\e[0m"
#define BOLD "\e[0;1m"
#define BRED "\033[1;31m"
#define BBLUE "\033[1;34m"
#define BGREEN "\e[1;32m"
#define GREY "\033[38;5;8m"

typedef struct node List;
typedef struct curl Json_buffer;

void str_malloc(char **ptr, int size);
List *list_malloc(void);
Json_buffer *buffer_malloc(void);  

#endif