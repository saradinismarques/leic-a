#include "server_app.h"

void constructFilePath(char *path, char *dir, char *id, char *id1, char *type) {
    strcpy(path, dir);
    strcat(path, "/");
    strcat(path, id);
    strcat(path, "/");
    if(id1 != NULL) {
        strcat(path, id1);
        strcat(path, "_");
    }
    if(type != NULL) {
        strcat(path, type);
        strcat(path, ".txt");
    }
}

void createFile(char *path, char *data) {
    FILE *fp;

    fp = fopen(path, "w");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    int nwritten = fwrite(data, 1, strlen(data), fp);
    if(nwritten != strlen(data)) {
        perror("Error writing on file\n");
        exit(1);
    }
    fclose (fp);
}

void deleteFile(char *path) {
    if(unlink(path) != 0) {
        perror("Error deleting file\n");
        exit(1);
    }
}

int isFile(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int existsFile(char *file) {
    if(access(file, F_OK ) == -1)
        return ERROR;
    return SUCCESS;
}

void createDirectory(char *path) {
    if(mkdir(path, 0700) == -1) 
        if(errno != EEXIST) {
            perror("Error creating directory\n");
            exit(1);
        }
}

void deleteDirectory(char *path) {
    DIR *d;
    struct dirent *dir;
    d = opendir(path);
    char file_path[PATH_MAX_SIZE] = "";
    
    if(d == NULL) {
        perror("Error opening directory\n");
        exit(1);
    }
    while((dir = readdir(d)) != NULL) { /* deletes all files in this directory */
        if(dir->d_name[0] == '.')
            continue;
        sprintf(file_path, "%s/%s", path, dir->d_name);
        deleteFile(file_path);
    }
    closedir(d);
    if(rmdir(path) != 0) {
        perror("Error removing directory\n");
        exit(1);
    }
}

int existsDirectory(char *path) {
    struct stat st = {0};
    return (stat(path, &st) == 0);
}

int countUsers() {
    DIR* d = opendir("USERS");
    struct dirent *dir;
    int counter = 0;

    if(d == NULL) {
        perror("Error opening directory\n");
        exit(1);
    }
    while((dir = readdir(d)) != NULL){
        if(dir->d_name[0] == '.')
            continue;
        counter++;
    }
    closedir(d);
   
    return counter;
}

int countMessages(char *GID) {
    DIR* d;
    struct dirent *dir;
    char msg_path[PATH_SIZE] = "";
    int counter = 0;
    
    sprintf(msg_path, "GROUPS/%s/MSG", GID);
   
    if(!existsDirectory(msg_path)) 
        return 0;
    d = opendir(msg_path);
    if(d == NULL) {
        perror("Error opening directory\n");
        exit(1);
    }
    while((dir = readdir(d)) != NULL){
        if(dir->d_name[0]=='.')
            continue;
        counter++;
    }
    closedir(d);
    return counter;
}

int initGroupList(GROUPLIST *list) {
    DIR *d;
    struct dirent *dir;
    FILE *fp;
    char group_path[PATH_MAX_SIZE] = "";
    int counter = 0;
    
    d = opendir("GROUPS");
    if(d == NULL) {
        perror("Error opening directory\n");
        exit(1);
    }
    while((dir = readdir(d)) != NULL) { /* initializes all the list struct with the contents in the directory GROUPS */
        if(dir->d_name[0] == '.')
            continue;

        int gid = atoi(dir->d_name);
        strcpy(list[gid-1].gid, dir->d_name); /* GID */
        sprintf(group_path, "GROUPS/%s/%s_name.txt", dir->d_name, dir->d_name);
        fp = fopen(group_path, "r");
        if(fp == NULL) {
            perror("Error opening file\n");
            exit(1);
        }
        int nread = fread(list[gid-1].gname, sizeof(char), sizeof(list[counter].gname), fp); /* group name */
        if(nread < 0) {
            perror("Error reading from file\n");
            exit(1);
        }
        fclose(fp);
        list[gid-1].no_msgs = countMessages(dir->d_name); /* number of messages */
        counter++;
    }
    closedir(d);
    return counter; /* returns number of groups */
}

int checkPassword(char* UID, char * pass) {
    char pass_path[PATH_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    FILE * fp;

    constructFilePath(pass_path, "USERS", UID, UID, "pass");
    fp = fopen(pass_path, "r");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    int nread = fread(buffer, sizeof(char), sizeof(buffer), fp);
    if(nread < 0) {
        perror("Error reading from file\n");
        exit(1);
    }
    fclose(fp);

    if(strcmp(pass, buffer) == 0)
        return SUCCESS;
    else
        return ERROR;   
}
