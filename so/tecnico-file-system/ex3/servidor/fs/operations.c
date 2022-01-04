#include "operations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
pthread_mutex_t mutexlock;
pthread_rwlock_t rwlock;
extern inode_t inode_table[INODE_TABLE_SIZE];

/* Given a path, fills pointers with strings for the parent path and child
 * file name
 * Input:
 *  - path: the path to split. ATENTION: the function may alter this parameter
 *  - parent: reference to a char*, to store parent path
 *  - child: reference to a char*, to store child file name
 */
void split_parent_child_from_path(char *path, char **parent, char **child)
{

	int n_slashes = 0, last_slash_location = 0;
	int len = strlen(path);

	// deal with trailing slash ( a/x vs a/x/ )
	if (path[len - 1] == '/')
	{
		path[len - 1] = '\0';
	}

	for (int i = 0; i < len; ++i)
	{
		if (path[i] == '/' && path[i + 1] != '\0')
		{
			last_slash_location = i;
			n_slashes++;
		}
	}

	if (n_slashes == 0)
	{ // root directory
		*parent = "";
		*child = path;
		return;
	}

	path[last_slash_location] = '\0';
	*parent = path;
	*child = path + last_slash_location + 1;
}

/*
 * Initializes tecnicofs and creates root node.
 */
