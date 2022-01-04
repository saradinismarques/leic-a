/*
 * File: paths.c
 * Author: Sara Marques
 * Number: 93342
 * Description: This file contains all funtions related to the manipulation
 * of paths in the system.
*/

#include "paths.h"

/*----------------------------- Initialization -----------------------------*/

/* Inicializes the two hashtables. Receives the hashtable by path name and
by value and their size. */
void init_all(PathHT **path_ht, ValueHT **value_ht, long int size) {
    init_hashtables(path_ht, value_ht, size);
}

/*-------------------------------- Creation --------------------------------*/

/* Inicializes a path. Receives the path name, full name and value. Returns
the new path or NULL if error. */
Path *init_path(char *name, char *full_name, char *value) {
    Path *new_path = (Path*) malloc(sizeof(Path));

    new_path->name = strdup(name);
    new_path->full_name = strdup(full_name);
    new_path->value = strdup(value);

    new_path->next = NULL;
    new_path->prev = NULL;

    if(new_path->name == NULL || new_path->full_name == NULL)
        return NULL;

    return new_path;
} 

/* Inicializes a path node. Receives the path and its parent. Returns the new
path node or NULL if error. */
PathNode *init_path_node(Path *path, Path *parent) {
    PathNode *new_path_node = (PathNode *) malloc(sizeof(PathNode));

    new_path_node->path = path;
    new_path_node->parent = parent;
    new_path_node->n_child = 0;

    new_path_node->child_list = (PathList*) malloc(sizeof(PathList));
    new_path_node->child_list->first = NULL;
    new_path_node->child_list->last = NULL;

    if(new_path_node->child_list == NULL)
        return NULL;

    return new_path_node;
}

/* Inserts a child in the child list of a path. Receives the child list and the
child to insert. */
void insert_child(PathList *child_list, Path *child) {
    if(child_list->first == NULL) {
        child_list->first = child;
    } else {
        child->prev = child_list->last;
        child_list->last->next = child;
    }
    child_list->last = child;
}

/* Inserts a path node in the hashtables after its inicialization. Receives the 
path name, full name, value, parent name, the two hashtables and their size. */
int insert_path(char *name, char *full_name, char *value, char *parent_name, 
                PathHT **path_ht, ValueHT **value_ht, long int size) {
    PathNode *path_node, *parent;
    Path *path = init_path(name, full_name, value);

    if(path == NULL)
        return ERROR;

    /* Inicializes the root path. */
    if(parent_name == NULL) {
        parent = NULL;
        path_node = init_path_node(path, NULL);

        if(path_node == NULL)
            return ERROR;
    }
    /* Inicializes other paths. */
    else {
        parent = hashtable_search_path(path_ht, parent_name, size)->path_node;
        path_node = init_path_node(path, parent->path);

        if(path_node == NULL)
            return ERROR;

        insert_child(parent->child_list, path);
        parent->n_child++;
    }

    /* Inserts in both hashtables. */
    if(!hashtable_insert_path(path_ht, path_node, path_node->path->full_name, size))
        return ERROR;
    if(!hashtable_insert_value(value_ht, path_node, path_node->path->value, size)) 
        return ERROR;

    return SUCCESS;
}

/* Creates a path by breaking apart each component of a path name and checking if 
every path hierarchically superior is already created and if not create it with 
NULL value. Receives the path name, value, two hashtables and their size. */
int create_path(char *path_name, char* value, PathHT **path_ht, ValueHT **value_ht, 
                long int size) {
    char *parent_name, *child_name = ROOT, *path_aux, *path_token;
    char *full_name = format_path(path_name);
    char *value_aux;

    path_aux = malloc(strlen(path_name) + 2);
    strcpy(path_aux, "");
    path_token = strtok(path_name, SLASH);

    /* Creates root path. */
    if(path_token == NULL) {
        if(hashtable_search_path(path_ht, ROOT, size) == NULL) {
            free(full_name);
            free(path_aux);
            if(!insert_path(ROOT, ROOT, NULL, NULL, path_ht, value_ht, size)) 
                return ERROR;
            return SUCCESS;
        }
    }
    parent_name = strdup(child_name);

    /* Loop that breaks apart every component in a path name saving the parent 
    name and child name at each iteration. */
    while(path_token != NULL) {
        strcat(path_aux, SLASH);
        strcat(path_aux, path_token);
        child_name = path_aux;

        if(hashtable_search_path(path_ht, path_aux, size) == NULL) {
            if(!strcmp(child_name, full_name))
                value_aux = value;
            else 
                value_aux = NULL;

            if(!insert_path(path_token, child_name, value_aux, parent_name, 
                            path_ht, value_ht, size)) {
                free(full_name);
                free(parent_name);
                free(path_aux);
                return ERROR;
            }
        }
        free(parent_name);
        parent_name = strdup(child_name);
        path_token = strtok(NULL, SLASH);
    }
    free(full_name);
    free(parent_name);
    free(path_aux);
    return SUCCESS;
}

