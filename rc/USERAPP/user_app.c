#include "user_app.h"
#include "../validation.c"
#include "user_udp.c"
#include "user_tcp.c"

int main(int argc, char *argv[]) {
    
    parseArguments(argc, argv);

    while(applyCommands()) {}

    return 0;
}

char *getIPAddress() {
    int hostname;
    char hostbuffer[BUFFER_SIZE];
    struct hostent *host_entry;

    /* retrieve hostname */
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if(hostname == -1) {
        perror("Error getting hostname\n");
        exit(1);
    }

    /* retrieve host information */
    host_entry = gethostbyname(hostbuffer);
    if(host_entry == NULL) {
        perror("Error host_entry\n");
        exit(1);
    }
    return inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
}

void parseArguments(int argc, char * argv[]) {
    int option;
    char options[2][2] = {"", ""}; /* saves options given when running the program 0=n; 1=p */

    while((option = getopt(argc, argv, "n:p:")) != -1) {
        switch(option){
            case 'n':
                strcpy(DSIP, optarg);
                strcpy(options[0], "n");
                break;
            case 'p':
                strcpy(DSport, optarg);
                strcpy(options[1], "p");
                break;
            default:
                perror("Error parsing arguments\n");
                exit(1);
        }
    }
    /* checks given options and completes them if not passed */
    if(strcmp(options[0], "n") != 0) {
        strcpy(DSIP, getIPAddress()); /* current machine IP */
    }
    if(strcmp(options[1], "p") != 0) {
        strcpy(DSport, DEFAULT_DS_PORT); 
    }
}

int applyCommands() {
    char buffer[BUFFER_SIZE] = "";
    char arg1[ARG1_SIZE] = "";
    char arg2[ARG2_SIZE] = "";
    char command[COMMAND_SIZE] = "";

    fgets(buffer, BUFFER_SIZE, stdin);
    sscanf(buffer, "%s", command);

    if(strcmp(command, "post") == 0) { /* on the command post we need to ignore spaces between "" */
        sscanf(buffer, "%s \"%[^\"]\" %s", command, arg1, arg2);
        if(validText(arg1) && validFName(arg2))
            post(arg1, arg2);
    }
    sscanf(buffer, "%s %s %s", command, arg1, arg2);
    
    if(strcmp(command, "reg") == 0){
        if(validUID(arg1) && validPass(arg2)) 
            reg(arg1, arg2);

    } else if(strcmp(command, "unregister") == 0 || strcmp(command, "unr") == 0){
        if(validUID(arg1) && validPass(arg2)) 
            unregister(arg1, arg2);
        
    } else if(strcmp(command, "login") == 0) {
        if(validUID(arg1) && validPass(arg2))
            login(arg1, arg2);

    } else if(strcmp(command, "logout") == 0) {
        logout(); 

    } else if(strcmp(command, "showuid") == 0 || strcmp(command, "su") == 0) {
        showuid();

    } else if(strcmp(command, "exit") == 0) {
        if(isLoggedIn)
            logout(); /* user logout before terminating program */
        printf("Exiting user app...\n");
        return EXIT;

    } else if(strcmp(command, "groups") == 0 || strcmp(command, "gl") == 0) {
        groups();

    } else if(strcmp(command, "subscribe") == 0 || strcmp(command, "s") == 0) {
        if(validGID(arg1) && validGName(arg2))
            subscribe(arg1, arg2);

    } else if(strcmp(command, "unsubscribe") == 0 || strcmp(command, "u") == 0) {
        if(validGID(arg1))
            unsubscribe(arg1);

    } else if(strcmp(command, "my_groups") == 0 || strcmp(command, "mgl") == 0) {
        my_groups();

    } else if(strcmp(command, "select") == 0 || strcmp(command, "sag") == 0) {
        if(validGID(arg1))
            select_(arg1);

    } else if(strcmp(command, "showgid") == 0 || strcmp(command, "sg") == 0) {
        showgid();

    } else if(strcmp(command, "ulist") == 0 || strcmp(command, "ul") == 0) {
        ulist();

    } else if(strcmp(command, "retrieve") == 0 || strcmp(command, "r") == 0) {
        if(validMID(arg1))
            retrieve(arg1);
    } 
    return CONTINUE;
}

void constructMessage(char *message, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6, char *arg7) {
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
    if(arg5 != NULL) {
        strcat(message, " ");
        strcat(message, arg5);
    }
    if(arg6 != NULL) {
        strcat(message, " ");
        strcat(message, arg6);
    }
    if(arg7 != NULL) {
        strcat(message, " ");
        strcat(message, arg7);
    }
    if(strcmp(arg1, "PST") == 0) /* no caso do post envia a medida que vai lendo por isso nao pomos logo o \n */
        return;
    
    strcat(message, "\n"); 
}