void init_fs()
{
	inode_table_init();

	/* create root inode */
	int root = inode_create(T_DIRECTORY);
	lock_unlock(&inode_table[root].rwlock);
	//printf("unlock-%d\n", root);
	for (int i = 0; i < INODE_TABLE_SIZE; i++)
	{
		lock_init(&inode_table[i].rwlock);
		inode_table[i].lock_counter = 0;
	}

	if (root != FS_ROOT)
	{
		printf("failed to create node for tecnicofs root\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * Destroy tecnicofs and inode table.
 */
void destroy_fs()
{
	for (int i = 0; i < INODE_TABLE_SIZE; i++)
	{
		lock_destroy(&inode_table[i].rwlock);
	}
	inode_table_destroy();
}

/*
 * Checks if content of directory is not empty.
 * Input:
 *  - entries: entries of directory
 * Returns: SUCCESS or FAIL
 */

int is_dir_empty(DirEntry *dirEntries)
{
	if (dirEntries == NULL)
	{
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++)
	{
		if (dirEntries[i].inumber != FREE_INODE)
		{
			return FAIL;
		}
	}
	return SUCCESS;
}

void unlock_all(int *unlock_array)
{
	for (int i = 0; i < INODE_TABLE_SIZE; i++)
	{
		if (unlock_array[i] == 1)
		{
			lock_unlock(&inode_table[i].rwlock);
		}
	}
}

/*
 * Looks for node in directory entry from name.
 * Input:
 *  - name: path of node
 *  - entries: entries of directory
 * Returns:
 *  - inumber: found node's inumber
 *  - FAIL: if not found
 */
int lookup_sub_node(char *name, DirEntry *entries)
{
	if (entries == NULL)
	{
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++)
	{
		if (entries[i].inumber != FREE_INODE && strcmp(entries[i].name, name) == 0)
		{
			return entries[i].inumber;
		}
	}
	return FAIL;
}

/*
 * Creates a new node given a path.
 * Input:
 *  - name: path of node
 *  - nodeType: type of node
 * Returns: SUCCESS or FAIL
 */
int create(char *name, type nodeType)
{

	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType;
	union Data pdata;
	int unlock_array[INODE_TABLE_SIZE];

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);

	for (int i = 0; i < INODE_TABLE_SIZE; i++)
		unlock_array[i] = 0;

	parent_inumber = lookup(parent_name, unlock_array, 1);

	if (parent_inumber == FAIL)
	{
		printf("failed to create %s, invalid parent dir %s\n",
			   name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	inode_get(parent_inumber, &pType, &pdata);

	if (pType != T_DIRECTORY)
	{
		printf("failed to create %s, parent %s is not a dir\n",
			   name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	if (lookup_sub_node(child_name, pdata.dirEntries) != FAIL)
	{
		printf("failed to create %s, already exists in dir %s\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	/* create node and add entry to folder that contains new node */
	child_inumber = inode_create(nodeType);

	if (child_inumber == FAIL)
	{
		printf("failed to create %s in  %s, couldn't allocate inode\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}
	unlock_array[child_inumber] = 1;

	if (dir_add_entry(parent_inumber, child_inumber, child_name) == FAIL)
	{
		printf("could not add entry %s in dir %s\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}
	unlock_all(unlock_array);

	return SUCCESS;
}

/*
 * Deletes a node given a path.
 * Input:
 *  - name: path of node
 * Returns: SUCCESS or FAIL
 */
int delete (char *name)
{
	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType, cType;
	union Data pdata, cdata;
	int unlock_array[INODE_TABLE_SIZE];

	for (int i = 0; i < INODE_TABLE_SIZE; i++)
		unlock_array[i] = 0;

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);

	parent_inumber = lookup(parent_name, unlock_array, 1);

	if (parent_inumber == FAIL)
	{
		printf("failed to delete %s, invalid parent dir %s\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	inode_get(parent_inumber, &pType, &pdata);

	if (pType != T_DIRECTORY)
	{
		printf("failed to delete %s, parent %s is not a dir\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	child_inumber = lookup_sub_node(child_name, pdata.dirEntries);

	if (child_inumber == FAIL)
	{
		printf("could not delete %s, does not exist in dir %s\n",
			   name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	//printf("lw-%d\n", child_inumber);
	lock_wrlock(&inode_table[child_inumber].rwlock);
	unlock_array[child_inumber] = 1;

	inode_get(child_inumber, &cType, &cdata);

	if (cType == T_DIRECTORY && is_dir_empty(cdata.dirEntries) == FAIL)
	{
		printf("could not delete %s: is a directory and not empty\n",
			   name);
		unlock_all(unlock_array);
		return FAIL;
	}

	/* remove entry from folder that contained deleted node */
	if (dir_reset_entry(parent_inumber, child_inumber) == FAIL)
	{
		printf("failed to delete %s from dir %s\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	if (inode_delete(child_inumber) == FAIL)
	{
		printf("could not delete inode number %d from dir %s\n",
			   child_inumber, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	unlock_all(unlock_array);
	return SUCCESS;
}

int move(char *name, char *name2)
{

	int parent_inumber, child_inumber, parent_inumber2, child_inumber2;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME], *parent_name2, *child_name2, name_copy2[MAX_FILE_NAME];
	/* use for copy */
	type pType, pType2;
	union Data pdata, pdata2;
	int unlock_array[INODE_TABLE_SIZE];

	for (int i = 0; i < INODE_TABLE_SIZE; i++)
		unlock_array[i] = 0;

	if (strcmp(name, name2) == 0)
	{
		printf("failed to move %s, invalid destination %s\n",
			   name, name2);
		return FAIL;
	}
	if (strlen(name) < strlen(name2))
	{
		int flag = 0;
		for (int i = 0; i < strlen(name); i++)
		{
			if (name[i] != name2[i])
			{
				flag = 1;
				break;
			}
		}
		if (flag == 0)
		{
			printf("failed to move %s, invalid destination %s\n",
				   name, name2);
			return FAIL;
		}
	}
	strcpy(name_copy, name);
	strcpy(name_copy2, name2);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);
	split_parent_child_from_path(name_copy2, &parent_name2, &child_name2);

	if (strcmp(name, name2) < 0)
	{
		parent_inumber = lookup(parent_name, unlock_array, 2);

		parent_inumber2 = lookup(parent_name2, unlock_array, 2);
	}
	else
	{
		parent_inumber2 = lookup(parent_name2, unlock_array, 2);

		parent_inumber = lookup(parent_name, unlock_array, 2);
	}

	if (parent_inumber == FAIL)
	{
		printf("failed to move %s, invalid source parent dir %s\n",
			   name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	if (parent_inumber2 == FAIL)
	{
		printf("failed to move %s, invalid destination parent dir %s\n",
			   name, parent_name2);
		unlock_all(unlock_array);
		return FAIL;
	}

	inode_get(parent_inumber, &pType, &pdata);
	inode_get(parent_inumber2, &pType2, &pdata2);

	child_inumber = lookup_sub_node(child_name, pdata.dirEntries);

	if (child_inumber == FAIL)
	{
		printf("could not move %s, does not exist in dir %s\n",
			   name, child_name);
		unlock_all(unlock_array);
		return FAIL;
	}

	lock_wrlock(&inode_table[child_inumber].rwlock);
	unlock_array[child_inumber] = 1;

	child_inumber2 = lookup_sub_node(child_name2, pdata2.dirEntries);

	if (child_inumber2 != FAIL)
	{
		printf("failed to create %s, already exists in dir %s\n",
			   name, child_name2);
		unlock_all(unlock_array);
		return FAIL;
	}

	if (dir_add_entry(parent_inumber2, child_inumber, child_name2) == FAIL)
	{
		printf("could not add entry %s in dir %s\n",
			   child_name, parent_name2);
		unlock_all(unlock_array);
		return FAIL;
	}

	if (dir_reset_entry(parent_inumber, child_inumber) == FAIL)
	{
		printf("failed to delete %s from dir %s\n",
			   child_name, parent_name);
		unlock_all(unlock_array);
		return FAIL;
	}
	unlock_all(unlock_array);

	return SUCCESS;
	//m a(r)/b(w)/ c(w)    a(r)/b(w)/ d(w)
	//m a/b/c
}

/*
 * Lookup for a given path.
 * Input:
 *  - name: path of node
 *  - flag: lock flag (checks if there should be a lock)
 * Returns:
 *  inumber: identifier of the i-node, if found
 *     FAIL: otherwise
 */
int lookup(char *name, int *unlock_array, int flag)
{
	char full_path[MAX_FILE_NAME];
	char delim[] = "/";

	strcpy(full_path, name);

	/* start at root node */
	int current_inumber = FS_ROOT;

	/* use for copy */
	type nType;
	union Data data;
	char *saveptr;

	if (strcmp(full_path, "") == 0 && flag == 1)
	{
		lock_wrlock(&inode_table[current_inumber].rwlock);
		unlock_array[current_inumber] = 1;
	}
	else if (strcmp(full_path, "") == 0 && flag == 2)
	{
		if (unlock_array[current_inumber] == 0)
		{
			lock_wrlock(&inode_table[current_inumber].rwlock);
			unlock_array[current_inumber] = 1;
		}
	}
	else if (strcmp(full_path, "") != 0 && flag == 2)
	{
		if (unlock_array[current_inumber] == 0)
		{
			lock_rdlock(&inode_table[current_inumber].rwlock);
			unlock_array[current_inumber] = 1;
		}
	}
	else
	{
		lock_rdlock(&inode_table[current_inumber].rwlock);
		unlock_array[current_inumber] = 1;
	}
	char *path = strtok_r(full_path, delim, &saveptr);

	/* get root inode data */
	inode_get(current_inumber, &nType, &data);

	/* search for all sub nodes */
	while (path != NULL && (current_inumber = lookup_sub_node(path, data.dirEntries)) != FAIL)
	{
		path = strtok_r(NULL, delim, &saveptr);
		if (path == NULL && flag == 1)
		{
			lock_wrlock(&inode_table[current_inumber].rwlock);
			unlock_array[current_inumber] = 1;
		}
		else if (path == NULL && flag == 2)
		{
			if (unlock_array[current_inumber] == 0)
			{
				lock_wrlock(&inode_table[current_inumber].rwlock);
				unlock_array[current_inumber] = 1;
			}
		}
		else if (path != NULL && flag == 2)
		{
			if (unlock_array[current_inumber] == 0)
			{
				lock_rdlock(&inode_table[current_inumber].rwlock);
				unlock_array[current_inumber] = 1;
			}
		}
		else
		{
			lock_rdlock(&inode_table[current_inumber].rwlock);
			unlock_array[current_inumber] = 1;
		}
		inode_get(current_inumber, &nType, &data);
	}

	if (!flag)
	{
		unlock_all(unlock_array);
	}

	return current_inumber;
}

/*
 * Prints tecnicofs tree.
 * Input:
 *  - fp: pointer to output file
 */
int print_tecnicofs_tree(FILE *fp)
{
	lock_wrlock(&inode_table[FS_ROOT].rwlock);	
	inode_print_tree(fp, FS_ROOT, "");
	lock_unlock(&inode_table[FS_ROOT].rwlock);	

	return SUCCESS;
}