/* Changes the value of a path and inserts it in the hashtable by value if its
previous value was NULL or deletes it first and inserts again in a new position
if not. Receives the path, value, two hashtables and their size. */
int change_path_value(Path *path, char *value, ValueHT **value_ht, PathHT **path_ht, 
                    long int size) {
    ValueHT *aux;
    PathNode *path_node = hashtable_search_path(path_ht, path->full_name, 
                                                size)->path_node;
    long int pos;
    
    if(path->value == NULL) {
        path->value = strdup(value);
        if(!hashtable_insert_value(value_ht, path_node, value, size))
            return ERROR;
    } else {
        aux = hashtable_search_value(value_ht, path->value, path->full_name, size);
        pos = hash(aux->key, size);
        hashtable_remove_value(value_ht, aux, pos);
        if(!hashtable_insert_value(value_ht, path_node, value, size)) 
            return ERROR;
    }
    free(path->value);
    path->value = strdup(value);
    return SUCCESS;
}

/*--------------------------------- Search ---------------------------------*/

/* Finds a path with a certain name. Receives the path name, the hashtable by 
path, and its size. Returns the path node or NULL if not found. */
PathNode *find_path(char *path_name, PathHT **path_ht, long int size) {
    PathHT *aux;
    char *full_name = format_path(path_name);

    aux = hashtable_search_path(path_ht, full_name, size);

    if(aux == NULL) {
        free(full_name);
        return NULL;
    }
    free(full_name);
    return aux->path_node;
}

/* Finds a path with a certain value. Receives the value, the hashtable by 
value, and its size. Returns the path node or NULL if not found. */
PathNode *find_value(char *value, ValueHT **value_ht, long int size) {
    ValueHT *aux;

    aux = hashtable_search_value(value_ht, value, NULL, size);

    if(aux == NULL) 
        return NULL;

    return aux->path_node;
}

/*--------------------------------- Print ---------------------------------*/

/* Prints every path and value in depth by creation order of each component if 
the value is not NULL. Receives the starting path, the first child, the hashtable
by path name and its size. Returns recursively a new path to print. */
Path* print_path(Path *path, Path* child, PathHT **path_ht, long int size) {
    PathNode *path_node = hashtable_search_path(path_ht, path->full_name, 
                                                size)->path_node, 
            *parent, *child_aux;
    
    /* If path does not have more children. */
    if(child == NULL) {
        if(!strcmp(path->full_name, ROOT)) 
            return NULL;

        /* Returns the next path in the list. */
        else if(path->next != NULL) 
            return path->next;

        else {
            if(!strcmp(path_node->parent->full_name, ROOT)) 
                return NULL;
            
            parent = hashtable_search_path(path_ht, path_node->parent->full_name, 
                                            size)->path_node;
            /* Loop to find a parent that still has paths next to it. */
            while(parent->path->next == NULL) {
                parent = hashtable_search_path(path_ht, parent->parent->full_name, 
                                                size)->path_node;
                if(!strcmp(parent->path->full_name, ROOT))
                    return NULL;
            }
            return parent->path->next;
        }
    } else {
        if(child->value != NULL)
            printf("%s %s\n", child->full_name, child->value);
        child_aux = hashtable_search_path(path_ht, child->full_name, 
                                            size)->path_node;
        /* Recursively calls this function until a path does not have more children. */
        child = print_path(child_aux->path, child_aux->child_list->first, 
                            path_ht, size);
        if(child == NULL)
            return NULL; 
        parent = hashtable_search_path(path_ht, child_aux->parent->full_name, 
                                        size)->path_node;
        /* Recursively calls this function now with the parent of the path. */
        return print_path(parent->path, child, path_ht, size);
    }
}

/* Lists every direct child of a path alphabetically ordered. Receives the 
path node. */
void list_paths_alphabetically(PathNode *path_node) {
    PathList* child_list = path_node->child_list;
    char **list_sorted = NULL;
    int i;
 
    if(path_node->n_child == 0)
        return;

    list_sorted = copy_child_list(child_list, path_node->n_child);

    quicksort(list_sorted, 0, path_node->n_child - 1);

    for(i = 0; i < path_node->n_child; i++) {
        printf("%s\n", list_sorted[i]);
        free(list_sorted[i]);
    }

    free(list_sorted);
}

/*--------------------------------- Delete ---------------------------------*/

/* Removes a child in the child list of a path. Receives the child list and the
child to remove. */
void remove_child(PathList *child_list, Path *child) {
    if(child->next == NULL && child->prev == NULL) {
        child_list->first = NULL;
    } else if(child->next == NULL) {
        child->prev->next = NULL;
        child_list->last = child->prev;
    } else if(child->prev == NULL) {
        child_list->first = child->next;
        child->next->prev = NULL;
    } else {
        child->prev->next = child->next;
        child->next->prev = child->prev;
    }
}

