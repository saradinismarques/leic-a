#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

char fileAddress[30];
char GID[6]; // pode ter tamanho 5 ou 6

#define ERROR 0
#define ERR -1
#define SUCCESS 1

typedef struct grouplist{
    int no_groups;
    char group_no[98][3];
    char group_name[98][25];
    int group_msg[98]; // nº de mensagens por grupo

}GROUPLIST;

int CreateUserDir(char *UID){
    char user_dirname[20];
    int ret;
    sprintf(user_dirname,"USERS/%s",UID);
    ret=mkdir(user_dirname,0700);
    if(ret==-1)
        return ERROR;
    printf("success");
    return SUCCESS;
}

int DelUserDir(char *UID){
    char user_dirname[20];
    sprintf(user_dirname,"USERS/%s",UID);
    if(rmdir(user_dirname)==0)
        return SUCCESS;
    else
        return ERROR;
}

int ListGroupsDir(GROUPLIST *list){
    DIR *d;
    struct dirent *dir;
    int i=0;
    FILE *fp;
    char GIDname[30];
    list->no_groups=0;
    d = opendir("GROUPS");
    if(d){
        while ((dir = readdir(d)) != NULL){
            if(dir->d_name[0]=='.')
                continue;
            if(strlen(dir->d_name)>2)
                continue;
            strcpy(list->group_no[i], dir->d_name);
            sprintf(GIDname,"GROUPS/%s/%s_name.txt",dir->d_name,dir->d_name);
            fp = fopen(GIDname,"r");
            if(fp){
                fscanf(fp,"%24s",list->group_name[i]);
                fclose(fp);
            }
            ++i;
            if(i==99)
                break;
            countMessageGroup(list, dir->d_name);
        }
        list->no_groups=i;
        closedir(d);
    }
    else
        return ERR;

    // if(list->no_groups>1)
    //     SortGList(list);

    return(list->no_groups);
}

int countUsers(){
    DIR* d;
    int counter = 0;
    struct dirent *dir;
    char dir_path[30] = "USERS/";
    d = opendir(dir_path);
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(dir->d_name[0]=='.')
                continue;
            counter++;
        }
    }
    return counter;
}

int countMessageGroup(GROUPLIST* list, char* GID){
    DIR* d;
    struct dirent *dir;
    char dir_path[30] = "";
    int i = atoi(GID);
    sprintf(dir_path,"GROUPS/%s/MSG",GID);
    d = opendir(dir_path);
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(dir->d_name[0]=='.')
                continue;
            list->group_msg[i-1]++;
        }
    }
    return list->group_msg[i-1];
}

char* convertMID(int n){
    char MID[5] = "";
    char *MID_ptr = MID;
    if ( n < 10)
        sprintf(MID,"000%d",n);
    else if ( n < 100)
        sprintf(MID,"00%d",n);
    else if ( n < 1000)
        sprintf(MID,"0%d",n);
    else
        sprintf(MID,"%d",n);
    
    printf("convertMID: '%s'\n",MID);

    return MID_ptr;
}

char* convertGID(int n){
    char GID[5] = "";
    char *GID_ptr = GID;
    if (n < 10)
        sprintf(GID,"0%d",n);
    else
        sprintf(GID,"%d",n);
    
    return GID_ptr;
}

int DelPassFile(char *UID){
    char pathname[50];
    sprintf(pathname,"USERS/%s/%s_pass.txt",UID,UID);
    if(unlink(pathname)==0)
        return SUCCESS;
    else
        return ERROR;
}

// int TimerON(int sd){

//     struct timeval tmout;

//     memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
//     tmout.tv_sec=15; /* Wait for 15 sec for a reply from server. */

//     return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
// }

// int TimerOFF(int sd){

//     struct timeval tmout;

//     memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */

//     return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tmout,sizeof(struct timeval)));
// }

// projeto Kika

