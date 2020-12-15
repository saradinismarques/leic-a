#ifndef FS_H
#define FS_H
#include "state.h"
#include "sync.h"

void init_fs();
void destroy_fs();
int is_dir_empty(DirEntry *dirEntries);
int create(char *name, type nodeType);
int delete (char *name);
int lookup(char *name, int flag);
void print_tecnicofs_tree(FILE *fp);

#endif /* FS_H */
