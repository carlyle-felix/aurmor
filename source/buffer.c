#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/buffer.h"
#include "../include/memory.h"

void get_buffer(const char *cmd, Buffer *buffer_ptr) {
	
	char *temp = NULL;
	FILE *p;	
	
	str_malloc(&temp, MAX_BUFFER);

	p = popen(cmd, "r");
	if (p == NULL) {
		printf(BRED"ERROR:"BOLD" Failed in buffer().\n"RESET);
		exit(EXIT_FAILURE);
	}
	fgets(temp, MAX_BUFFER, p);
	str_malloc(buffer_ptr, (strlen(temp) + 1));
	strcpy(*buffer_ptr, temp);
	free(temp);

	pclose(p);
}

void get_str(char **cmd, const char *str, const char *str_var) {
	
	if (str_var != NULL) {
		str_malloc(cmd, strlen(str) + strlen(str_var) - 1);
		sprintf(*cmd, str, str_var);
	} else {
		str_malloc(cmd, strlen(str) + 1);
		sprintf(*cmd, str);
	}
}