#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/stat.h> 
#include <signal.h>

#define TEJO "tejo.tecnico.ulisboa.pt"
#define PORT "58011"
#define SIZE 100
#define GN 10 /*Numero do grupo*/
#define DEFAULT_PD_PORT "57010"
#define DEFAULT_AS_PORT "58010"
#define DEFAULT_FS_PORT "59000"

char IPBuffer[25]; /*guarda o IP da maquina*/
char hostbuffer[256]; 
struct hostent *host_entry; 
int hostname;

int errcode, as, fs;

ssize_t n;
socklen_t addrlenAS, addrlenFS;
struct addrinfo hintsAS, *resAS, hintsFS, * resFS;
struct sockaddr_in addrAS, addrFS;

char buffer[500];
char command[20];
char message[SIZE];
char FILENAME[SIZE];
 
char asip[20], asPORT[20], fsip[20], fsPORT[20];

int a;
int flag = 1;

char UID[10];
char rid [6];
char tid [6];
char filesize[6];

int fs_channel_on = 0;

/*Variaveis Ficheiros*/
char fileAddress[30];

int connectToASSocket();
int connectToFSSocket();
void closeSocket(int fd, struct addrinfo *res);
void sendMessage(int fd, char * message);
void readFromSocket(int fd);

void login(char* uid, char* pass); //AS
void req(char * fop, char * fname); //AS
void val(char * uid, char * rid, char* vc); //AS
void list();
void retrieve(char * filename);
void upload(char * filename);
void delete(char * filename);
void remove_x();
void exit_user();

int random_number();

void constructUserMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4) {

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
    if(strcmp(command, "UPL")){
        strcat(message, "\n"); /*as mensagens têm que acabar todas com "\n"*/
    }
}

void parsingSTDinArguments() {
    char buffer[100];
    char arg1[20];
    char arg2[20];

    fgets(buffer, SIZE, stdin);
    sscanf(buffer, "%s %s %s", command, arg1, arg2);

    //printf("command: %s\tUID: %s\tpass: %s\n", command, uid, pass);
    //strcmp devolve 0 quando e igual
    if (!strcmp(command, "login")) {
        login(arg1, arg2);
        sendMessage(as, message);
    }
    else if (!strcmp(command, "req")) {
        req(arg1, arg2);
        sendMessage(as, message);
    } 
    else if (!strcmp(command, "val")) {
        val(UID, rid, arg1); //recebe VC no arg1
        sendMessage(as, message);
    } 
    else if (!strcmp(command, "list") || !strcmp(command, "l")) {
        fs = connectToFSSocket();
        list();
        sendMessage(fs, message);
    } 
    else if (!strcmp(command, "retrieve") || !strcmp(command, "r")) {
        fs = connectToFSSocket();
        retrieve(arg1);
        sendMessage(fs, message);
    } 
    else if (!strcmp(command, "upload") || !strcmp(command, "u")) {
        fs = connectToFSSocket();
        upload(arg1);
    } 
    else if (!strcmp(command, "delete") || !strcmp(command, "d")) {
        fs = connectToFSSocket();
        delete(arg1);
        sendMessage(fs, message);
    } 
    else if (!strcmp(command, "remove") || !strcmp(command, "x")) {
        fs = connectToFSSocket();
        remove_x();
        sendMessage(fs, message);
    }
    else if (!strcmp(command, "exit")) {
        exit_user();
    }
    else {
        perror("ERROR command received in stdin invalid\n");
        exit(1);
    }
}

