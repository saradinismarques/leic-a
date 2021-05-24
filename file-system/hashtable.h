/*
 * File: paths.h
 * Author: Sara Marques
 * Number: 93342
 * Description: This file contains the constants and function prototypes related 
 * to the hashtables.
 */

#ifndef HASHTABLE
#define HASHTABLE

#include "structs.h"
#include "paths.h"

/* Maxium size of the hashtables. */
#define HT_SIZE 1290719

/*----------------------------- Initialization -----------------------------*/
void init_hashtables(PathHT **path_ht, ValueHT **value_ht, long int size);

/*----------------------------- Hash Function -----------------------------*/
long int hash(char* key, long int size);

/*-------------------------------- Creation --------------------------------*/
int hashtable_insert_path(PathHT **path_ht, PathNode *path_node, char *key, long int size);
int hashtable_insert_value(ValueHT **value_ht, PathNode *path_node, char *key, long int size);

/*--------------------------------- Search ---------------------------------*/
PathHT *hashtable_search_path(PathHT **path_ht, char *key, long int size);
ValueHT *hashtable_search_value(ValueHT **value_ht, char *key, char* path_name, long int size);

/*--------------------------------- Delete ---------------------------------*/
void hashtable_remove_path(PathHT **path_ht, PathHT *path_aux, long int pos);
void hashtable_remove_value(ValueHT **value_ht, ValueHT *path_aux, long int pos);

/*---------------------------------- Free ----------------------------------*/
void free_path(Path *path);
void free_path_node(PathNode *path_node);
void free_hashtables(PathHT **path_ht, ValueHT **value_ht, long int size);

#endif
