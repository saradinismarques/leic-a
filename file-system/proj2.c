/*
 * File: proj2.c
 * Author: Sara Marques
 * Number: 93342
 * Description: File system implementation.
*/

#include "paths.h"

/*---------------------------- Function Prtotypes ---------------------------*/
void help();
int set(char *path_name, char* value, PathHT **path_ht, ValueHT **value_ht, long int size);
void print(PathHT **path_ht, long int size);
void find(char *path_name, PathHT **path_ht, long int size);
void list(char *path_name, PathHT **path_ht, long int size);
void search(char *value, ValueHT **value_ht, long int size);
void delete(char *path_name, PathHT **path_ht, ValueHT **value_ht, long int size);

/*------------------------------ Main Function -------------------------------*/

/* Creates and initializes the hashtables by path name and by value. Starts by 
inserting the root path. Reads the command given in a loop and executes the 
corresponding function until it receives the command 'quit'. After that frees
all the memory used and returns 0 if no problem has occured. */
int main() {
    char buffer[MAX_BUFFER];
    char *cmd, *path_name, *value;
    long int size_ht = HT_SIZE;
    PathHT **path_ht = (PathHT **) malloc(size_ht * sizeof(PathHT));
    ValueHT **value_ht = (ValueHT **) malloc(size_ht * sizeof(ValueHT));

    if(path_ht == NULL || value_ht == NULL) {
        printf("%s\n", NO_MEMORY);
        return 0;
    }

    init_all(path_ht, value_ht, size_ht);

    if(!set(ROOT, NULL, path_ht, value_ht, size_ht)) {
        free_all(path_ht, value_ht, size_ht);
        return 0;
    }

    fgets(buffer, sizeof(buffer), stdin);
    cmd = strtok(buffer, CMD_DELIM);

    while(strcmp(cmd, QUIT)) {

        if(!strcmp(cmd, HELP)) {
            help();

        } else if(!strcmp(cmd, SET)) {
            path_name = strtok(NULL, PATH_DELIM);
            value = strtok(NULL, VALUE_DELIM);
            if(!set(path_name, value, path_ht, value_ht, size_ht))
                break;
        } else if(!strcmp(cmd, PRINT)) {
            print(path_ht, size_ht);

        } else if(!strcmp(cmd, FIND)) {
            path_name = strtok(NULL, PATH_DELIM);
            find(path_name, path_ht, size_ht);

        } else if(!strcmp(cmd, LIST)) {
            path_name = strtok(NULL, PATH_DELIM);
            list(path_name, path_ht, size_ht);

        } else if(!strcmp(cmd, SEARCH)) {
            value = strtok(NULL, VALUE_DELIM);
            search(value, value_ht, size_ht);

        } else if(!strcmp(cmd, DELETE)) {
            path_name = strtok(NULL, PATH_DELIM);
            delete(path_name, path_ht, value_ht, size_ht);
        }
        fgets(buffer, sizeof(buffer), stdin);
        cmd = strtok(buffer, CMD_DELIM);
    }
    free_all(path_ht, value_ht, size_ht);
    return 0;
}

/*----------------------------- Command Funtions -----------------------------*/

/* Prints all the available commands. */
void help() {
    printf("%s: %s\n", HELP, HELP_DESCRIPTION);
    printf("%s: %s\n", QUIT, QUIT_DESCRIPTION);
    printf("%s: %s\n", SET, SET_DESCRIPTION);
    printf("%s: %s\n", PRINT, PRINT_DESCRIPTION);
    printf("%s: %s\n", FIND, FIND_DESCRIPTION);
    printf("%s: %s\n", LIST, LIST_DESCRIPTION);
    printf("%s: %s\n", SEARCH, SEARCH_DESCRIPTION);
    printf("%s: %s\n", DELETE, DELETE_DESCRIPTION);
}

/* Adds a new path to the system or changes the value of an existing path. 
Receives the path name, value, the two hashtables and their size. */
int set(char *path_name, char* value, PathHT **path_ht, ValueHT **value_ht, long int size) {
    PathNode *path_node = find_path(path_name, path_ht, size);

    if(path_node == NULL) { 
        if(!create_path(path_name, value, path_ht, value_ht, size)) {
            printf("%s\n", NO_MEMORY);
            return ERROR;
        }
    } else {
        if(!change_path_value(path_node->path, value, value_ht, path_ht, size)) {
            printf("%s\n", NO_MEMORY);
            return ERROR;
        }
    } 
    return SUCCESS;
}

/* Prints every path in the system with its corresponding value. Receives the
hashtable by path name and its size. */
void print(PathHT **path_ht, long int size) {
    PathNode *path_node = find_path(ROOT, path_ht, size);
    print_path(path_node->path, path_node->child_list->first, path_ht, size);
}

/* Finds a path in the system and if it exists prints its value if it has one. 
Receives the path name and the hashtable by path name and its size. */
void find(char *path_name, PathHT **path_ht, long int size) {
    PathNode *path_node = find_path(path_name, path_ht, size);

    if(path_node == NULL) {
        printf("%s\n", NOT_FOUND);
        return;
    }
    if(path_node->path->value == NULL) {
        printf("%s\n", NO_DATA);
        return;
    }
    printf("%s\n", path_node->path->value);
}

/* Lists all the immediate components of a sub path if it exists. Receives the
path name, the hashtable by path name and its size. */
void list(char *path_name, PathHT **path_ht, long int size) {
    PathNode *path_node = find_path(path_name, path_ht, size);

    if(path_node == NULL) {
        printf("%s\n", NOT_FOUND);
        return;
    }
    list_paths_alphabetically(path_node);
}

/* Searchs for a path with certain value and prints it if exists. Receives the
value, the hashtable by value and its size. */
void search(char *value, ValueHT **value_ht, long int size) {
    PathNode* path_node = find_value(value, value_ht, size);

    if(path_node == NULL) {
        printf("%s\n", NOT_FOUND);
        return;
    }
    printf("%s\n", path_node->path->full_name);
}

/* Deletes every path of a sub path if it exists. Receives the path name, the
two hashtables and their size. */
void delete(char *path_name, PathHT **path_ht, ValueHT **value_ht, long int size) {
    PathNode *path_node = find_path(path_name, path_ht, size);

    if(path_node == NULL) {
        printf("%s\n", NOT_FOUND);
        return;
    }
    delete_path(path_node, path_ht, value_ht, size);
} 