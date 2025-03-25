#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/memory.h"

#define SIZE 1000

void get_buffer(const char *cmd, char **buffer_ptr) {
	
	char temp[SIZE] = {'\0'};
	FILE *p;	
	
	p = popen(cmd, "r");
	if (p == NULL) {
		printf("ERROR: failed in buffer().");
		exit(EXIT_FAILURE);
	}
	fgets(temp, SIZE, p);
	
	*buffer_ptr = mem_malloc(VSTR(buffer_ptr), sizeof(temp));
	strcpy(*buffer_ptr, temp);
	
	pclose(p);
}