void checkReceivedMessage(int fd, char *message) {
    char *arg2, *arg3, *arg4;
    char buffer[BUFFER_SIZE] = "";

    char *command = strtok(message, " \n");
    char *arg1 = strtok(NULL, " \n");
    if(strcmp(command, "ERR") == 0) {
        printf("Something went wrong...\n");
    } else if(strcmp(command, "RRG") == 0) { /* register reply */
        if(strcmp(arg1, "OK") == 0)
            printf("User successfully registered\n");
        else if(strcmp(arg1, "DUP") == 0)
            printf("ERROR: This user already exists\n");
        else    
            printf("ERROR: Too many users registered\n");
    } else if(strcmp(command, "RUN") == 0) { /* unregister reply */
        if(strcmp(arg1, "OK") == 0)
            printf("User successfully unregistered\n");
        else 
            printf("ERROR: This user does not exist or is not logged in or password provided is incorrect\n");
    } else if(strcmp(command, "RLO") == 0) { /* login reply */
        if(strcmp(arg1, "OK") == 0) {
            isLoggedIn = 1;
            printf("User successfully logged in\n");
        } else 
            printf("ERROR: This user does not exist or password provided is incorrect\n");
    } else if(strcmp(command, "ROU") == 0) { /* logout reply */
        if(strcmp(arg1, "OK") == 0)
            printf("User successfully logged out\n");
        else 
            printf("ERROR: This user does not exist or is not logged in or password provided is incorrect\n");
    } else if(strcmp(command, "RGL") == 0) { /* groups reply */
        if(strcmp(arg1, "0") == 0)
            printf("There are no groups\n");
        else {
            int N = atoi(arg1);
            printf("There are %d groups:\n", N);
            
            while(N > 0) { /* reads all the groups information */
                arg2 = strtok(NULL, " ");
                arg3 = strtok(NULL, " ");
                arg4 = strtok(NULL, " \n");
                printf("GID: %s, Group Name: %s, Number of Messages: %s\n", arg2, arg3 ,arg4);
                N--;
            }
        }
    } else if(strcmp(command, "RGS") == 0) { /* subscribe reply */
        if(strcmp(arg1, "OK") == 0)
            printf("User successfully subscribed\n");
        else if(strcmp(arg1, "E_USR") == 0)
            printf("ERROR: This user does not exist\n");
        else if(strcmp(arg1, "E_FULL") == 0)
            printf("ERROR: Too many groups\n");
        else if(strcmp(arg1, "E_GRP") == 0)
            printf("ERROR: This group does not exist\n");
        else if(strcmp(arg1, "E_GNAME") == 0)
            printf("ERROR: Wrong group name\n");
        else if(strcmp(arg1, "NEW") == 0)
            printf("Group successfully created\n");
        else    
            printf("ERROR: Unsuccessful subscription\n");
    } else if(strcmp(command, "RGU") == 0) { /* unsubscribe reply */
        if(strcmp(arg1, "OK") == 0)
            printf("User successfully unsubscribed\n");
        else if(strcmp(arg1, "E_USR") == 0)
            printf("ERROR: This user does not exist\n");
        else if(strcmp(arg1, "E_GRP") == 0)
            printf("ERROR: This group does not exist\n");
        else    
            printf("ERROR: This user is not subscribed to that group\n");
    } else if(strcmp(command, "RGM") == 0) { /* my_groups reply */
        if(strcmp(arg1, "E_USR") == 0)
            printf("ERROR: This user does not exist\n");
        else if(strcmp(arg1, "0") == 0)
            printf("User not subscribed to any groups\n");
        else {
            int N = atoi(arg1);
            printf("User subscribed to %d groups:\n", N);
            while(N > 0) { /* reads all the groups information the users is subscribed to */
                arg2 = strtok(NULL, " ");
                arg3 = strtok(NULL, " ");
                arg4 = strtok(NULL, " \n");
                printf("GID: %s, Group Name: %s, Number of Messages: %s\n", arg2, arg3 ,arg4);
                N--;
            }
        }
    } else if(strcmp(command, "RUL") == 0) { /* ulist reply */
        if(strcmp(arg1, "OK") == 0) {
            char GName[GNAME_SIZE] = "";
            while(strcmp(buffer, " ") != 0) {
                memset(buffer, 0, BUFFER_SIZE); 
                readTCPMessage(fd, buffer, 1);
                if(strcmp(buffer, "\n") == 0)
                    break;
                strcat(GName, buffer);
            }
            printf("Group name: %s\n", GName);
            if(strcmp(buffer, "\n") == 0) {
                printf("This group does not have any users subscribed\n");
                return;
            }
            memset(buffer, 0 , BUFFER_SIZE);
            char UID[UID_SIZE] = "";

            while(1) { /* reads all the UIDs subscribed to that group */
                readTCPMessage(fd, UID, 5);
                printf("User ID: %s\n", UID);
                readTCPMessage(fd, buffer, 1);
                if(strcmp(buffer, "\n") == 0) 
                    break;
            }
        } else 
            printf("ERROR: This group does not exist\n");
    } else if(strcmp(command, "RPT") == 0) { /* post reply */
        if(strcmp(arg1, "NOK") == 0)
            printf("ERROR: This user or group does not exist or this user is not subscribed to this group\n");
        else 
            printf("Successful post. Message ID: %s\n", arg1);
    } else if(strcmp(command, "RRT") == 0) { /* retrieve reply */
        if(strcmp(arg1, "EOF") == 0) 
            printf("There are no messages in this group\n");
        else if(strcmp(arg1, "NOK") == 0)
            printf("ERROR: This user or group or this message does not exist or this user is not subscribed to this group\n");
        else {
            int has_file = 1;
            char UID[UID_SIZE] = "";
            char MID[MID_SIZE] = "";
            char MID_aux[MID_SIZE] = "";
            char aux[FILE_DIGITS] = ""; 

            /* reads N - number of messages retrieved */
            readTCPMessage(fd, buffer, 2);

            int N = atoi(buffer);
            printf("There are %d messages to read:\n", N);
            printf("--------------------------------------\n");

            if (N > 10) 
                readTCPMessage(fd, buffer, 1);

            for(int i = 0; i < N; i++) { /* retrieves each N message */
                /* reads MID and UID */
                if(has_file == 0) { /* if there is no file some bytes of MID were already read bellow */
                    readTCPMessage(fd, MID_aux, 2);
                    readTCPMessage(fd, buffer, 1);
                    readTCPMessage(fd, UID, 5);
                    readTCPMessage(fd, buffer, 1);
                    strcat(MID, MID_aux);
                }
                else {
                    readTCPMessage(fd, MID_aux, 4);
                    readTCPMessage(fd, buffer, 1);
                    readTCPMessage(fd, UID, 5);
                    readTCPMessage(fd, buffer, 1);
                    strcpy(MID, MID_aux);
                }
                printf("Message ID: %s\nUser ID: %s\n", MID, UID);
                
                memset(aux, 0, FILE_DIGITS);;

                /* reads text size */
                while(1) {
                    readTCPMessage(fd, buffer, 1);

                    if(strcmp(buffer, " ") == 0) 
                        break;
                    strcat(aux, buffer);
                }

                int Tsize = atoi(aux);
                printf("Text size: %d\n", Tsize);

                /* reads text */
                readTCPMessage(fd, buffer, Tsize);

                printf("Text: %s\n", buffer);

                /* reads / that separates file information if it exists in the message */
                readTCPMessage(fd, buffer, 1);
                if(strcmp(buffer, "\n") != 0) /* if buffer is \n it is the last message and has no text */
                    readTCPMessage(fd, MID, 2);

                if(strcmp(MID, "/ ") != 0 || strcmp(buffer, "\n") == 0) { /* no file in this message or is the last message */
                    has_file = 0;
                    printf("--------------------------------------\n");
                } else { /* there is a file in this message */
                    has_file = 1;
                    memset(aux, 0, 11);;

                    /* reads file name */
                    while(1) {
                        readTCPMessage(fd, buffer, 1);

                        if(strcmp(buffer, " ") == 0) 
                            break;
                        strcat(aux, buffer);
                    }
                    printf("File Name: %s\n", aux);
                    memset(aux, 0, FILE_DIGITS);

                    /* reads file size */
                    while(1) {
                        readTCPMessage(fd, buffer, 1);

                        if(strcmp(buffer, " ") == 0) 
                            break;
                        strcat(aux, buffer);
                    }
                    long int file_size = atoi(aux);
                    printf("File Size: %ld\n", file_size);

                    /* reads file data */
                    long int nleft = file_size;
                    int nread = BUFFER_SIZE;
                    printf("File content:\n");

                    while(nleft > 0) { /* while is reading from socket prints data */
                        if(nleft < BUFFER_SIZE)
                            nread = nleft;
                        readTCPMessage(fd, buffer, nread);
                        fwrite(buffer, 1, nread, stdout);
                        nleft -= BUFFER_SIZE;
                    }
                    readTCPMessage(fd, buffer, 1); 
                    printf("\n--------------------------------------\n");
                }
            }
        }
    }
}

