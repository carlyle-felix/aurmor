#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/memory.h"

#define MAX_BUFFER 1000

void get_buffer(const char *cmd, char **buffer_ptr) {
	
	char *temp;
	FILE *p;	
	
	temp = mem_malloc(VSTR(temp), sizeof(char) * MAX_BUFFER);

	p = popen(cmd, "r");
	if (p == NULL) {
		printf("ERROR: failed in buffer().");
		exit(EXIT_FAILURE);
	}
	fgets(temp, MAX_BUFFER, p);
	
	*buffer_ptr = mem_malloc(VSTR(buffer_ptr), strlen(temp) + 1);
	strcpy(*buffer_ptr, temp);
	free(temp);

	pclose(p);
}