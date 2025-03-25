#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collect.h"
#include "memory.h"

#define SIZE 1000

void collect(const char *cmd, char **buffer_ptr) {
	
	char temp[SIZE] = {'\0'};
	FILE *p;	
	
	p = popen(cmd, "r");
	if (p == NULL) {
		printf("ERROR: failed in collect().");
		exit(EXIT_FAILURE);
	}
	fgets(temp, SIZE, p);
	
	*buffer_ptr = memory(NULL, "malloc", "buffer_ptr", sizeof(temp));
	strcpy(*buffer_ptr, temp);
	
	pclose(p);
}
