#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/memory.h"

void get_buffer(const char *cmd, char **buffer_ptr) {
	
	char *temp = NULL;
	FILE *p;	
	
	mem_alloc(&temp, VSTR(temp), sizeof(char) * MAX_BUFFER);

	p = popen(cmd, "r");
	if (p == NULL) {
		printf("ERROR: failed in buffer().");
		exit(EXIT_FAILURE);
	}
	fgets(temp, MAX_BUFFER, p);
	
	mem_alloc(buffer_ptr, VSTR(buffer_ptr), sizeof(char) * (strlen(temp) + 1));
	strcpy(*buffer_ptr, temp);
	free(temp);

	pclose(p);
}