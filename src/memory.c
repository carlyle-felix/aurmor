#include <stdio.h>
#include "../include/memory.h"

char mem_alloc(char **ptr, char *var, int size) {
	
	if (*ptr == NULL) {
		*ptr = malloc(size);
		if (*ptr == NULL) {
			printf("ERROR: Failed to allocate memory for %s", var);
			exit(EXIT_FAILURE);
		} else {
			*ptr[0] = '\0';
		}
	} else {
		char *temp;

		temp = realloc(*ptr, size);
		if (temp == NULL) {
			printf("ERROR: Failed to allocate memory for %s", var);
			exit(EXIT_FAILURE);
		}
		*ptr = temp;
	}
}