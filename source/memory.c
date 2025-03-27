#include <stdio.h>
#include "../include/memory.h"
#include "../include/list.h"

void mem_alloc(char **ptr, const char *var, int size) {
	
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

List *struct_malloc(void) {

	List *temp = malloc(sizeof(List));
	if (temp == NULL) {
		printf("ERROR: Failed to allocate memory for new node.");
		exit(EXIT_FAILURE);
	}

	temp->pkgname = NULL;
	temp->pkgver = NULL;
	temp->next = NULL;
	temp->update = false;
	
	return temp;
}