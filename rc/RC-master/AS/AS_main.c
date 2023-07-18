#include "AS_TCP_Server.h"
#include "AS_UDP_Server.h"
#include "AS_UDP_Client.h"
#include <stdio.h>
#include <dirent.h>
#include <errno.h> 
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h> 
#include <time.h>
#include <math.h>

#define SIZE 100
#define LOGIN "login"
#define REG "reg"
#define PASS "pass"
#define TID "tid"


char message[SIZE];
char asPORT[20];

/*Variaveis que recebe nos sockets*/
char command[20];
char UID[20];
char password[20];

char status[20];
char Fname[100];
char fop[5][20];
char RID[5][20];
char VLC[5][20];
char FNM[5][20];
char loggedUIDs[MAX_CLIENTS][20];

int counter_codes = 0; 

/*Variaveis Ficheiros*/
char fileAddress[30];

/*Flags*/
int flag = 1;
int removeflag = 0;

void reg(char * UID, char * password, char * PDIP, char * PDPort);
void unr(char * UID, char * password);
int verifyPassword(char * pass);
int verifyUID(char * UID);
int verifyFilename(char * filename);
int fileExists(char* filename);

void login(int fd, char * pass);
void req(int fd, char * rid, char * FOP);
void aut(int fd, char * rid, char * vc);
int random_number();

void vld(char * uid, char * tid);

void parsingArguments(int argc, char * argv[]) {
    int option;
    char options[1][2] = {"\0"}; /*guarda as opçoes passadas na inicializaçao do programa 0=d; 1=p; 2=n*/

    while( (option = getopt(argc, argv, "p:v")) != -1) {
        //optarg e gerado e guarda a palavra apos a flag
        switch(option){
            case 'p':
                strcpy(asPORT, optarg);
                strcpy(options[0], "p");
                break;
            case 'v':
                /*Verbose Mode activation*/
                isVerbose = 1;
                break;
            default:
                fprintf(stderr, "Problem parsing arguments!\n");
                killServer();
        }
    }

    /*verifica quais foram as opçoes apanhadas e completa as que nao foram dadas */
    if ( strcmp(options[0], "p") != 0 ) {
        strcpy(asPORT, DEFAULT_AS_PORT); /*mete o IP da maquina actual*/
    }
}

void constructASMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4) {

    strcpy(message, command);
    strcat(message, " ");
    strcat(message, arg1);
    if(arg2!=NULL){
        strcat(message, " ");
        strcat(message, arg2);
    }
    if(arg3!=NULL){
        strcat(message, " ");
        strcat(message, arg3);
    }
    if(arg4!=NULL){
        strcat(message, " ");
        strcat(message, arg4);
    }

    strcat(message, "\n"); /*as mensagens têm que acabar todas com "\n"*/
}

void parsingUDPArguments() {
    char arg1[20];
    char arg2[20];
    char arg3[20];
    char arg4[20];


    readFromUDPSocket();
    sscanf(buffer_udp_server, "%s %s %s %s %s", command, arg1, arg2, arg3, arg4);
    
    //strcmp devolve 0 quando e igual

    if (!strcmp(command, "REG")) {
        reg(arg1, arg2, arg3, arg4);
    }
    else if (!strcmp(command, "UNR")) {
        /*aqui devia sair do programa*/
        unr(arg1, arg2);
    } else if (!strcmp(command, "VLD")) {
        /*aqui devia sair do programa*/
        vld(arg1, arg2);
    }
    else {
        writeToUDPSocket("ERR\n"); // manda ERR para o user, porque não mandou comando válido
    }
}

void parsingTCPArguments(int fd, int index) {
    char arg1[20];
    char arg2[20];
    char arg3[20];
    char arg4[20];
    sscanf(buffer_server_tcp, "%s %s %s %s %s", command, arg1, arg2, arg3, arg4);

    if (!strcmp(command, "LOG")) {
        strcpy(UID, arg1);
        strcpy(password, arg2);
        login(fd, password);
        strcpy(loggedUIDs[index], UID);
        
    }
    else if (!strcmp(command, "REQ")) {
        if(!verifyUID(arg1) || (strlen(arg2) !=4) || (strlen(arg3)!=1) ){
            writeToTCPConnection(fd, "RRQ ERR");
        }
        strcpy(UID, arg1);
        if(!(strcmp(arg3, "R")) || !(strcmp(arg3, "U")) || !(strcmp(arg3, "D"))){
            if(!verifyFilename(arg4)) {
                writeToTCPConnection(fd, "RRQ ERR");
                return;
            }
            else
                strcpy(Fname, arg4);
        }else{
            strcpy(Fname,"\0");
        }
        req(fd, arg2, arg3);
    } else if (!strcmp(command, "AUT")){
        aut(fd, arg2, arg3);
    } else {
        writeToTCPConnection(fd, "ERR\n");// manda ERR para o user, porque não mandou comando válido
    }
}


