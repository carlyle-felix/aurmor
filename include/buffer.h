#ifndef BUFFER_H
#define BUFFER_H

#define MAX_BUFFER 1000

typedef char *Buffer;

void get_buffer(const char *cmd, char **buffer_ptr);

#endif
