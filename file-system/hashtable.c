/*
 * File: hashtable.c
 * Author: Sara Marques
 * Number: 93342
 * Description: This file contains all funtions related to the hashtables.
*/

#include "hashtable.h"

/*----------------------------- Initialization -----------------------------*/

/* Inicializes the two hashtables. Receives the hashtable by path name and
by value and their size. */
void init_hashtables(PathHT **path_ht, ValueHT **value_ht, long int size) {
    long int i;

    for(i = 0; i < size; i++) {
        path_ht[i] = NULL;
        value_ht[i] = NULL;
    }
}

/*----------------------------- Hash Function -----------------------------*/

/* Receives a key and a size and generates a number between 0 and the maxium 
size of the hashtable. Returns the generated number. */
long int hash(char* key, long int size) {
    long int hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = (((hash << 5) + hash) + c) % size; 
    }
    return hash;
}

/*-------------------------------- Creation --------------------------------*/

/* Inserts a path node in the hashtable by path name. Receives the path node, 
the key that is its name, the hashtable, and its size. */
int hashtable_insert_path(PathHT **path_ht, PathNode *path_node, char *key, 
                            long int size) {
    PathHT *new_path; 
    long int pos = hash(key, size);

    new_path = (PathHT*) malloc(sizeof(PathHT));
    
    if(new_path == NULL) 
        return ERROR;

    new_path->path_node = path_node;
    new_path->key = strdup(key);
    new_path->prev = NULL;
    new_path->next = path_ht[pos];

    if(path_ht[pos] != NULL)
        path_ht[pos]->prev = new_path;
    path_ht[pos] = new_path;

    return SUCCESS;
}

/* Inserts a path node in the hashtable by value if the value is not NULL. 
Receives the path node, the key that is its value, the hashtable, and its size. */
int hashtable_insert_value(ValueHT **value_ht, PathNode *path_node, char *key, 
                            long int size) {
    ValueHT *new_path;
    long int pos;

    if(key == NULL) 
        return SUCCESS;

    pos = hash(key, size);
    
    new_path = (ValueHT*) malloc(sizeof(ValueHT));

    if(new_path == NULL)
        return ERROR;

    new_path->path_node = path_node;
    new_path->key = strdup(key);
    new_path->prev = NULL;
    new_path->next = value_ht[pos];

    if(value_ht[pos] != NULL)
        value_ht[pos]->prev = new_path;
    value_ht[pos] = new_path;

    return SUCCESS;
}

/*--------------------------------- Search ---------------------------------*/

/* Searchs for path with a certain name as the key. Receives the key, the
hashtable by path, and its size. Returns the hashtable struct with that path
or NULL if not found. */
PathHT *hashtable_search_path(PathHT **path_ht, char *key, long int size) {
    PathHT *aux;
    long int pos = hash(key, size);

    aux = path_ht[pos];
    while(aux != NULL) {
        if(!strcmp(aux->key, key))
            return aux;
        aux = aux->next;
    }
    return NULL;
}

/* Searchs for path with a certain value as the key. Receives the key, the path name
the hashtable by value, and its size. Returns the hashtable struct with that path
or NULL if not found. */
ValueHT *hashtable_search_value(ValueHT **value_ht, char *key, char* full_name, 
                                long int size) {
    ValueHT *aux, *aux_value = NULL;
    long int pos = hash(key, size);
    aux = value_ht[pos];

    /* If path name is null searchs for the first element by creation order that has 
    that value, which means the last one in a certain position in the hashtable. */
    if(full_name == NULL) {
        while(aux != NULL) {
            if(!strcmp(aux->key, key))
                aux_value = aux;
            aux = aux->next;
        }
        return aux_value;
    }
     
    /* If path name is not null searchs for the path with that value and name. */
    while(aux != NULL) {
        if(!strcmp(aux->key, key) && !strcmp(aux->path_node->path->full_name, 
                                            full_name))  
            return aux;
        aux = aux->next;
    }
    return NULL;
}

/*--------------------------------- Delete ---------------------------------*/

/* Removes a path from the hashtable by path name. Receives the hastable struct 
with that path, the hashtable and its size.*/
void hashtable_remove_path(PathHT **path_ht, PathHT *path_aux, long int pos) {
    if(path_aux->next == NULL && path_aux->prev == NULL)
        path_ht[pos] = NULL;
    else if (path_aux->next == NULL){
        path_aux->prev->next = NULL;
    }
    else if (path_aux->prev == NULL){
        path_ht[pos] = path_aux->next;
        path_aux->next->prev = NULL;
    }
    else{
        path_aux->prev->next = path_aux->next;
        path_aux->next->prev = path_aux->prev;
    }
    free(path_aux->key);
    free(path_aux);
}

/* Removes a path from the hashtable by value. Receives the hastable struct 
with that path, the hashtable and its size.*/
void hashtable_remove_value(ValueHT **value_ht, ValueHT *path_aux, long int pos) {
    if(path_aux->next == NULL && path_aux->prev == NULL)
        value_ht[pos] = NULL;
    else if (path_aux->next == NULL){
        path_aux->prev->next = NULL;
    }
    else if (path_aux->prev == NULL){
        value_ht[pos] = path_aux->next;
        path_aux->next->prev = NULL;
    }
    else{
        path_aux->prev->next = path_aux->next;
        path_aux->next->prev = path_aux->prev;
    }
    free(path_aux->key);
    free(path_aux);
}

/*---------------------------------- Free ----------------------------------*/

/* Frees a path. */
void free_path(Path *path) {
    free(path->name);
    free(path->full_name);
    free(path->value);
    free(path);
}

/* Frees path node. */
void free_path_node(PathNode *path_node) {
    free_path(path_node->path);
    free(path_node->child_list);
    free(path_node);
}

/* Frees both hashtables. */
void free_hashtables(PathHT **path_ht, ValueHT **value_ht, long int size) {
    PathHT *path_aux;
    ValueHT *value_aux;
    long int i;

    for(i = 0; i < size; i++) {
        while(path_ht[i] != NULL) {
            path_aux = path_ht[i];
            path_ht[i] = path_ht[i]->next;
            free_path_node(path_aux->path_node);
            free(path_aux->key);
            free(path_aux);
        }
        while(value_ht[i] != NULL) {
            value_aux = value_ht[i];
            value_ht[i] = value_ht[i]->next;
            free(value_aux->key);
            free(value_aux);
        }
    }
    free(path_ht);
    free(value_ht);
}