/**
 * FUNÇOES DA BASE DE DADOS DO AS
 */

char* constructFileAddress(char* UID, char* type) {
    strcpy(fileAddress, "AS/USERS/");
    strcat(fileAddress, UID);
    strcat(fileAddress, "/");
    strcat(fileAddress, UID);
    strcat(fileAddress, "_");
    strcat(fileAddress, type);
    strcat(fileAddress, ".txt");

    return fileAddress;
}

int existsDirectory(char* dirName) {
    struct stat st = {0};
    return (stat(dirName, &st) == 0);
}

void createDirectory(char* dirName) {
    int n;
    n = mkdir(dirName, 0700);

    if (n != 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS creating Directory: ");
        exit(0);
    }
}

/*UID - UserID, type - file type, pass, reg, login, etc*/
void deleteFile(char* UID, char* type) {
    int n;
    n = remove(constructFileAddress(UID, type));
    if (n != 0) {
        if (errno == 2) {
            /*File does not exist, continues the program*/
            return;
        }

        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS deleting File: ");
        exit(0);
    }
}


/*Creates a File*/
void createFile(char* UID, char* type, char * data) {
    char address[30];
    FILE *fp;

    strcpy(address, "AS/USERS");

    /*if USERS directory does not exist*/
    if (!existsDirectory(address)) {
        createDirectory(address); /*creates the directory*/
    }

    /*If the UID directory does not exist*/
    strcat(address, "/");
    strcat(address, UID);
    if (!existsDirectory(address)){
        createDirectory(address); /*creates the directory*/
    }
    
    fp = fopen(constructFileAddress(UID, type), "w");

    if (fp == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS creating File: ");
        exit(0);
    }

    fputs(data, fp);
    fclose (fp);
}

/*Deletes a User's directory from the AS Server (this is called after a remove command from the FS*/
int removeDirectory(char* uid) {
    DIR *d;
    char dirname[100];
    char newDirname[130];
    struct dirent *dir;

    strcpy(dirname, "AS/USERS/");
    strcat(dirname, uid);

    d=opendir(dirname);
    if(d) {
        while((dir=readdir(d)) != NULL) {
            strcpy(newDirname, dirname);
            strcat(newDirname, "/");
            strcat(newDirname, dir->d_name);
            remove(newDirname);
        }
        closedir(d);
        return(1);
    }
    else {
        return(-1);
    }
}


