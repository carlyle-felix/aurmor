#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *memory(char *p, char *mem, char *var, int size) {
	
	char *temp;

	if (strcmp(mem, "malloc") == 0) {
		temp = malloc(size);
		if (temp == NULL) {
			printf("ERROR: Failed to allocate memory for %s", var);
			exit(EXIT_FAILURE);
		} else {
			temp[0] = '\0';
		}
	} else if (strcmp(mem, "realloc") == 0) {
		temp = realloc(p, size);
		if (temp == NULL) {
			printf("ERROR: Failed to allocate memory for %s", var);
			exit(EXIT_FAILURE);
		}
	} else {
		printf("ERROR: Invalid call to memory()");
	}
	
	return temp;
}  