char* constructFileAddress(char* dir, char* id, char* id1, char* type) {

    memset (fileAddress,0,sizeof(fileAddress));
    strcpy(fileAddress, dir);
    strcat(fileAddress, "/");
    strcat(fileAddress, id);
    strcat(fileAddress, "/");
    if (id1 != NULL){
        strcat(fileAddress, id1);
        strcat(fileAddress, "_");
    }
    if(type != NULL) {
        strcat(fileAddress, type);
        strcat(fileAddress, ".txt");
    }

    return fileAddress;
}

int createDirectory(char* dirName) {
    int n;
    n = mkdir(dirName, 0700);

    if (n != 0) {
        fprintf(stderr, "Value of errno: %d\n", n);
        perror("Error AS creating Directory: ");
        return ERROR;
    }
    return SUCCESS;
}

int existsDirectory(char* dirName) {
    struct stat st = {0};
    return (stat(dirName, &st) == 0);
}

int fileExists(char* filename){
    if(access( filename, F_OK ) != -1) {
        // file exists
        return SUCCESS;
    } else {
        // file doesn't exist
        return ERROR;
    }
}

/*Creates a File*/
int createFile(char* dir, char* id, char* id1, char* type, char * data) {
    char address[30] = "" ;
    FILE *fp;

    strcpy(address, dir);

    /*if USERS directory does not exist*/
    if (!existsDirectory(address)) {
        createDirectory(address); /*creates the directory*/
    }

    /*If the id directory does not exist*/
    strcat(address, "/");
    strcat(address, id);
    if (!existsDirectory(address)){
        CreateUserDir(address); /*creates the directory*/
    }
    fp = fopen(constructFileAddress(dir,id,id1,type), "w+");
    if (fp == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS creating File: ");
        return ERROR;
    }

    fputs(data, fp);
    fclose (fp);
    return SUCCESS;
}


int deleteFile(char* dir, char* id, char* id1, char* type ) {
    int n;
    char address[30] = "";

    strcpy(address, dir);
    strcat(address, "/");
    strcat(address, id);
    strcat(address, "/");
    if ( id1 != NULL ){
        strcat(address, id1);
        strcat(address, "_");
    }
    strcat(address, type);
    strcat(address, ".txt");

    n = remove(address);

    if (n != 0) {
        if (errno == 2) {
            /*File does not exist, continues the program*/
            return ERROR;
        }

        /*fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS deleting File: ");
        killServer(); */
    }
    return SUCCESS;
}


int checkPassword(char* UID, char * pass){
    char address[128] = "", passwordFile[128] = "", buffer[9] = "";
    FILE * fp;
    int num;

    strcpy(address, "USERS/");
    strcat(address, UID);

    if(existsDirectory(address)){

        strcpy(passwordFile, constructFileAddress("USERS",UID,UID,"pass"));

        if (!fileExists(passwordFile)) {
            return ERROR;
        }

        fp = fopen(passwordFile, "r");
        if(fp == NULL )  {
            return ERROR;
        }
      
        int res = fread(buffer,sizeof(char),sizeof(buffer),fp);

        fclose(fp);

        if (strcmp(pass,buffer) == 0)
            return SUCCESS;
        else
            return ERROR;
    }
    return ERROR;
}

char *getGname(char* GID) {
    char address[128]= "" , groupFile[128] = "", buffer[9] = "";
    FILE * fp;
    int num;
    char *buffer_ptr = buffer;

    strcpy(address, "GROUPS/");
    strcat(address, GID);
    if(existsDirectory(address)){

        strcpy(groupFile, constructFileAddress("GROUPS",GID,GID,"name"));

        if (!fileExists(groupFile)) {
            return NULL;
        }

        fp = fopen(groupFile, "r");
        if(fp == NULL){

            return NULL;
        }
      
        int res = fread(buffer,sizeof(char),sizeof(buffer),fp);

        fclose(fp);
        return buffer_ptr;
    }
    return NULL;
}

int checkGname(char* GID, char * Gname) {
        if (strcmp(getGname(GID),Gname) == 0)
            return SUCCESS;
        else
            return ERROR;
    return ERROR;
}