void selection(){
    fd_set inputs, testfds;
    int out_fds;
    char reply[50];
    char arg[50];

    FD_ZERO(&inputs); //Clear inputs
    FD_SET(0, &inputs); // Set standard input channel on
    FD_SET(as, &inputs); // Set socket channel on
    FD_SET(fs, &inputs); // Set socket channel on
    while(1){
        testfds = inputs;

        //printf("testfds byte: %d\n", ((char *)&testfds)[0]);

        out_fds = select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, NULL);
        //int select (int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);

        //printf("Time = %d and %d\n", ((char *)&testfds)[0]);
        //printf("testfds byte: %d\n", ((char *)&testfds)[0]);

        switch (out_fds)
        {

        case -1:
            perror("select");
            exit(1);
        
        default:
            if(FD_ISSET(0,&testfds)){
                parsingSTDinArguments();

            } else if(FD_ISSET(as,&testfds)){
                readFromSocket(as);
                sscanf(buffer, "%s %s", reply, arg);

                if(!strcmp(reply, "RAU")){
                    strcpy(tid, arg);
                }

            } else if(FD_ISSET(fs,&testfds)){
                readFromSocket(fs);
                closeSocket(fs, resFS);
            }
        }

        if(!flag)
            break;

            
        FD_ZERO(&inputs); //Clear inputs
        FD_SET(0, &inputs); // Set standard input channel on
        FD_SET(as, &inputs); // Set socket channel on
        if(fs_channel_on)
            FD_SET(fs, &inputs); // Set socket channel on
    }
}

char* getIPAddress() {

    /*retrieve hostname*/ 
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    if (hostname == -1) { 
        perror("ERROR getting hostname!\n"); 
        exit(1); 
    } 
  
    /*retrieve host information*/ 
    host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL) { 
        perror("host_entry ERROR\n"); 
        exit(1); 
    }

    return inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
}

void parsingArguments(int argc, char * argv[]) {
    int option;
    char options[4][2] = {"\0", "\0", "\0", "\0"}; /*guarda as opçoes passadas na inicializaçao do programa 0=d; 1=p; 2=n*/

    while( (option = getopt(argc, argv, "n:p:m:q:")) != -1) {
        //optarg e gerado e guarda a palavra apos a flag
        switch(option){
            case 'p':
                strcpy(asip, optarg);
                strcpy(options[0], "p");
                break;
            case 'n':
                strcpy(asPORT, optarg);
                strcpy(options[1], "n");
                break;
              case 'm':
                strcpy(fsip, optarg);
                strcpy(options[2], "m");
                break;
            case 'q':
                strcpy(fsPORT, optarg);
                strcpy(options[3], "q");
                break;
            default:
                fprintf(stderr, "Problem parsing arguments!\n");
                exit(1); 
        }
    }

    /*verifica quais foram as opçoes apanhadas e completa as que nao foram dadas */
    if ( strcmp(options[0], "p") != 0 ) {
        strcpy(asip, getIPAddress()); /*mete o IP da maquina actual*/
    }
    if ( strcmp(options[1], "n") != 0 ) {
        strcpy(asPORT, DEFAULT_AS_PORT);
    }
    if ( strcmp(options[2], "m") != 0 ) {
        strcpy(fsip, getIPAddress());  /*mete o IP da maquina actual*/
    }  
    if ( strcmp(options[3], "q") != 0 ) {
        strcpy(fsPORT, DEFAULT_FS_PORT);
    }
    //printf("ASIP: %s\tasPORT: %s\t FSIP: %s\tFSport: %s\n", asip, asPORT, fsip, fsPORT);
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

FILE * openFile(char * filename){
/*if USERS directory does not exist*/
    char address[30];
    FILE *fp;

    strcpy(address, "USER_FILES");
    if (!existsDirectory(address)) {
        createDirectory(address); /*creates the directory*/
    }
    strcat(address,"/");
    strcat(address,filename);

    fp = fopen(address, "w");
    
    if (fp == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error AS creating File: ");
        exit(0);
    }

    return fp;
}



int main(int argc, char * argv[]){
    parsingArguments(argc, argv);
    signal(SIGPIPE, SIG_IGN);

    as = connectToASSocket();

    selection();

    closeSocket(as, resAS);
}

int connectToASSocket(){
    int fd;   //TCP socket

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd==-1){
        puts("erro fd\n");
        exit(1);
        //error
    }

    memset(&hintsAS,0,sizeof(hintsAS));
    hintsAS.ai_family=AF_INET;        //IPv4
    hintsAS.ai_socktype=SOCK_STREAM;  //TCP socket

    errcode= getaddrinfo(TEJO,PORT,&hintsAS,&resAS);
    if(errcode!=0){
        /*error*/
        puts("erro getaddrinfo as\n");
        exit(1);
    }

    n = connect (fd,resAS->ai_addr,resAS->ai_addrlen);
    if(n == -1){
        /*error*/
        puts("erro connect as\n");
        exit(1);
    }

    return fd;
}

