#include <stdio.h>
#include <stdlib.h>

#include "../include/memory.h"
#include "../include/list.h"
#include "../include/rpc.h"
#include "../include/util.h"

void str_malloc(char **ptr, int size) {
	
	if (*ptr == NULL) {
		*ptr = malloc(size);
		if (*ptr == NULL) {
			printf(BRED"ERROR:"BOLD" Failed to allocate memory for string.\n"RESET);
			exit(EXIT_FAILURE);
		} else {
				*ptr[0] = '\0';
		}
	} else {
		char *temp;

		temp = realloc(*ptr, size);
		if (temp == NULL) {
			printf(BRED"ERROR:"BOLD" \e[0;1mFailed to reallocate memory of string.\n"RESET);
			exit(EXIT_FAILURE);
		}
		*ptr = temp;
	}
}

List *list_malloc(void) {

	List *temp = malloc(sizeof(List));
	if (temp == NULL) {
		printf(BRED"ERROR:"BOLD" Failed to allocate memory for new node.\n"RESET);
		exit(EXIT_FAILURE);
	}

	temp->pkgname = NULL;
	temp->pkgver = NULL;
	temp->pop = 0;
	temp->rpc_pkg = true;
	temp->update = false;
	temp->next = NULL;
	
	return temp;
}

Json_buffer *buffer_malloc(void) {

	Json_buffer *temp = malloc(sizeof(Json_buffer));
	if (temp == NULL) {
		printf(BRED"ERROR:"BOLD" Failed to allocate memory for JSON buffer.\n"RESET);
		exit(EXIT_FAILURE);
	}
	temp->response = NULL;
    str_malloc(&temp->response, sizeof(char *));
    temp->response[0] = '\0';
    temp->size = 0;

	return temp;
}