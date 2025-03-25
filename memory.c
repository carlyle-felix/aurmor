#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *mem_malloc(char *var, int size) {
	
	char *temp;

	temp = malloc(size);
	if (temp == NULL) {
		printf("ERROR: Failed to allocate memory for %s", var);
		exit(EXIT_FAILURE);
	} else {
		temp[0] = '\0';
	}
	
	return temp;
}  

char *mem_realloc(char *p, char *var, int size) {

	char *temp;

	temp = realloc(p, size);
	if (temp == NULL) {
		printf("ERROR: Failed to allocate memory for %s", var);
		exit(EXIT_FAILURE);
	}
	
	return temp;
}