int connectToFSSocket() {

    fs = socket(AF_INET, SOCK_STREAM, 0);
    if (fs ==-1){
        puts("erro socket fs\n");
        exit(1);
        //error
    }

    memset(&(hintsFS),0,sizeof(hintsFS));
    hintsFS.ai_family=AF_INET;        //IPv4
    hintsFS.ai_socktype=SOCK_STREAM;  //TCP socket

    errcode= getaddrinfo ("tejo.tecnico.ulisboa.pt",DEFAULT_FS_PORT,&(hintsFS),&(resFS));
    if(errcode!=0){
        /*error*/
        puts("erro getaddrinfo fs\n");
        exit(1);
    }

    n= connect(fs,resFS->ai_addr,resFS->ai_addrlen);
    if(n==-1){
        /*error*/
        puts("erro connect fs\n");
        exit(1);
    }
    fs_channel_on = 1;
    return fs;
}

void closeSocket(int fd, struct addrinfo *res){
    if(fd == fs)
        fs_channel_on = 0;
    freeaddrinfo(res);
    close (fd);
}

void sendMessage(int fd, char * message){
    
    n=write (fd,message,strlen(message));
    if(n==-1){/*error*/
        puts("Send message ERROR");
        exit(1);
    }
}

void readFromSocket(int fd){
    char command[SIZE], info1[SIZE], info2[SIZE];
    char data [128];
    int readSize = 0;
    int fileSize;
    FILE * fp;

    n= read(fd,buffer,500);
    if(n==-1)/*error*/exit(1);
        buffer[n] = '\0';
    sscanf(buffer, "%s %s %s", command, info1, info2);
    write(1,buffer,n);
    if(fd == as){
        if(!strcmp(command, "RAU")){
            strcpy(tid, info1);
        } 
    } else if(fd == fs){
        if(!strcmp(command, "RRT")){
            puts("");
            fileSize = atoi(info2);
            fp = openFile(FILENAME);
            while(readSize < fileSize){
                n= read(fd,data,128);
                readSize += n;
                //printf("readSize: %d\n", readSize);
                fwrite(data, 1, n, fp);
            }
            fclose(fp);
        } else if(!strcmp(buffer, "RRM OK\n")){
            flag = 0;
        }

    }

}

void login(char * uid, char * pass){

    strcpy(UID, uid);

    constructUserMessage("LOG", uid, pass, NULL, NULL);
}

void req(char * fop, char * fname){

    sprintf(rid, "%d", random_number());

    switch (fop[0]){
    case 'L':
    case 'X':
        constructUserMessage("REQ", UID, rid, fop, NULL);
        break;
    case 'R':
    case 'U':
    case 'D':
        constructUserMessage("REQ", UID, rid, fop, fname);
        break;
    default:
        perror("Wrong Fop");
        exit(1);
        break;
    }
}

void val(char * uid, char * rid, char* vc) {
    constructUserMessage("AUT", uid, rid, vc, NULL);
}

void list(){
    constructUserMessage("LST", UID, tid, NULL, NULL);
}

void retrieve(char * filename){
    strcpy(FILENAME, filename);
    constructUserMessage("RTV", UID, tid, filename, NULL);
}

void upload(char * filename){
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
    constructUserMessage("UPL", UID, tid, filename, fileSize);
    strcat(message, " ");
    puts("");
    //strcpy(buffer, mess)
    sendMessage(fs, message);
    while(sizeRead < sz){

        n = fread(buffer, 1, SIZE, fp);
        sizeRead += n;
        m=write(fs,buffer,n);
        if(m==-1){/*error*/
            perror("write ERROR");
            exit(1);
        }
    }

    fclose(fp);

}

void delete(char * filename){
    constructUserMessage("DEL", UID, tid, filename, NULL);
}

void remove_x(){
    constructUserMessage("REM", UID, tid, NULL, NULL);
}

void exit_user() {
    flag = 0;
}

int random_number(){
    int rand_num;
    do{
        rand_num = rand() % 10000;
        //printf("rand num %d\n", rand_num);
    }while(rand_num<=999);

    return rand_num; //prints a random positive integer;
}