/* Deletes every path of a sub path. Receives the path node to delete, the two 
hashtabels, and their size. Returns recursively a new path to delete.*/
PathNode* delete_path(PathNode *path_node, PathHT **path_ht, ValueHT **value_ht, 
                        long int size) {
    long int i = hash(path_node->path->full_name, size), j;
    PathHT *path_aux = hashtable_search_path(path_ht, path_node->path->full_name, 
                                            size);
    ValueHT *value_aux;
    PathNode *parent = NULL, *child = NULL, *aux = path_aux->path_node;

    /* If path does not have more children. */
    if(path_node->child_list->first == NULL) {
        if(path_node->parent == NULL)
            return NULL;
            
        /* Removes from child list. */
        parent = hashtable_search_path(path_ht, path_node->parent->full_name, 
                                            size)->path_node;
        remove_child(parent->child_list, path_node->path);
        parent->n_child--;

        /* Removes path from the hashtables. */
        if(path_node->path->value != NULL && strcmp(path_node->path->full_name, ROOT)) {
            value_aux = hashtable_search_value(value_ht, path_node->path->value, 
                                                path_node->path->full_name, size);
            j = hash(path_node->path->value, size);
            hashtable_remove_value(value_ht, value_aux, j);
        }
        if(strcmp(path_node->path->full_name, ROOT)) {
            hashtable_remove_path(path_ht, path_aux, i);
            free_path_node(aux);
        }
        return parent;

    } else {
        child = hashtable_search_path(path_ht, path_node->child_list->first->full_name, 
                                        size)->path_node;
        /* Recursively calls this function until a path does not have more childre. */
        parent = delete_path(child, path_ht, value_ht, size);
        if(parent == NULL)
            return NULL;
        /* Recursively calls this function now with the parent of the path. */
        return delete_path(parent, path_ht, value_ht, size);
    } 
}

/*---------------------------------- Free ----------------------------------*/

/* Frees all the memory associated with the two hashtables. Receives the 
hashtable by path name and by value and their size. */
void free_all(PathHT **path_ht, ValueHT **value_ht, long int size) {
    free_hashtables(path_ht, value_ht, size);
}

/*--------------------------------- Auxiliary --------------------------------*/

/* Duplicates a given string by allocating memory and then copying it. Receives
a string. Returns the copy. */
char *strdup(char *string) {
    char *copy;
    if(string == NULL)
        return NULL;

    copy = (char *) malloc(strlen(string) + 1);

    if(string == NULL)
        return NULL;

    strcpy(copy, string);
    return copy;
}

/* Formats a given path name putting and slash in the beginning and removing
other slashes in excess. Receives the path name. Returns the path formatted. */
char* format_path(char *path_name) {
    char *full_name, *path_token, *path_name_copy = strdup(path_name);

    if(path_name_copy == NULL || !strcmp(path_name_copy, SLASH)) {
        full_name = malloc(strlen(SLASH) + 1);
        strcpy(full_name, SLASH);
        free(path_name_copy);
        return full_name;
    }

    full_name = malloc(strlen(path_name_copy) + 2);
    path_token = strtok(path_name_copy, SLASH);

    strcpy(full_name, "");

    while(path_token != NULL) {
        strcat(full_name, SLASH);
        strcat(full_name, path_token);
        path_token = strtok(NULL, SLASH);
    }
    free(path_name_copy);
    return full_name;
}

/* Copies the child list of a path. Receives the child list and its size. Returns
the copy of the list. */
char **copy_child_list(PathList *child_list, int size) {
    char **list_copy = (char**) malloc(size * sizeof(char**));
    Path *child = child_list->first;
    int i = 0;

    while(child != NULL) {
        list_copy[i] = strdup(child->name);
        child = child->next;
        i++;
    }
    return list_copy;
}

/* Compares two names alphabetically with upper cases first. Receives two names.
Returns 1 if first name is bigger and 0 otherwise. */
int compare_name(char *name_a, char *name_b) {

    if(*name_a >= 'A' && *name_a <= 'Z' && *name_b >= 'a' && *name_b <= 'z') 
        return 1;
    else if(*name_b <= 'A' && *name_b <= 'Z' && *name_a >= 'a' && *name_a <= 'z')
        return 0;
    else
        return strcmp(name_a, name_b) <= 0 ? 1 : 0;
}

/* Quicksort algorithm. Sorts array of strings. Receives the array of strings
to sort and the left and right indexes in the array. */
void quicksort(char **to_order, int left, int right) {
    int i, j, pivot;

    if(left < right) {
        
        /* Chooses left element for pivot. */
        pivot = left;
        i = left;
        j = right;

        while(i < j) {
            while(compare_name(to_order[i], to_order[pivot]) && i < right) 
                i++;

            while(!compare_name(to_order[j], to_order[pivot])) 
                j--;
                
            if(i < j) 
                exch(to_order[i], to_order[j]);
        }
        exch(to_order[pivot], to_order[j]);

        /* Recursive call of the function. */
        quicksort(to_order, left, j - 1);
        quicksort(to_order, j + 1, right);
    }
}