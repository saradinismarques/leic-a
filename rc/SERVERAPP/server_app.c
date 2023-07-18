#include "server_app.h"
#include "server_udp.c"
#include "server_tcp.c"
#include "../validation.c"
#include "directories.c"

int main(int argc, char *argv[]) {
    int tcpfd, udpfd, newfd, afd[MAX_USERS];
    fd_set rfds;
    enum {idle,busy} state;
    int maxfd, counter;
    char message[MESSAGE_SIZE] = "";
    socklen_t addrlen;
	struct sockaddr_in addr;
    int i;

    parseArguments(argc, argv);

    tcpfd = connectToTCPSocket();
    udpfd = connectToUDPSocket();

    /* initializes variables with users and groups that already exist */
    no_groups = initGroupList(list);
    no_users = countUsers();

    /* initializes array that will keep fds for each client */
    for(i = 0; i < MAX_CONNECTIONS; i++) {
        afd[i] = -1;
    }
    state = idle;

    while(1) {
        FD_ZERO(&rfds);
        FD_SET(tcpfd, &rfds);
		FD_SET(udpfd, &rfds);
        maxfd = max(tcpfd, udpfd);
    
        for(i = 0; i < MAX_CONNECTIONS; i++) {
            if(afd[i] != -1) { 
                FD_SET(afd[i], &rfds);
                maxfd = max(maxfd, afd[i]);
            }
        }
        counter = select(maxfd+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
    
        if(counter == 0) {
            perror("Error on select\n");
            exit(1);
        }
        /* TCP */
        if(FD_ISSET(tcpfd, &rfds)){
            addrlen = sizeof(addr);
            if((newfd = accept(tcpfd, (struct sockaddr*)&addr, &addrlen)) == -1) {
                perror("Error accepting a new connection\n");
                exit(1);
            }
            switch(state){
                case idle:    
                    for(i = 0; i < MAX_CONNECTIONS; i++) { /* searches for one position available */
                        if(afd[i] == -1) { 
                            close(afd[i]);
                            afd[i] = newfd;
                            break;
                        }
                    }
                    if(i == MAX_CONNECTIONS) /* all array positions are full */             
                        state = busy;
                    break;
                case busy: 
                    close(newfd); 
                    break;
            }
        }
        for(int i = 0; i < MAX_CONNECTIONS; i++) {
            if(FD_ISSET(afd[i], &rfds)){
                readTCPMessage(afd[i], message, COMMAND_SIZE);
                if(!checkReceivedMessage(afd[i], message)) {
                    memset(message, 0, MESSAGE_SIZE);
                    constructMessage(message, "ERR", NULL, NULL, NULL);
                    strcat(message, "\n"); 
                    sendTCPMessage(afd[i], message, strlen(message));
                }
                afd[i] = -1;
                state = idle;
            }
        }
        /* UDP */
        if(FD_ISSET(udpfd, &rfds)) {
            readUDPMessage(udpfd, message);
            if(!checkReceivedMessage(udpfd, message)) {
                memset(message, 0, MESSAGE_SIZE);
                constructMessage(message, "ERR", NULL, NULL, NULL);
                strcat(message, "\n"); 
                sendUDPMessage(udpfd, message);
            }
        }
    }
    for(i = 0; i < MAX_CONNECTIONS; i++) 
        close(afd[i]);
    closeTCPSocket(tcpfd);
    closeUDPSocket(udpfd);
    return 0;
}

void parseArguments(int argc, char * argv[]) {
    int option;
    char options[2][2] = {"", ""}; /* saves options given when running the program 0=p; 1=v */

    if(argc > 1) {
        if(strcmp(argv[1], "-p") == 0) {
            strcpy(DSport, argv[2]);
            strcpy(options[0], "p");
        }
    }
    if(argc > 3) {
        if(strcmp(argv[3], "-v") == 0) {
            verboseMode = 1;
            strcpy(options[1], "v");
        }
    }

    /* checks given options and completes them if not passed */
    if(strcmp(options[0], "p") != 0) {
        strcpy(DSport, DEFAULT_DS_PORT);
    } 
    if(strcmp(options[1], "v") != 0) {
        verboseMode = 0;
    }
}

int checkReceivedMessage(int fd, char *message) { 
    char arg1[ARG1_SIZE] = "";
    char arg2[ARG2_SIZE] = "";
    char arg3[ARG3_SIZE] = "";
    char command[COMMAND_SIZE] = "";

    sscanf(message, "%s %s %s %s", command, arg1, arg2, arg3);
    if(strcmp(command, "REG") == 0) {
        reg(fd, arg1, arg2);
        return SUCCESS;
    } else if(strcmp(command, "UNR") == 0) {
        unregister(fd, arg1, arg2);
        return SUCCESS;
    }
    else if(strcmp(command, "LOG") == 0) {
        login(fd, arg1, arg2);
        return SUCCESS;
    }
    else if(strcmp(command, "OUT") == 0) {
        logout(fd, arg1, arg2);
        return SUCCESS;
    }
    else if(strcmp(command, "GLS") == 0) {
        groups(fd);
        return SUCCESS;
    }
    else if(strcmp(command, "GSR") == 0) {
        subscribe(fd, arg1, arg2, arg3);
        return SUCCESS;
    }
    else if(strcmp(command, "GUR") == 0) {
        unsubscribe(fd, arg1, arg2);
        return SUCCESS;
    }
    else if(strcmp(command, "GLM") == 0) {
        my_groups(fd, arg1);
        return SUCCESS;
    }
    else if(strcmp(command, "ULS") == 0) {
        ulist(fd);
        return SUCCESS;
    }
    else if(strcmp(command, "PST") == 0) {
        post(fd);
        return SUCCESS;
    }
    else if(strcmp(command, "RTV") == 0) {
        retrieve(fd);
        return SUCCESS;
    }
    else 
        return ERROR; 
}
    
void constructMessage(char *message, char *arg1, char *arg2, char *arg3, char *arg4) {
    if(arg1 != NULL) {
        strcat(message, arg1);
    }
    if(arg2 != NULL) {
        strcat(message, " ");
        strcat(message, arg2);
    }
    if(arg3 != NULL) {
        strcat(message, " ");
        strcat(message, arg3);
    }
    if(arg4 != NULL) {
        strcat(message, " ");
        strcat(message, arg4);
    }
}

void convertMIDChar(char *MID, int mid) {
    if(mid < 10)
        sprintf(MID, "000%d", mid);
    else if(mid < 100)
        sprintf(MID, "00%d", mid);
    else if(mid < 1000)
        sprintf(MID, "0%d", mid);
    else
        sprintf(MID, "%d", mid);
}

void convertGIDChar(char *GID, int gid) {
    if(gid < 10)
        sprintf(GID, "0%d", gid);
    else
        sprintf(GID, "%d", gid);
}

/* -------------------------------------- command functions -------------------------------------- */
/* UDP */
void reg(int fd, char* UID, char* pass) {
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char pass_path[PATH_SIZE] = "";

    if(!validUID(UID) || !validPass(pass)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);

        if(verboseMode)
            printf("UID: %s, wrong format in register\n", UID);

        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    createDirectory(user_path);
    
    if(errno == EEXIST) {
        constructMessage(message, "RRG", "DUP", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, user duplicated\n", UID);
    } else if(no_users != MAX_USERS) {
        constructFilePath(pass_path, "USERS", UID, UID, "pass");
        createFile(pass_path, pass);
        no_users++;
        constructMessage(message, "RRG", "OK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, new user created\n", UID);
    } else {
        constructMessage(message, "RRG", "NOK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, error creating new user\n", UID);
    } 
    sendUDPMessage(fd, message);
}

void unregister(int fd, char* UID, char* pass) {
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE] = "";

    if(!validUID(UID) || !validPass(pass)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);

        if(verboseMode)
            printf("UID: %s, wrong format in unregister\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);

    if(existsDirectory(user_path) && checkPassword(UID, pass)) {
        deleteDirectory(user_path);
        no_users--;
        constructMessage(message, "RUN", "OK", NULL, NULL);
        strcat(message, "\n"); 

         if(verboseMode)
            printf("UID: %s, unregister successful\n", UID);
    } else {
        constructMessage(message, "RUN", "NOK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, error unregistering user\n", UID);
    }
    sendUDPMessage(fd, message);
}

void login(int fd, char* UID, char* pass) {
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE];
    char login_path[PATH_SIZE];
    
    if(!validUID(UID) || !validPass(pass)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);
        if(verboseMode)
            printf("UID: %s, wrong format in login\n", UID);
       return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);

    if(existsDirectory(user_path) && checkPassword(UID, pass)) {
        constructFilePath(login_path, "USERS", UID, UID, "login");
        createFile(login_path, "");
        constructMessage(message, "RLO", "OK", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s, login successful\n", UID);
    } else {
        constructMessage(message, "RLO", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s, login unsuccessful\n", UID);
    }
    sendUDPMessage(fd, message);
}

void logout(int fd, char* UID, char* pass) {
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char login_path[PATH_SIZE] = "";
       
    if(!validUID(UID) || !validPass(pass)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);
        
        if(verboseMode)
            printf("UID: %s, wrong format in logout\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);

    if(existsDirectory(user_path) && checkPassword(UID, pass)) {
        constructFilePath(login_path, "USERS", UID, UID, "login");
        deleteFile(login_path);
        constructMessage(message, "ROU", "OK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, logout successful\n", UID);
    } else {
        constructMessage(message, "ROU", "NOK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s, logout unsuccessful\n", UID);
    }
    sendUDPMessage(fd, message);
}

void groups(int fd) {
    char message[LIST_SIZE] = "";
    char N[GID_SIZE];
    char MID[MID_SIZE];

    if(no_groups == 0) { // não há grupos
        constructMessage(message, "RGL", "0", NULL, NULL);

        if(verboseMode)
            printf("There are no groups\n");
    } else {
        convertGIDChar(N, no_groups); 
        constructMessage(message, "RGL", N, NULL, NULL);

        for(int i = 0; i < no_groups; i++) {
            convertMIDChar(MID, list[i].no_msgs);
            constructMessage(message, NULL, list[i].gid, list[i].gname, MID);
        }
    }
    strcat(message, "\n");
    if(verboseMode)
        printf("There are %s groups. Sending groups information\n", N);
    sendUDPMessage(fd, message);
}

void subscribe(int fd, char *UID, char *GID, char *GName){
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char uid_path[PATH_SIZE] = "";
    char group_path[PATH_SIZE] = "";
    char gname_path[PATH_SIZE] = "";
    char N[GID_SIZE] = "";
    int gid = atoi(GID);

    if(!validUID(UID) || !validGID(GID) || !validGName(GName)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);
        
        if(verboseMode)
            printf("UID: %s, wrong format in subscribe\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    constructFilePath(group_path, "GROUPS", GID, NULL, NULL);
    
    if(!existsDirectory(user_path)) {
        constructMessage(message, "RGS", "E_USR", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s is invalid\n", UID);
    } else if(strcmp(GID, "00") == 0) { /* creates new group */
        if(no_groups == MAX_GROUPS) {
            constructMessage(message, "RGS", "E_FULL", NULL, NULL);
            strcat(message, "\n"); 
        
            if(verboseMode)
                printf("UID: %s GID: %s group is full\n", UID, GID);
        } else {
            convertGIDChar(N, ++no_groups);
            constructFilePath(group_path, "GROUPS", N, NULL, NULL);
            createDirectory(group_path);
            strcpy(list[no_groups-1].gid, N); /* puts new group data on struct list */
            constructFilePath(gname_path, "GROUPS", N, N, "name");
            createFile(gname_path, GName);
            strcpy(list[no_groups-1].gname, GName);
            constructFilePath(uid_path, "GROUPS", N, NULL, UID); /* user that creates the groups is subscribed in it */
            createFile(uid_path, "");
            constructMessage(message, "RGS", "NEW", N, NULL);
            strcat(message, "\n"); 
    
            if(verboseMode)
                printf("GID: %s new group created \"%s\"\nUID: %s user subscribed to group %s \"%s\"\n", N, GName, UID, N, GName);
        }
    } else if(!existsDirectory(group_path)) {
        constructMessage(message, "RGS", "E_GRP", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s GID: %s GID is invalid\n", UID, GID);
    } else if(strcmp(list[gid-1].gname, GName) != 0) {
        constructMessage(message, "RGS", "E_GNAME", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s GID: %s GName is invalid\n", UID, GID);
    } else {
        constructFilePath(uid_path, "GROUPS", GID, NULL, UID);
        createFile(uid_path, "");
        constructMessage(message, "RGS", "OK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s user subscribed to group %s \"%s\"\n", UID, GID, GName);
    }
    sendUDPMessage(fd, message);
}

void unsubscribe(int fd, char *UID, char *GID) {
    char message[MESSAGE_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char group_path[PATH_SIZE] = "";
    char uid_path[PATH_SIZE] = "";

    if(!validUID(UID) || !validGID(GID)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);
        
        if(verboseMode)
            printf("UID: %s, wrong format in unsbuscribe\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    constructFilePath(group_path, "GROUPS", GID, NULL, NULL);
    constructFilePath(uid_path, "GROUPS", GID, NULL, UID);
    
    if(!existsDirectory(user_path)){
        constructMessage(message, "RGU", "E_USR", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s is invalid\n", UID);
    } else if(!existsDirectory(group_path)) {
        constructMessage(message, "RGU", "E_GRP", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s GID: %s GID is invalid\n", UID, GID);
    } else if(!existsFile(uid_path)) {
        constructMessage(message, "RGU", "NOK", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s is invalid\n", UID);
    } else {
        deleteFile(uid_path);
        constructMessage(message, "RGU", "OK", NULL, NULL);
        strcat(message, "\n"); 
        
        if(verboseMode)
            printf("UID: %s unsubscribed from GID: %s\n", UID, GID);
    }
    sendUDPMessage(fd, message);
}

void my_groups(int fd, char *UID) {
    char message[LIST_SIZE] = "";
    char buffer[LIST_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char uid_path[PATH_SIZE] = "";
    int user_groups = 0;
    char N[GID_SIZE] = "";
    char MID[MID_SIZE] = "";
    
    if(!validUID(UID)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendUDPMessage(fd, message);
        
        if(verboseMode)
            printf("UID: %s wrong format in my_groups\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    
    if(!existsDirectory(user_path)) {
        constructMessage(message, "RGM", "E_USR", NULL, NULL);
        strcat(message, "\n"); 

        if(verboseMode)
            printf("UID: %s user not logged in or does not exist\n", UID);
    } else {
        for(int i = 0; i < no_groups; i++) { /* checks in every group if user is subscribed */
            convertGIDChar(N, i+1);
            constructFilePath(uid_path, "GROUPS", N, NULL, UID);
            
            if(existsFile(uid_path)) { 
                user_groups++;
                convertMIDChar(MID, list[i].no_msgs);
                constructMessage(buffer, NULL, list[i].gid, list[i].gname, MID);
            }
        }
        if(user_groups == 0) { /* user is not subscribed in any group */
            strcpy(N, "0");
            constructMessage(message, "RGM", "0", NULL, NULL);
            strcat(message, "\n"); 
        } else {
            char *aux = buffer + 1;
            sprintf(N, "%d", user_groups);
            constructMessage(message, "RGM", N, aux, NULL);
            strcat(message, "\n"); 
        }
        if(verboseMode)
            printf("UID: %s user subscribed to %s groups\n", UID, N);
    }
    sendUDPMessage(fd, message);
}

/* TCP */
void ulist(int fd) {
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    char GID[GID_SIZE] = "";
    char GName[GNAME_SIZE] = "";
    char group_path[PATH_SIZE] = "";
    char gname_path[PATH_SIZE] = "";
    FILE *fp;
    
    /* reads GID */
    readTCPMessage(fd, GID, 2);
    
    if(!validGID(GID)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        if(verboseMode)
            printf("GID: %s, wrong format in ulist\n", GID);
        return;
    }
    readTCPMessage(fd, buffer, 1);
    constructFilePath(group_path, "GROUPS", GID, NULL, NULL);
    constructFilePath(gname_path, "GROUPS", GID, GID, "name");

    if(!existsDirectory(group_path)) { 
        constructMessage(message, "RUL", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("GID: %s is invalid\n", GID);
        return;
    }
    fp = fopen(gname_path, "r");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    /* reads group name and sends it */
    int gname_size = fread(GName, sizeof(char), sizeof(GName), fp);
    if(gname_size < 0) {
        perror("Error reading from file\n");
        exit(1);
    }
    fclose(fp);
    constructMessage(message, "RUL", "OK", GName, NULL);
    sendTCPMessage(fd, message, strlen(message));

    DIR *d = opendir(group_path);
    struct dirent *dir;

    if(d == NULL) {
        perror("Error opening directory\n");
        exit(1);
    }
    char file_path[PATH_MAX_SIZE] = "";
    char UID[UID_FILE] = "";

    while((dir = readdir(d)) != NULL) { /* checks all the files in the directory and sends the ones corresponding to user ids */
        sprintf(file_path, "%s%s", group_path, dir->d_name);

        if(dir->d_name[0] == '.')
            continue;
        if (strcmp(file_path, gname_path) == 0 ) /* ignores file that has the group name */
            continue;
        if(isFile(file_path) == 0) /* ignores directory MSG if it exists */
            continue;

        int size = strlen(dir->d_name);
        sprintf(UID, "%s", dir->d_name);
        UID[size-4] = '\0'; /* removes .txt */
        sendTCPMessage(fd, " ", 1);
        sendTCPMessage(fd, UID, strlen(UID));
    }
    strcat(message, "\n"); 
    closedir(d);

    if(verboseMode)
        printf("GID: %s listing users subscribed to group\n", GID);

    sendTCPMessage(fd, "\n", 1);
}

void post(int fd) {
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    char UID[UID_SIZE] = "";
    char GID[GID_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char group_path[PATH_SIZE] = "";
    char uid_path[PATH_SIZE] = "";
    char size[FILE_DIGITS] = "";

    /* reads UID and GID */
    readTCPMessage(fd, UID, 5);
    readTCPMessage(fd, buffer, 1);
    readTCPMessage(fd, GID, 2);
    readTCPMessage(fd, buffer, 1);

    if(!validUID(UID) || !validGID(GID)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));

        if(verboseMode)
            printf("UID: %s, wrong format in unsbuscribe\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    constructFilePath(group_path, "GROUPS", GID, NULL, NULL);
    constructFilePath(uid_path, "GROUPS", GID, NULL, UID);
    
    if(!existsDirectory(user_path) || !existsDirectory(group_path) || !existsFile(uid_path)) {
        constructMessage(message, "RPT", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        if(verboseMode)
            printf("UID: %s GID: %s UID or GID is invalid\n", UID, GID);
        return;
    }
    /* reads text size */
    while(1) {
        readTCPMessage(fd, buffer, 1);
        
        if(strcmp(buffer, " ") == 0)
            break;
        strcat(size, buffer);
    }
    int Tsize = atoi(size);

    /* if text size is larger than limit returns */
    if(Tsize > TEXT_SIZE-1) {
        constructMessage(message, "RPT", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        if(verboseMode)
            printf("UID: %s error posting in group GID: %s\n", UID, GID);
        return;
    }

    char text[TEXT_SIZE] = "";
    /* reads text */
    readTCPMessage(fd, text, Tsize);

    char msg_path[PATH_SIZE] = "";
    int gid = atoi(GID);
    sprintf(msg_path,"GROUPS/%s/MSG", GID);
    char MID[MID_SIZE] = "";

    if(!existsDirectory(msg_path)) { /* creates directory MSG if it does not exists */
        createDirectory(msg_path);
    }
    if(list[gid-1].no_msgs == MAX_MSGS) { 
        constructMessage(message, "RPT", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("UID: %s error posting in group GID: %s\n", UID, GID);
       return;
    } else {
        convertMIDChar(MID, ++(list[gid-1].no_msgs));
    }
    
    char mid_path[PATH_SIZE] = "";
    sprintf(mid_path,"%s/%s", msg_path, MID);
    
    createDirectory(mid_path);
    char author_path[PATH_SIZE] = "";
    char text_path[PATH_SIZE] = "";
    FILE *fp;

    /* writes the UID on the author file */
    strcpy(author_path, mid_path);
    strcat(author_path, "/A U T H O R.txt");
    fp = fopen(author_path, "w");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    int nwritten = fwrite(UID, 1, strlen(UID), fp);
    if(nwritten != strlen(UID)) {
        perror("Error writing on file\n");
        exit(1);
    }
    fclose(fp);

    /* writes the text on the text file */
    strcpy(text_path, mid_path);
    strcat(text_path, "/T E X T.txt");
    fp = fopen(text_path, "w");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    nwritten = fwrite(text, 1, strlen(text), fp);
    if(nwritten != strlen(text)) {
        perror("Error writing on file\n");
        exit(1);
    }
    fclose(fp);
    
    readTCPMessage(fd, buffer, 1);

    if(strcmp(buffer, "\n") == 0) { /* if message has no file returns */
        constructMessage(message, "RPT", MID, NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("UID: %s posting message MID: %s to group GID: %s\n", UID, MID, GID);
        return;
    }
    char FName[FNAME_SIZE] = "";
    /* reads file name */
    while(1) {
        readTCPMessage(fd, buffer, 1);

        if(strcmp(buffer, " ") == 0) 
            break;
        strcat(FName, buffer);
    }

    if(!validFName(FName)) {
        constructMessage(message, "RPT", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));

        if(verboseMode)
            printf("UID: %s error posting in group GID: %s\n", UID, GID);
        return;
    }

    char file_path[PATH_SIZE] = "";
    strcpy(file_path, mid_path);
    strcat(file_path, "/");
    strcat(file_path, FName);

    fp = fopen(file_path, "w");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    memset(size, 0, FILE_DIGITS);
    /* reads file size */
    while(1) {
        readTCPMessage(fd, buffer, 1);

        if(strcmp(buffer, " ") == 0) 
            break;
        strcat(size, buffer);
    }
    /* reads file data */
    long int Fsize = atoi(size);
    long int nleft = Fsize;
    int nread = BUFFER_SIZE;

    while(nleft > 0) { /* while is reading from socket writes on file opened before */
        if(nleft < 512)
            nread = nleft;

        readTCPMessage(fd, buffer, nread);
        int nwritten = fwrite(buffer, 1, nread, fp);
        if(nwritten != nread) {
            perror("Error writing on file\n");
            exit(1);
        }
        nleft -= BUFFER_SIZE;
    }
    fclose(fp);
    constructMessage(message, "RPT", MID, NULL, NULL);
    strcat(message, "\n"); 
    sendTCPMessage(fd, message, strlen(message));

    if(verboseMode)
        printf("UID: %s posting message MID: %s to group GID: %s\n", UID, MID, GID);
}

void retrieve(int fd) {  
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    char UID[UID_SIZE] = "";
    char GID[GID_SIZE] = "";
    char MID[MID_SIZE] = "";
    char user_path[PATH_SIZE] = "";
    char group_path[PATH_SIZE] = "";
    char uid_path[PATH_SIZE] = "";
    char msg_path[PATH_SIZE] = "";
    char mid_path[PATH_SIZE] = "";

    /* reads UID, GID and MID */
    readTCPMessage(fd, UID, 5); 
    readTCPMessage(fd, buffer, 1);
    readTCPMessage(fd, GID, 2);
    readTCPMessage(fd, buffer, 1);
    readTCPMessage(fd, MID, 4);
        
    if(!validUID(UID) || !validGID(GID) || !validMID(MID)) {
        constructMessage(message, "ERR", NULL, NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("UID: %s, wrong format in unsbuscribe\n", UID);
        return;
    }
    constructFilePath(user_path, "USERS", UID, NULL, NULL);
    constructFilePath(group_path, "GROUPS", GID, NULL, NULL);
    constructFilePath(uid_path, "GROUPS", GID, NULL, UID);
    sprintf(msg_path,"GROUPS/%s/MSG", GID);
    sprintf(mid_path, "%s/%s", msg_path, MID);

    if(!existsDirectory(user_path) || !existsDirectory(group_path) || !existsFile(uid_path) || !existsDirectory(mid_path)) {
        constructMessage(message, "RRT", "NOK", NULL, NULL);
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("UID: %s GID: %s MID: %s UID or GID or MID is invalid\n", UID, GID, MID);
        return;
    }

    int mid = atoi(MID);
    int gid = atoi(GID);
    int no_msgs = list[gid-1].no_msgs-mid+1; /* number of messages to retrieve */
    if(no_msgs > RTV_MSGS) /* if number bigger than the limit - 20 */
        no_msgs = RTV_MSGS;
   
    if(no_msgs == 0) { /* no messages to retrieve */
        constructMessage(message, "RRT", "EOF", NULL, NULL); 
        strcat(message, "\n"); 
        sendTCPMessage(fd, message, strlen(message));
        
        if(verboseMode)
            printf("UID: %s there are no messages in group GID: %s\n", UID, GID);
        return;
    }
    char N[MID_SIZE] = "";
    sprintf(N, "%d", no_msgs);
    constructMessage(message, "RRT", "OK", N, NULL);
    sendTCPMessage(fd, message, strlen(message));
    char author_path[PATH_SIZE] = "";
    char text_path[PATH_SIZE] = "";
    FILE *fp;
    DIR *d;
    struct dirent *dir;
    char text[TEXT_SIZE] = "";

    if(verboseMode)
        printf("UID: %s retrieving messages starting from MID: %s from group GID: %s\n", UID, MID, GID);

    while(no_msgs > 0) { /* retrieves all the messages */
        memset(message, 0, MESSAGE_SIZE);
        convertMIDChar(MID, mid);
        constructMessage(message, NULL, MID, NULL, NULL);
        
        sprintf(mid_path, "%s/%s", msg_path, MID);
        sprintf(author_path, "%s/A U T H O R.txt", mid_path);

        fp = fopen(author_path, "r");
        if(fp == NULL) {
            perror("Error opening file\n");
            exit(1);
        }
        /* reads the user id that posted the message */
        memset(UID, 0, UID_SIZE);
        int uid_size = fread(UID, sizeof(char), sizeof(UID), fp);
        if(uid_size < 0) {
            perror("Error reading from file\n");
            exit(1);
        }
        fclose(fp);

        sprintf(text_path, "%s/T E X T.txt", mid_path);
        fp = fopen(text_path, "r");
        if(fp == NULL) {
            perror("Error opening file\n");
            exit(1);
        }
        /* reads the text of the message */
        memset(text, 0, TEXT_SIZE);
        int text_size = fread(text, sizeof(char), sizeof(text), fp);
        if(text_size < 0) {
            perror("Error reading from file\n");
            exit(1);
        }
        fclose(fp);
        /* text size of the message */
        char Tsize[TEXT_DIGITS] = "";
        sprintf(Tsize, "%d", text_size);

        constructMessage(message, NULL, UID, Tsize, text);
        sendTCPMessage(fd, message, strlen(message));
        
        memset(message, 0, MESSAGE_SIZE);

        d = opendir(mid_path);
        if(d == NULL) {
            perror("Error opening directory\n");
            exit(1);
        }
        while((dir = readdir(d)) != NULL) { /* searches for other files */
            if(dir->d_name[0]=='.')
                continue;
            if(strcmp(dir->d_name,"A U T H O R.txt") == 0 || strcmp(dir->d_name,"T E X T.txt") == 0) /* ignores the author and text files */
                continue;
            
            char file_path[PATH_MAX_SIZE] = "";
            sprintf(file_path, "%s/%s", mid_path, dir->d_name);
            fp = fopen(file_path,"rb");
            if(fp == NULL) {
                perror("Error opening file\n");
                exit(1);
            }
            fseek(fp, 0, SEEK_END);
            long int file_size = ftell(fp), ntotal = 0;
            ssize_t nread;
            char Fsize[FILE_DIGITS] = "";
            sprintf(Fsize, "%ld", file_size);

            /* sends the file name and size */
            constructMessage(message, " /", dir->d_name, Fsize, NULL);
            strcat(message, " ");
            sendTCPMessage(fd, message, strlen(message));
            memset(message, 0, MESSAGE_SIZE);
            
            fseek(fp, 0, SEEK_SET);
            
            while(ntotal < file_size) { /* while reading the data in the file sends it to the socket */
                nread = fread(message, 1, MESSAGE_SIZE, fp);
                if(nread < 0) {
                    perror("Error reading from file\n");
                    exit(1);
                }
                ntotal += nread;
                sendTCPMessage(fd, message, nread);
                memset(message, 0, MESSAGE_SIZE);
            }
            fclose(fp);
        }
        closedir(d);
        no_msgs--;
        mid++;
    }
    sendTCPMessage(fd, "\n", 1);
}