//REG
int verifyUID(char * userid){
    int i;
    if(strlen(userid) == 5){
        for(i = 0; i < 5; i++){
            if(!((userid[i] >= 48) && (userid[i] <= 57))){ //se nao esta no intervalo
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int verifyPassword(char * password){
    int i;
    if(strlen(password) == 8){
        for(i = 0; i < 8; i++){
            if(!(((password[i] >= 48) && (password[i] <= 57)) ||
               ((password[i] >= 65) && (password[i] <= 90)) ||
               ((password[i] >= 97) && (password[i] <= 122)))){
                   return 0;
               }
        }
        return 1;
    }
    return 0;
}

int verifyFilename(char * filename){
    if(strlen(filename) <= 24 && strlen(filename) >= 4){
        for(int i = 0; i < strlen(filename); i++){
            if(!(((filename[i] >= 48) && (filename[i] <= 57)) ||
               ((filename[i] >= 65) && (filename[i] <= 90)) ||
               ((filename[i] >= 97) && (filename[i] <= 122)) ||
               (filename[i] == 95) || (filename[i] == 45) || (filename[i] == 46)))
                   return 0;
               
        }

        if(filename[strlen(filename)-4] != 46)
            return 0;
        
        for(int i = strlen(filename) -1; i > (strlen(filename)-4) ; i--){
            if(!(((filename[i] >= 65) && (filename[i] <= 90)) ||
               ((filename[i] >= 97) && (filename[i] <= 122))))
                return 0;
        }
        return 1;
    }
    return 0;
}

int checkPassword(char * pass){
    char address[128], passwordFile[128], buffer[128];
    FILE * fp;
    int num;

    strcpy(address, "AS/USERS/");
    strcat(address, UID);

    if(existsDirectory(address)){
        strcpy(passwordFile, constructFileAddress(UID, "pass"));
        if (!fileExists(passwordFile)) {
            return -1;
        }
        fp = fopen(passwordFile, "r");
        if( fp == NULL )  {
            return -1;
            
        }
        fseek(fp, 0, SEEK_END);

        num = ftell(fp);
        rewind(fp);
        if (num == 0) { //se o ficheiro pass estiver vazio
            return -1;
        }
        if (num > 0) {
            fgets(buffer, 10, fp);

            if(!strcmp(pass, buffer)){
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
        return 0;
    }
    return -1;
}

int fileExists(char* filename){
    if( access( filename, F_OK ) != -1 ) {
        // file exists
        return 1;
    } else {
        // file doesn't exist
        return 0;
    }
}

void reg(char * UID, char * password, char * PDIP, char * PDPort){
    char PDInfo[30];
    if(verifyUID(UID) && verifyPassword(password)){
        strcpy(PDInfo, PDIP);
        strcat(PDInfo, " ");
        strcat(PDInfo, PDPort);
        createFile(UID, "reg", PDInfo);
        createFile(UID, "pass", password);
        writeToUDPSocket("RRG OK\n");
    } else {
        writeToUDPSocket("RRG NOK\n");
    }
}

void unr(char * uid, char * pass){
    char address[128], passwordFile[128], buffer[128];
    FILE * fp;

    strcpy(address, "AS/USERS/");
    strcat(address, uid);

    if(existsDirectory(address)){
        strcpy(passwordFile, constructFileAddress(uid, "pass"));
        fp = fopen(passwordFile, "r");
        fgets(buffer, 10, fp);
        fclose(fp);
        if(!strcmp(pass, buffer)){
            deleteFile(uid, "reg");
            deleteFile(uid, "tid");
            if (removeflag) {
                removeDirectory(uid);
                rmdir(address);
            }
            writeToUDPSocket("RUN OK\n");
        } else{
            writeToUDPSocket("RUN NOK\n");
        }
    } else{
        writeToUDPSocket("RUN NOK\n");
    }
}

void login(int fd, char * pass){

    if(checkPassword(pass) == 1){
        createFile(UID, "login","");
        writeToTCPConnection(fd, "RLO OK\n");
    } else if(checkPassword(pass)==0){
        writeToTCPConnection(fd, "RLO NOK\n");
    } else if(checkPassword(pass)==-1){
        writeToTCPConnection(fd, "RLO ERR\n");
    }
}

void req(int fd, char * rid, char * FOP){
    int vlc = random_number();
    char vc[5];
    char status[10], uid[10];
    FILE * fp;
    char fileAddress[30];

    sprintf(vc, "%d", vlc);
    if(counter_codes >= 5){
        counter_codes = 0;
    } 
        
    strcpy(VLC[counter_codes], vc);
    strcpy(RID[counter_codes], rid);
    strcpy(fop[counter_codes], FOP);

    strcpy(fileAddress, constructFileAddress(UID, "reg"));
    
    if (!fileExists(fileAddress)) {
        writeToTCPConnection(fd, "ERR USER NOT REGISTERED\n");
        return;
    }
    fp = fopen(fileAddress, "r");
    fscanf(fp, "%s %s", pdip, pdport);
    fclose(fp);
    openUDPClientSocket(pdip, pdport);

    strcpy(message, "VLC ");
    strcat(message, UID);
    strcat(message, " ");
    strcat(message, vc);
    strcat(message, " ");
    strcat(message, FOP);
    if(!(strcmp(FOP, "R")) || !(strcmp(FOP, "U")) || !(strcmp(FOP, "D"))){

        strcat(message, " ");
        strcat(message, Fname);
        strcpy(FNM[counter_codes], Fname);
    }
    counter_codes ++;
    strcat(message, "\n");



    sendMessageByUDPSocket(pdip, pdport, message);
    readFromUDPClientSocket(pdip, pdport);
    if(timeoutflag) {
        writeToTCPConnection(fd, "RRQ EPD\n");
        closeUDPClientSocket();
        return;
    }

    sscanf(buffer_client, "%s %s %s", command, uid, status);
    if(!strcmp(UID, uid)){    
        if(!strcmp(command, "RVC")){
            if(fileExists(constructFileAddress(UID, "login"))){
                if(!strcmp("X", FOP) || !strcmp("D", FOP) || !strcmp("L", FOP) || !strcmp("U", FOP) || !strcmp("R", FOP)){
                    writeToTCPConnection(fd, "RRQ OK\n");
                } else {
                    writeToTCPConnection(fd, "RRQ EFOP\n");
                }
            } else {
                writeToTCPConnection(fd, "RRQ ELOG\n");
            }
        }
    } else {
        writeToTCPConnection(fd, "RRQ EUSER\n");
    }
    closeUDPClientSocket();

}

void aut(int fd, char * rid, char * vc){
    int tid_int;
    char tid[70];
    char tidPRAMANDAR[10];

    for(int i = 0; i < 5; i++){
        if(!strcmp(RID[i], rid)){
            if(!strcmp(VLC[i], vc)){
                tid_int = random_number();
                strcpy(RID[i],"\0");
                strcpy(VLC[i],"\0");

                sprintf(tid, "%d", tid_int);
                sprintf(tidPRAMANDAR, "%d", tid_int);
                strcat(tid, " ");
                strcat(tid, fop[i]);
                if(verifyFilename(FNM[i])){
                    strcat(tid, " ");
                    strcat(tid, FNM[i]);
                    strcpy(FNM[i],"\0");
                }
                strcpy(fop[i],"\0");
                createFile(UID, "tid", tid);
                constructASMessage("RAU", tidPRAMANDAR, NULL,  NULL,  NULL);
                writeToTCPConnection(fd, message);
                return;
            }
        }
    }

    writeToTCPConnection(fd, "RAU 0\n");

}

void vld(char * uid, char * tid){
    FILE * fp;
    char ftid[20], ffop[20], ffname[30];
    char add[25];
    char address[30];

    fp = fopen(constructFileAddress(uid, "tid"), "r");
    fscanf(fp, "%s %s %s", ftid, ffop, ffname);

    strcpy(add, "AS/USERS/");


    strcpy(address, "AS/USERS/");
    strcat(address, uid);

    if(!strcmp(tid, ftid)){
        if(!strcmp(ffop, "X")){
            removeflag = 1;
    
            constructASMessage("CNF", uid, tid, ffop, NULL);
            writeToUDPSocket(message);
            
        } else if((!strcmp(ffop, "R"))||(!strcmp(ffop, "U"))||(!strcmp(ffop, "D"))){
            constructASMessage("CNF", uid, tid, ffop, ffname);
            writeToUDPSocket(message);
            return;
        } else if((!strcmp(ffop, "L"))){
            constructASMessage("CNF", uid, tid, ffop, NULL);
            writeToUDPSocket(message);
            return;
        } 
    } 

    fclose(fp);
    constructASMessage("CNF", uid, tid, "E", NULL);
    writeToUDPSocket(message);
}

void selection(){
    fd_set inputs, testfds;
    int out_fds;
    ssize_t n;
    char uiddummy[50];

    FD_ZERO(&inputs); //Clear inputs
    FD_SET(udp_server_fd, &inputs); // Set socket channel on
    FD_SET(tcp_server_fd, &inputs); // Set socket channel on

    while(1){
        testfds = inputs;   
        out_fds = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, NULL);

        switch (out_fds)
        {

        case -1:
            perror("select");
            exit(1);
        
        default:
            if(FD_ISSET(udp_server_fd,&testfds)){
                parsingUDPArguments();

            } else if(FD_ISSET(tcp_server_fd,&testfds)){
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(tcp_server_connections[i] == -1){
                        acceptNewTCPConnection(i);
                        FD_SET(tcp_server_connections[i], &inputs); // Set socket channel on
                        break;
                    }
                }
               
            } else{
                for(int i = 0; i < MAX_CLIENTS; i++){
                    if(FD_ISSET(tcp_server_connections[i],&testfds)){
                        n = readFromTCPConnection(tcp_server_connections[i], i);
                        if(n > 0){
                            parsingTCPArguments(tcp_server_connections[i], i);
                        } else if(n <= 0){
                            close(tcp_server_connections[i]);
                            tcp_server_connections[i] = -1; 
                            strcpy(uiddummy, loggedUIDs[i]);
                            strcpy(loggedUIDs[i], "\0");
                            deleteFile(uiddummy, "login");

                            FD_ZERO(&inputs); //Clear inputs
                            FD_SET(udp_server_fd, &inputs); // Set socket channel on
                            FD_SET(tcp_server_fd, &inputs); // Set socket channel on
                            for(int j = 0; j < MAX_CLIENTS; j++){
                                if(tcp_server_connections[j] != -1)
                                    FD_SET(tcp_server_connections[j], &inputs); // Set socket channel on
                            }
                        }
                    }
                }
            }
        }
        
    }
}
/**
 * Main Function
 */
void killServer() {
    closeUDPServer();
    closeAllTCPConnections();
    puts("");
    exit(0);
}

int main(int argc, char * argv[]) {
    isVerbose = 0;
    timeoutflag = 0;
    signal(SIGPIPE, SIG_IGN);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        tcp_server_connections[i] = -1;
    }
    srand(time(NULL)); 
    signal(SIGINT, killServer);
    parsingArguments(argc, argv);
    openUDPServer(asPORT);

    openTCPServer(asPORT);
    selection();

    return 0;
}

int random_number(){
    int rand_num;
    do{
        rand_num = rand() % 10000;
    }while(rand_num<=999);

    return rand_num; //prints a random positive integer;
}