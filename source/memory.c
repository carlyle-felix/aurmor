#include <stdio.h>
#include <stdlib.h>

#include "../include/memory.h"
#include "../include/manager.h"
#include "../include/list.h"
#include "../include/rpc.h"
#include "../include/util.h"

void str_alloc(char **ptr, int size) {
	
	if (*ptr == NULL) {
		*ptr = malloc(size);
		if (*ptr == NULL) {
			printf(BRED"error:"RESET" Failed to allocate memory for string.\n");
			exit(EXIT_FAILURE);
		} else {
				*ptr[0] = '\0';
		}
	} else {
		char *temp;

		temp = realloc(*ptr, size);
		if (temp == NULL) {
			printf(BRED"error:"RESET" Failed to reallocate memory of string.\n");
			exit(EXIT_FAILURE);
		}
		*ptr = temp;
	}
}

List *list_malloc(void) {
	
	List *temp = malloc(sizeof(List));
	if (temp == NULL) {
		printf(BRED"error:"RESET" Failed to allocate memory for new node.\n");
		exit(EXIT_FAILURE);
	}

	temp->pkgname = NULL;
	temp->pkgver = NULL;
	temp->pop = 0;
	temp->install = false;
	temp->installed = false;
	temp->update = false;
	temp->next = NULL;
	
	return temp;
}

void clear_list(List *list) {
    
    List *temp;

    while (list != NULL) {
        temp = list;
        list = list->next;
        free(temp->pkgname);
        free(temp->pkgver);
        free(temp);
    }
}

Json_buffer *json_buffer_malloc(void) {

	Json_buffer *temp = malloc(sizeof(Json_buffer));
	if (temp == NULL) {
		printf(BRED"error:"RESET" Failed to allocate memory for JSON buffer.\n");
		exit(EXIT_FAILURE);
	}
	temp->response = NULL;
    str_alloc(&temp->response, sizeof(char *));
    temp->response[0] = '\0';
    temp->size = 0;

	return temp;
}

/*
* free func for alpm_list_fn_free
*/
void list_free(char *data) {
	
	if (data == NULL){
		return;
	}
	free(data);
}

Srcinfo *pkg_srcinfo_malloc(void) {

	Srcinfo *pkg = malloc(sizeof(Srcinfo));
	if (pkg == NULL) {
		printf(BRED"error:"RESET" failed to allocate memory for srcinfo list.\n");
		exit(EXIT_FAILURE);
	}
	pkg->pkgname = NULL;
	pkg->epoch = NULL;
	pkg->pkgver = NULL;
	pkg->pkgrel = NULL;
	pkg->zst_path = NULL;
	pkg->makedepends = NULL;
	pkg->depends = NULL;
	pkg->optdepends = NULL;

	return pkg;
}

void clear_pkg_srcinfo(Srcinfo *pkg) {

	free(pkg->pkgname);
	free(pkg->epoch);
	free(pkg->pkgver);
	free(pkg->pkgrel);
	free(pkg->zst_path);
	clear_depends(pkg->makedepends);
	clear_depends(pkg->depends);
	clear_depends(pkg->optdepends);
	free(pkg);
	
}

Depends *depends_malloc(void) {

	Depends *temp;

	temp = malloc(sizeof(Depends));
	if (temp == NULL) {
		printf(BRED"error:"RESET" failed to allocate storage for data_list\n");
		exit(EXIT_FAILURE);
	}
	temp->data = NULL;
	temp->next = NULL;

	return temp;
}



void clear_depends(Depends *list) {

	Depends *temp_list;

	while (list != NULL) {
		temp_list = list;
		list = list->next;
		free(temp_list->data);
		free(temp_list);
	}
}