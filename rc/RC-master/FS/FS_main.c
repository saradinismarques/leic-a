#include "FS_TCP_Server.h"
#include "FS_UDP_Client.h"
#include <stdio.h>
#include <errno.h> 
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>




/*Para obtenção do IP da máquina*/
char IPBuffer[25]; /*guarda o IP da maquina*/
char hostbuffer[256]; 
struct hostent *host_entry; 
int hostname;


/*Variaveis Ficheiros*/
char fileAddress[SIZE];


char message[500];
char fsPORT[20];
char asPORT[20];
char ASIP[20];

ssize_t n;
int flag = 1;



/*Variaveis que recebe nos sockets*/
char command[20];
char UID[20];
char password[20];
char PDIP[20];
char PDport[20];
char status[20];
char fop[20];
char TID[20];
char FILENAME[SIZE];

/*Declaração de Funções*/
char* getIPAddress();
int ListDir(char *dirname, int modo_contar);
int existsDirectory(char* dirName);
void createDirectory(char* dirName);
void parsingArguments(int argc, char * argv[]);
char* getIPAddress();
void as_fsProtocol();
void constructFSMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4);
void parsingTCPArguments(int fd);
char* constructFileAddress(char* UID, char* filename);
void sendFile(int tcpConnection, char * filename);


/*Funcoes para os comandos*/
void list(int fd, char * uid, char * tid);
void retrieve(int fd, char * uid, char * tid, char * fname);
void del(int fd, char * uid, char * tid, char * fname);
void rem(int fd, char * uid, char * tid);
void upl(int fd, char * uid, char * tid, char * fname, char * fsize);

int sendVLD(char * uid, char * tid);

int verifyUID(char * userid);
int verifyFilename(char * filename);
int verifyTID(char * tid);
int verifyFilesize(char * fsize);

/**
 * ESTAS FUNÇOES TEM QUE SER CORRIGIDAS
 */
void parsingArguments(int argc, char * argv[]) {
    int option;
    char options[4][2] = {"\0", "\0", "\0", "\0"}; /*guarda as opçoes passadas na inicializaçao do programa 0=d; 1=p; 2=n*/

    while( (option = getopt(argc, argv, "q:n:p:v")) != -1) {
        //optarg e gerado e guarda a palavra apos a flag
        switch(option){
            case 'q':
                strcpy(fsPORT, optarg);
                strcpy(options[0], "q");
                break;
            case 'n':
                strcpy(ASIP, optarg);
                strcpy(options[1], "n");
                break;
            case 'p':
                strcpy(asPORT, optarg);
                strcpy(options[2], "p");
                break;
            case 'v':
                /*Verbose Mode activation*/
                isVerbose = 1;
                break;
            default:
                fprintf(stderr, "Problem parsing arguments!\n");
                killServer(); 
            //* acrescentamos isto
        }
    }

    /*verifica quais foram as opçoes apanhadas e completa as que nao foram dadas */
    if ( strcmp(options[0], "q") != 0 ) {
        strcpy(fsPORT, DEFAULT_FS_PORT); /*mete o Default PORT do FS*/
    }
    if ( strcmp(options[1], "n") != 0 ) {
        strcpy(ASIP, getIPAddress()); /*mete o IP da maquina actual*/
    }
    if ( strcmp(options[2], "p") != 0 ) {
        strcpy(asPORT, DEFAULT_AS_PORT); /*mete o Default PORT do AS*/
    }
}

char* getIPAddress() {

    /*retrieve hostname*/ 
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1) { 
        perror("ERROR getting hostname!\n"); 
        killServer(); 
    } 
  
    /*retrieve host information*/ 
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) { 
        perror("host_entry ERROR\n"); 
        killServer(); 
    }

    return inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
}

void constructFSMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4) {

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


void parsingTCPArguments(int fd) {
    char arg1[20];
    char arg2[20];
    char arg3[20];
    char arg4[20];
    
    sscanf(buffer_server_tcp, "%s %s %s %s %s", command, arg1, arg2, arg3, arg4);
    
    if (!strcmp(command, "LST")) {
        if((!verifyUID(arg1))|| (!verifyTID(arg2))){
            writeToTCPConnection(fd, "RLS ERR\n");
        }
        list(fd, arg1, arg2);
    }
    else if (!strcmp(command, "RTV")) {
        if(!verifyUID(arg1)|| !verifyTID(arg2) || !verifyFilename(arg3)){
            writeToTCPConnection(fd, "RRT ERR\n");
        }
        retrieve(fd, arg1, arg2, arg3);
    } 
    
    else if (!strcmp(command, "UPL")) {
        //UPL UID TID Fname Fsize data
        if(!verifyUID(arg1) || !verifyTID(arg2) || !verifyFilename(arg3) || !verifyFilesize(arg4) ){
            writeToTCPConnection(fd, "UPL ERR\n");
        }
        upl(fd, arg1, arg2, arg3, arg4);
    }
    else if (!strcmp(command, "DEL")) {
        if(!verifyUID(arg1)|| !verifyTID(arg2) || !verifyFilename(arg3)){
            writeToTCPConnection(fd, "RDL ERR\n");
        }
        del(fd, arg1, arg2, arg3);
    }
    else if (!strcmp(command, "REM")) {
        if(!verifyUID(arg1)|| !verifyTID(arg2) ){
            writeToTCPConnection(fd, "RRM ERR\n");
        }
        rem(fd, arg1, arg2);
    }
    else {
        writeToTCPConnection(fd, "ERR\n");
    }
}

/**
 * FUNÇOES DA BASE DE FICHEIROS DO FS
 */
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


int verifyTID(char * tid){
  int i;
    if(strlen(tid) == 4){
        for(i = 0; i < 4; i++){
            if(!((tid[i] >= 48) && (tid[i] <= 57))){ //se nao esta no intervalo
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int verifyFilesize(char * fsize){
    for(int i = 0; i < strlen(fsize); i++){
        if(!((fsize[i] >= 48) && (fsize[i] <= 57))){ //se nao esta no intervalo
            return 0;
        }
    }
    return 1;

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

char* constructFileAddress(char* UID, char* filename) {
    strcpy(fileAddress, "FS/USERS/");
    strcat(fileAddress, UID);
    strcat(fileAddress, "/");
    strcat(fileAddress, filename);

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
        perror("Error FS creating Directory: ");
        killServer();
    }
}

//UID - UserID, type - file type, pass, reg, login, etc
void deleteFile(char* uid, char* fname) {
    int n;
    char address[30];
    strcpy(address, "FS/USERS/");
    strcat(address, uid);
    strcat(address, "/");
    strcat(address, fname);

    n = remove(address);

    if (n != 0) {
        if (errno == 2) {
            /*File does not exist, continues the program*/
            return;
        }

        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS deleting File: ");
        killServer();
    }
}

void sendFile(int tcpConnection, char * filename) {
    FILE * fp;
    long int sz, sizeRead = 0;
    char fileSize [SIZE], buffer[SIZE];
    ssize_t m;

    strcpy(FILENAME, filename);
    
    fp = fopen(FILENAME, "r");
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    sprintf(fileSize, "%ld", sz);

    constructFSMessage("UPL", UID, TID, filename, fileSize);
    strcat(message, " ");

    writeToTCPConnection(tcpConnection, message);
    while(sizeRead < sz){

        n = fread(buffer, 1, SIZE, fp);
        sizeRead += n;
        m=write(tcpConnection, buffer, n);
        if(m==-1){/*error*/
            perror("write ERROR");
            killServer();
        }
    }
    fclose(fp);
}

// A typical directory listing function.
/**
 * CORRIGIR ESTA FUNÇAO
 * TEM QUE ENVIAR PARA O USER MAIS INFORMAÇAO
 * BYTE SIZE DE CADA FICHEIRO
*/

int ListDir(char *dirname, int modo_contar) {
    DIR *d;
    char messageDummy[300];
    char filesBuffer[500];
    int numOfFiles = 0;
    char numFiles[4];
    struct stat sb;
    struct dirent *dir;
    //* OK AFINAL PERCEBEMOS MAS VERIFICAR NA MESMA

    d=opendir(dirname);
    if(d) {
        while((dir=readdir(d)) != NULL) {
            stat(dir->d_name, &sb);
    
            if( (!strcmp(dir->d_name, ".")) || (!strcmp(dir->d_name, "..")) ) {
                continue;
            }
            sprintf(messageDummy, "%s %lld", dir->d_name, (long long) sb.st_size );
            if (numOfFiles == 0) {
                strcpy(filesBuffer, messageDummy);
            }
            else {
                strcat(filesBuffer, " "); //podemos mudar para \n
                strcat(filesBuffer, messageDummy);
            }
             
            numOfFiles++;
        }
        closedir(d);
        if(modo_contar){
            return numOfFiles;
        }
        if (numOfFiles > 0) {
            sprintf(numFiles, "%d", numOfFiles);
            strcpy(message, "RLS ");
            strcat(message, numFiles);
            strcat(message, " ");
            strcat(message, filesBuffer);
            strcat(message, "\n");
            return(1);
        }
        else {
            return -1;
        }
    }
    else {
        return(-1);
    }
}

int removeDir(char * uid, char *dirname) {
    DIR *d;
    //struct stat sb;
    struct dirent *dir;
    char newDirname[130];
    //* OK AFINAL PERCEBEMOS MAS VERIFICAR NA MESMA
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

/*Funcoes para os comandos*/

/**/
void list(int fd, char * uid, char * tid){
    char address[20];
    int n_list;
    if(!sendVLD(uid, tid)){
        writeToTCPConnection(fd, "RLS INV\n");
        return;
    }
    strcpy(address,"FS/USERS/");
    strcat(address, uid);

    n_list =  ListDir(address, 0);
    if(n_list == -1) {
        writeToTCPConnection(fd, "RLS EOF\n");
        return;
    }
    else if (n_list == 1) {
        writeToTCPConnection(fd, message);
        return;
    }
    
}

void retrieve(int fd, char * uid, char * tid, char * fname){
    FILE * fp;
    int num;
    long int sz = 0, sizeRead = 0;
    char fileSize [SIZE], buffer[SIZE];
    char address[30];

    if(!sendVLD(uid, tid)){
        writeToTCPConnection(fd, "RRT INV\n");
        return;
    }
    strcpy(address, "FS/USERS/");
    strcat(address, uid);

    if (!fileExists(address)) {
        writeToTCPConnection(fd, "RRT EOF\n");
        return;
    }
    


    if (!existsDirectory(address)) {
        writeToTCPConnection(fd, "RRT NOK\n");
        return;
    }
    
    strcat(address, "/");
    strcat(address, fname);

    fp = fopen(address, "r");
    fseek(fp, 0L, SEEK_END);

    sz = ftell(fp);
    rewind(fp);
    sprintf(fileSize, "%ld", sz);

    strcpy(message, "RRT OK ");
    strcat(message, fileSize);
    strcat(message, " ");

    writeToTCPConnection(fd, message);

    while(sizeRead < sz){

        num = fread(buffer, 1, 50, fp);
        sizeRead += num;

        write(fd,buffer,num);
    }
    write(fd,"\n",1);
    fclose(fp);
}

void del(int fd, char * uid, char * tid, char * fname) {
    char address[30];

    if(!sendVLD(uid, tid)){
        writeToTCPConnection(fd, "RDL INV\n");
        return;
    }
    strcpy(address, "FS/USERS/");
    strcat(address, uid);

    if (!fileExists(address)) {
        writeToTCPConnection(fd, "RDL EOF\n");
        return;
    }
    
    if (!existsDirectory(address)) {
        writeToTCPConnection(fd, "RDL NOK\n");
        return;
    }

    deleteFile(uid, fname);
    writeToTCPConnection(fd, "RDL OK\n");

}

void rem(int fd, char * uid, char * tid) {
    char address[30];

    if(!sendVLD(uid, tid)){
        writeToTCPConnection(fd, "RRM INV\n");
        return;
    }
    strcpy(address, "FS/USERS/");
    strcat(address, uid);
    
    if (!existsDirectory(address)) {
        writeToTCPConnection(fd, "RRM NOK\n");
        return;
    }

    removeDir(uid, address);

    n = rmdir(address);
    if (n != 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS removing Directory: ");
        exit(0);
    }
    
    writeToTCPConnection(fd, "RRM OK\n");
}

void upl(int fd, char * uid, char * tid, char * fname, char * fsize){
    int len[4];
    int global_len;
    int fsize_int;
    int read_size = 0;
    FILE * fp;
    char data[300];
    char weird_buffer[128];
    char address[30];
    if(!sendVLD(uid, tid)){
        writeToTCPConnection(fd, "RUP INV\n");
        return;
    }


    fsize_int = atol(fsize);
    strcpy(address, "FS/USERS");
    if(!existsDirectory(address)){
        createDirectory(address);
    }
    strcat(address, "/");
    strcat(address, uid);
    if(!existsDirectory(address)){
        createDirectory(address);
    }
    if(ListDir(address, 1)==15){
        writeToTCPConnection(fd, "RUP FULL\n");
        return;
    }
    strcat(address, "/");
    strcat(address, fname);
    if(fileExists(address)){
        writeToTCPConnection(fd, "RUP DUP\n");
        return;
    }

    writeToTCPConnection(fd, "RUP OK\n");
    
    fp = fopen(constructFileAddress(uid, fname), "w");

    len[0] = strlen(uid);
    len[1] = strlen(tid);
    len[2] = strlen(fname);
    len[3] = strlen(fsize);
    global_len = len[0] + len[1] + len[2] + len[3] + 8;

    for(int i= global_len; i < n_bytes_upload - global_len; i++){
        weird_buffer[i-global_len] = buffer_server_tcp[i];
    }
    fwrite(weird_buffer, 1, n_bytes_upload - global_len, fp);
    read_size = n_bytes_upload - global_len;
    while (read_size < fsize_int)
    {
        n= read(fd,data,300);
        read_size += n;

        fwrite(data, 1, n, fp);

        if(n == -1 ){
            perror("trouble with reading: ");
        }
    }
    fclose(fp);
    
}

int sendVLD(char * uid, char * tid){
    char t_command[20];
    char t_uid[20];
    char t_tid[20];
    char t_fop[20];
    char t_fname[30];
    openUDPClientSocket(ASIP, asPORT);
    constructFSMessage("VLD", uid, tid, NULL, NULL);
    sendMessageByUDPSocket(ASIP, asPORT, message);
    readFromUDPClientSocket(ASIP, asPORT);
    
    sscanf(buffer_udp_client, "%s %s %s %s %s", t_command, t_uid, t_tid, t_fop, t_fname);

    closeUDPClientSocket();
    if(!strcmp(t_fop, "E"))
        return 0;
    
    return 1;
}

/*funcoes principais*/
void selection(){
    fd_set inputs, testfds;
    int out_fds;
    ssize_t n;

    FD_ZERO(&inputs); //Clear inputs
    FD_SET(tcp_server_fd, &inputs); // Set socket channel on
    while(1){
        testfds = inputs;
        
        out_fds = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, NULL);

        switch (out_fds)
        {

        case -1:
            perror("select");
            killServer();
        
        default:
            if(FD_ISSET(tcp_server_fd,&testfds)){
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
                        n = readFromTCPConnection(tcp_server_connections[i]);
                        if(n > 0){
                            parsingTCPArguments(tcp_server_connections[i]);
                        } else if (n==0){
                            close(tcp_server_connections[i]);
                            tcp_server_connections[i] = -1;
                            FD_ZERO(&inputs); //Clear inputs
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
void killServer() {
    closeAllTCPConnections();
    puts("");
    exit(0);
}

int main(int argc, char * argv[]) {
    isVerbose = 0;
    signal(SIGPIPE, SIG_IGN);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        tcp_server_connections[i] = -1;
    }
    signal(SIGINT, killServer);
    parsingArguments(argc, argv);
    openTCPServer(fsPORT);
    selection();

    return 0;
}