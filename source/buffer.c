#include <stdio.h>
#include <stdlib.h>
#include "../include/buffer.h"
#include "../include/memory.h"

void retrieve(const char *cmd, char **buffer_ptr) {
	
	char *temp = NULL;
	FILE *p;	
	
	mem_alloc(&temp, MAX_BUFFER);

	p = popen(cmd, "r");
	if (p == NULL) {
		printf("ERROR: failed in buffer().");
		exit(EXIT_FAILURE);
	}
	fgets(temp, MAX_BUFFER, p);
	mem_alloc(buffer_ptr, (strlen(temp) + 1));
	strcpy(*buffer_ptr, temp);
	free(temp);

	pclose(p);
}

void get_cmd(char **cmd, const char *str, const char *str_var) {
	
	if (str_var != NULL) {
		mem_alloc(cmd, strlen(str) + strlen(str_var) - 1);
		sprintf(*cmd, str, str_var);
	} else {
		mem_alloc(cmd, strlen(str) + 1);
		sprintf(*cmd, str);
	}
}