/* -------------------------------------- command functions -------------------------------------- */
/* UDP */
void reg(char *UID, char *pass) {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    constructMessage(message, "REG", UID, pass, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void unregister(char *UID, char *pass) {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    constructMessage(message, "UNR", UID, pass, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void login(char *UID, char *pass) {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    strcpy(UID_login, UID);
    strcpy(pass_login, pass);

    constructMessage(message, "LOG", UID_login, pass_login, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void logout() {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    constructMessage(message, "OUT", UID_login, pass_login, NULL, NULL, NULL, NULL);
    strcpy(UID_login, "");
    strcpy(pass_login, "");
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);

    isLoggedIn = 0;
}

void showuid() {
    if(strcmp(UID_login, "") == 0)
        printf("No user logged in\n");
    else
        printf("Current user logged in: %s\n", UID_login);
}

void groups() {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[LIST_SIZE] = "";

    constructMessage(message, "GLS", NULL, NULL, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void subscribe(char *GID, char *GName) {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[LIST_SIZE] = "";
    
    if(strcmp(GID, "0") == 0)
        strcat(GID, "0");

    constructMessage(message, "GSR", UID_login, GID, GName, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void unsubscribe(char *GID) {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    constructMessage(message, "GUR", UID_login, GID, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void my_groups() {
    int fd = connectToUDPSocket();
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";

    constructMessage(message, "GLM", UID_login, NULL, NULL, NULL, NULL, NULL);
    sendUDPMessage(fd, message);
    if(!readUDPMessage(fd, buffer)) {
        sendUDPMessage(fd, message);
        if(!readUDPMessage(fd, message)) 
            strcpy(buffer, "ERR\n");
    }
    checkReceivedMessage(fd, buffer);
    closeUDPSocket(fd);
}

void select_(char *GID) {
    strcpy(GID_selected, GID);
    printf("Successfuly selected group %s\n", GID);
}

void showgid() {
    if(strcmp(GID_selected, "") == 0) {
        printf("Please select a group first\n");
        return;
    }
    printf("Current group: %s\n", GID_selected);
}

/* TCP */
void ulist() {
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    int fd = connectToTCPSocket();

    if(strcmp(GID_selected, "") == 0) {
        printf("Please select a group first\n");
        return;
    }
    constructMessage(message, "ULS", GID_selected, NULL, NULL, NULL, NULL, NULL);
    sendTCPMessage(fd, message, strlen(message));
    readTCPMessage(fd, buffer, 7); 
    checkReceivedMessage(fd, buffer);
    closeTCPSocket(fd);
}

void post(char *text, char *FName) {
    char message[MESSAGE_SIZE] = "";
    char buffer[BUFFER_SIZE] = "";
    char t_size[TEXT_DIGITS] = "";
    int fd = connectToTCPSocket();
    
    sprintf(t_size,"%ld",strlen(text));

    if(strcmp(GID_selected, "") == 0) {
        printf("Please select a group first\n");
        return;
    }

    if(strcmp(FName, "") == 0) { /* post sem ficheiro */
        constructMessage(message, "PST", UID_login, GID_selected, t_size, text, NULL, NULL);
        strcat(message, "\n");
        sendTCPMessage(fd, message, strlen(message));
        readTCPMessage(fd, buffer, -1); 
        checkReceivedMessage(fd, buffer);
        closeTCPSocket(fd); 
        return;
    }
    /* post com ficheiro */
    FILE *fp = fopen(FName, "r");
    if(fp == NULL) {
        perror("Error opening file\n");
        exit(1);
    }
    long int file_size, ntotal = 0;
    char f_size[FILE_DIGITS] = "";
    ssize_t nread;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    sprintf(f_size, "%ld", file_size);
    constructMessage(message, "PST", UID_login, GID_selected, t_size, text, FName, f_size);
    strcat(message, " ");
    sendTCPMessage(fd, message, strlen(message));
    fseek(fp, 0, SEEK_SET);

    while(ntotal < file_size) { /* while is reading from file sents data to server */
        nread = fread(buffer, 1, BUFFER_SIZE, fp);
        ntotal += nread;
        sendTCPMessage(fd, buffer, nread);
        memset(buffer, 0, BUFFER_SIZE);
    }
    fclose(fp);
    sendTCPMessage(fd, "\n", 1);
    readTCPMessage(fd, buffer, -1);
    checkReceivedMessage(fd, buffer);
    closeTCPSocket(fd);
}

void retrieve(char *MID) {
    char buffer[BUFFER_SIZE] = "";
    char message[MESSAGE_SIZE] = "";
    int fd = connectToTCPSocket();
    
    if(strcmp(GID_selected, "") == 0) {
        printf("Please select a group first\n");
        return;
    }
    constructMessage(message, "RTV", UID_login, GID_selected, MID, NULL, NULL, NULL);
    sendTCPMessage(fd, message, strlen(message));

    /* reads RRT status */
    readTCPMessage(fd, buffer, 7);
    checkReceivedMessage(fd, buffer);
    closeTCPSocket(fd);
}

