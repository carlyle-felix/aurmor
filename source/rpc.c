#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "../include/rpc.h"
#include "../include/memory.h"
#include "../include/list.h"
#include "../include/util.h"

size_t callback(char *data, size_t size, size_t nmemb, Json_buffer *p);
size_t write_meta(char *data, size_t size, size_t nmemb, FILE *p);

List *get_rpc_data(char *url) {

    Json_buffer *buffer;
    char *response;
    List *temp;

    buffer = json_buffer_malloc();
    curl(buffer, url);
    temp = json(buffer->response);
    
    free(buffer->response);
    free(buffer);

    return temp;
} 

char *curl(Json_buffer *buffer, char *url) {
    
    CURLcode res;
    CURL *curl;
    char *response = NULL;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    
    if(curl != NULL) {

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    return buffer->response;
}

size_t callback(char *data, size_t size, size_t nmemb, Json_buffer *p) {
    
    int len = size * nmemb;
    Json_buffer *temp = (Json_buffer *)p;

    str_malloc(&temp->response, temp->size + len + 1);
    memcpy(&(temp->response[temp->size]), data, len);
    temp->size += len;
    temp->response[temp->size] = '\0';

    return len;
}

List *json(char *response) {

    register int i, n_results;
    json_object *root, *results, *name, *pop, *version, *pkg, *desc;
    
    List *temp = list_malloc();
    
    root = json_tokener_parse(response);
    results = json_object_object_get(root, "results");
    n_results = json_object_array_length(results);

    for (i = 0; i < n_results; i++) {
        pkg = json_object_array_get_idx(results, i);
        
        name = json_object_object_get(pkg, "Name");
        version = json_object_object_get(pkg, "Version");
        pop = json_object_object_get(pkg, "Popularity");

        temp = add_json_data(temp, json_object_get_string(name), \
                            json_object_get_string(version), \
                            json_object_get_int(pop));
    }
    json_object_put(root);
    
    return temp;
}