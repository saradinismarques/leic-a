/*
 * File: paths.h
 * Author: Sara Marques
 * Number: 93342
 * Description: This file contains the function prototypes related to the 
 * manipulation of paths in the system.
 */

#ifndef PATHS
#define PATHS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structs.h"
#include "constants.h"
#include "hashtable.h"

/* Exchanges two chars */
#define exch(A, B) {char* temp = A; A = B; B = temp;}

/*----------------------------- Initialization -----------------------------*/
void init_all(PathHT **path_ht, ValueHT **value_ht, long int size);

/*-------------------------------- Creation --------------------------------*/
Path *init_path(char *name, char *full_name, char *value);
PathNode *init_path_node(Path *path, Path *parent);
void insert_child(PathList *child_list, Path *child);
void insert_child_ordered(PathList *child_list, Path *child, PathNode *path_node);
int insert_path(char *name, char *full_name, char *value, char *parent_name, PathHT **path_ht, ValueHT **value_ht, long int size);
int create_path(char *path_name, char* value, PathHT **path_ht, ValueHT **value_ht, long int size);
int change_path_value(Path *path, char *value, ValueHT **value_ht, PathHT **path_ht, long int size);

/*--------------------------------- Search ---------------------------------*/
PathNode *find_path(char *path_name, PathHT **path_ht, long int size);
PathNode *find_value(char *value, ValueHT **value_ht, long int size);

/*--------------------------------- Print ---------------------------------*/
Path* print_path(Path *path, Path* child, PathHT **path_ht, long int size);
void list_paths_alphabetically(PathNode *path_node);

/*--------------------------------- Delete ---------------------------------*/
void remove_child(PathList *child_list, Path *child);
PathNode* delete_path(PathNode *path, PathHT **path_ht, ValueHT **value_ht, long int size);

/*---------------------------------- Free ----------------------------------*/
void free_all(PathHT **path_ht, ValueHT **value_ht, long int size);

/*--------------------------------- Auxiliary --------------------------------*/
char *strdup(char *string);
char* format_path(char *path_name);
char **copy_child_list(PathList *child_list, int size);
int compare_name(char *name_a, char *name_b);
void quicksort(char **to_order, int left, int right);

#endif
