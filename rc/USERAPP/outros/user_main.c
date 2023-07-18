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
#include <ctype.h>

// ./user -n tejo.tecnico.ulisboa.pt -p 58011
// ./user -n carolina-VirtualBox -p 58011

#define DEFAULT_DS_PORT "58028"
#define SIZE 100

#define SUCCESS 1
#define ERROR 0
#define EXIT 0

char DSIP[20], DSport[20]; // directory server credentials

//aux in getIPAddress()
int hostname;
char hostbuffer[256];
struct hostent *host_entry;

char message[300];
struct addrinfo *resTCP;

char UID[6];
char pass[9];
char GID[3];
char GName[24];
char MID[5];

//registragion
int reg(char* UID, char* pass); 
int unregister(char* UID, char* pass);

//user identification and session termination
int login(char* UID, char* pass);
int logout();
int showuid();
int ext();

//group management
int groups(); //AKA gl -> GID,GName
int subscribe(char* GID, char* GName); //AKA s
int unsubscribe(char* GID); //AKA u
int my_groups(); // AKA mgl -> list of groups user subscribed
int sag(char* GID); //sag
int showgid (); // aka sg
int ulist(); // AKA ul -> users subscribed to that groups

//messaging
int post(char* text, char* FName);
int retrieve(char* MID); //AKA r

//aux functions
char* getIPAddress();
void parsingArguments(int argc, char *argv[]);
int parsingCommands();
int validUID(char *UID);
int validPass(char *pass);
int validGID(char * GID);
int validGName(char * GName);
int validText(char *text);
int validFName(char *FName);
int validMID(char* MID);


void sendUDPMessage(char *message);
void constructUserMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6, char* arg7);

// ------------------------ MAIN -------------------------------
int main(int argc, char *argv[]){
    parsingArguments(argc, argv);

    while(parsingCommands()) {}

    return 0;
}

// ------------------------ IP -------------------------------
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

// ------------------------ PARSINGS -------------------------------
void parsingArguments(int argc, char * argv[]) {
    int option;
    char options[2][2] = {"\0", "\0"}; /*guarda as opçoes passadas na inicializaçao do programa 0=d; 1=p; 2=n*/

    while( (option = getopt(argc, argv, "n:p:")) != -1) {
        //optarg e gerado e guarda a palavra apos a flag
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
                fprintf(stderr, "Problem parsing arguments!\n");
                exit(1);
        }
    }

    /*verifica quais foram as opçoes apanhadas e completa as que nao foram dadas */
    if (strcmp(options[0], "n") != 0 ) {
        strcpy(DSIP, getIPAddress()); /*mete o IP da maquina actual*/
    }
    if (strcmp(options[1], "p") != 0 ) {
        strcpy(DSport, DEFAULT_DS_PORT);
    }
}

//parsing std input
int parsingCommands() {
    char buffer[500] = {'\0'};
    char arg1[300] = {'\0'};
    char arg2[30] = {'\0'};
    char command[20] = {'\0'};

    fgets(buffer, SIZE, stdin);
    sscanf(buffer, "%s", command);

    if(strcmp(command, "post") == 0) {
        sscanf(buffer, "%s \"%[^\"]\" %s", command, arg1, arg2);
        post(arg1, arg2);
    }
    
    sscanf(buffer, "%s %s %s", command, arg1, arg2);

    if(strcmp(command, "reg")==0){
        reg(arg1,arg2);
        
    } else if(strcmp(command, "unregister")==0 || strcmp(command, "unr")==0){
        unregister(arg1,arg2);

    } else if(strcmp(command, "login")==0) {
        login(arg1,arg2);
    } else if(strcmp(command, "logout")==0) {
        logout();
    } else if(strcmp(command, "showuid")==0 || strcmp(command, "su")==0) {
        showuid();
    } else if(strcmp(command, "exit")==0) {
        ext();
        return EXIT;
    } else if(strcmp(command, "groups")==0 || strcmp(command, "gl")==0) {
        groups();
    } else if(strcmp(command, "subscribe")==0 || strcmp(command, "s")==0) {
        subscribe(arg1,arg2);
    } else if(strcmp(command, "unsubscribe")==0 || strcmp(command, "u")==0) {
        unsubscribe(arg1);
    } else if(strcmp(command, "my_groups")==0 || strcmp(command, "mgl")==0) {
        my_groups();
    } else if(strcmp(command, "select")==0 || strcmp(command, "sag")==0) {
        sag(arg1);
    } else if(strcmp(command, "showgid")==0 || strcmp(command, "sg")==0) {
        showgid();
    } else if(strcmp(command, "ulist")==0 || strcmp(command, "ul")==0) {
        ulist();
    } else if(strcmp(command, "retrieve")==0 || strcmp(command, "r")==0) {
        retrieve(arg1);
    } 
    return SUCCESS;
}

void constructUserMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6, char* arg7) {

    strcpy(message, command);

    if(arg1!=NULL){
        strcat(message, " ");
        strcat(message, arg1);
    }

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
    if(arg5!=NULL){
        strcat(message, " ");
        strcat(message, arg5);
    }
    if(arg6!=NULL){
        strcat(message, " ");
        strcat(message, arg6);
    }
    if(arg7!=NULL){
        strcat(message, " ");
        strcat(message, arg7);
    }

    if (strcmp(command,"PST") == 0 ){
        strcat(message, " ");
        return;
    }

    strcat(message, "\n"); /*as mensagens têm que acabar todas com "\n"*/
}

void sendUDPMessage(char *message) {
    int fd, errcode;
    struct addrinfo hints, *res;
    ssize_t n;
    socklen_t addrlen;
    struct sockaddr_in addr;
    char buffer[300];

    fd=socket(AF_INET,SOCK_DGRAM,0);    //UDP socket
    if(fd==-1) /*error*/exit(1);
    
    memset(&hints,0,sizeof(hints)); 
    hints.ai_family=AF_INET;    //IPv4 
    hints.ai_socktype=SOCK_DGRAM;   //UDP socket

    errcode=getaddrinfo(DSIP,DSport,&hints,&res); 
    if(errcode!=0) /*error*/ exit(1);

    n = sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen); 
    if(n == -1) /*error*/ exit(1);

    addrlen=sizeof(addr);

    // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LER ATE NAO TER MAIS NADA <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    n=recvfrom(fd,buffer,300,0,(struct sockaddr*)&addr,&addrlen);

    if(n==-1) /*error*/ exit(1);
    write(1,"echo: ",6);
    write(1,buffer,n);
    freeaddrinfo(res); 
    close(fd);
}

// void sendTCPMessage(char* message){
//     int fd, errcode;
//     ssize_t nbytes, nleft, nwritten, nread;
//     struct addrinfo hints, *res;
//     char buffer[300], *ptr;

//     ptr = message;

//     fd = socket(AF_INET, SOCK_STREAM, 0);   //TCP socket
//     if (fd==-1) exit(1); //error

//     memset(&hints,0,sizeof hints);
//     hints.ai_family=AF_INET;        //IPv4
//     hints.ai_socktype=SOCK_STREAM;  //TCP socket

//     errcode= getaddrinfo(DSIP,DSport,&hints,&res);
//     if(errcode!=0)/*error*/exit(1);

//     nbytes = connect(fd, res->ai_addr, res->ai_addrlen);
//     if (nbytes == -1) /*error*/
//         exit(1);
//     nbytes = strlen(message);

//     nleft = nbytes;
//     while (nleft > 0) {
//         nwritten = write(fd, ptr, nleft);
//         if (nwritten <= 0)
//             /*error*/ exit(1);
//         nleft -= nwritten;
//         ptr += nwritten;
//     }

//     nleft = 300;
//     ptr = buffer;

//     while (nleft > 0) {
//         nread = read(fd, ptr, nleft);
//         printf("%ld\n", nread);
//         if (nread == -1)
//             /*error*/ exit(1);

//         else if (nread == 0)
//             break; // closed by peer

//         nleft -= nread;
//         ptr += nread;
//     }
//     nread = nbytes - nleft;

//     freeaddrinfo(res);
//     close (fd);

//     write(1,"echo: ",6);//stdout
//     write(1,buffer, nread);
// }
int connectToTCPSocket() {
    int fd, errcode;
    struct addrinfo hints;
    ssize_t n;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd==-1) {
        //ERROR
        exit(1);
    }
    
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;        //IPv4
    hints.ai_socktype=SOCK_STREAM;  //TCP socket

    errcode= getaddrinfo(DSIP,DSport,&hints,&resTCP);
    if(errcode!=0)/*error*/
        exit(1);

    n = connect(fd, resTCP->ai_addr, resTCP->ai_addrlen);
    if (n == -1) /*error*/
        exit(1);

    return fd;
}

void sendTCPMessage(int fd, char *message) {
    ssize_t n;

    n = write(fd, message, strlen(message));
    if(n == -1) {
        //ERROR
        exit(1);
    }
}

void readFromTCPSocket(int fd) {
    ssize_t n;
    char buffer[256];

    n = read(fd, buffer, 256);
    if(n == -1) {
        //ERROR
        exit(1);
    }
    write(1, "echo: ", 6);
    write(1, buffer, n);
}

void closeTCPSocket(int fd) {
    freeaddrinfo(resTCP);
    close(fd);    
}


// ------------------------ FUNCTIONS -------------------------------
int reg(char *arg1, char *arg2){
    
    strcpy(UID, arg1);
    strcpy(pass, arg2);

    //printf("reg - UID: %s pass: %s\n", UID, pass);

    constructUserMessage("REG", UID, pass, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);

    return SUCCESS;
}


int unregister(char *arg1, char *arg2){

    //printf("unregister - UID: %s pass: %s\n", UID, pass);
    strcpy(UID, arg1);
    strcpy(pass, arg2);

    constructUserMessage("UNR", UID, pass, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    return SUCCESS;
}

int login(char *arg1, char *arg2){

    //printf("login - UID: %s pass: %s\n", UID, pass);
    strcpy(UID, arg1);
    strcpy(pass, arg2);

    constructUserMessage("LOG", UID, pass, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    return SUCCESS;
}

int logout(){

    printf("logout\n");
    constructUserMessage("OUT", UID, pass, NULL, NULL,NULL,NULL,NULL);
    printf("'%s'\n", message);
    sendUDPMessage(message);
    return SUCCESS;
}

int showuid(){
    printf("%s\n",UID);
    return SUCCESS;
}

int ext(){
    printf("exit\n");
    return SUCCESS;
}

int groups(){
    constructUserMessage("GLS", NULL, NULL, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    return SUCCESS;
}

int subscribe(char *arg1, char *arg2){
    if (strcmp(arg1,"0") == 0 ){
        
    }
    strcpy(GID, arg1);
    strcpy(GName, arg2);
    constructUserMessage("GSR", UID, GID, GName, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    //printf("subscribe - GID: %s GName: %s\n", GID, GName);
    return SUCCESS;
}

int unsubscribe(char *arg1) {
    strcpy(GID, arg1);
    constructUserMessage("GUR", UID, GID, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    //printf("unsubscribe - GID: %s\n", GID);
    return SUCCESS;
}

int my_groups() {
    constructUserMessage("GLM", UID, NULL, NULL, NULL,NULL,NULL,NULL);
    sendUDPMessage(message);
    return SUCCESS;
}
int sag(char *arg1) {
    strcpy(GID,arg1);
    //printf("select - GID: %s\n", GID);
    return SUCCESS;
}

int showgid() {
    // printf("showgid\n");
    printf("%s\n",GID);
    return SUCCESS;
}

int ulist() {
    int fd = connectToTCPSocket();

    constructUserMessage("ULS",GID,NULL,NULL,NULL,NULL,NULL,NULL);
    //sendTCPMessage(message);
    // printf("ulist\n");

    sendTCPMessage(fd, message);
    readFromTCPSocket(fd);
    closeTCPSocket(fd);
    return SUCCESS;
}

// int post(char *arg1, char *arg2) {
//     char Tsize[4];
//     sprintf(Tsize,"%ld",strlen(arg1));
//     puts("passou copia");
//     printf("size:%s\n",Tsize);
//     FILE* fp;
//     char* data = 0;
//     long length;
//     if ( arg2 == NULL ){
//         constructUserMessage("PST",UID,GID,Tsize,arg1,NULL,NULL,NULL);
//         sendTCPMessage(message);
//         return SUCCESS;
//     }
//     fp = fopen(arg2,"rb");
//     if (fp){
//         fseek(fp,0,SEEK_END);
//         length = ftell(fp);
//         fseek(fp,0,SEEK_SET);
//         data = malloc(length);
//         if (data){
//             fread(data,1,length,fp);
//         }
//         fclose(fp);
//     }
//     char Fsize[10];
//     sprintf(Fsize,"%ld",length);
//     printf("Fsize: %s\n",Fsize);
//     printf("data: %s\n",data);
//     constructUserMessage("PST",UID,GID,Tsize,arg1,arg2,Fsize,NULL);
//     size_t post_msg_sz = strlen(data)+strlen(message)+1;


//     char* post_message = (char*)malloc(post_msg_sz);
//     puts("alocou");
//     strcpy(post_message,message);
//     strcat(post_message,data);
//     strcat(post_message,"\n");
//     printf("'%s'\n", post_message);

//     sendTCPMessage(post_message);
//     free(data);
//     free(post_message);
    //printf("post - text: %s FName: %s\n", text, FName);
//     return SUCCESS;
// }

int post(char *arg1, char *arg2) {
    char Tsize[4];
    sprintf(Tsize,"%ld",strlen(arg1));
    int fd = connectToTCPSocket();

    if (arg2 == NULL){
        constructUserMessage("PST", UID, GID, Tsize, arg1,NULL,NULL,NULL);
        sendTCPMessage(fd, message);
        readFromTCPSocket(fd);
        closeTCPSocket(fd);
        return SUCCESS;
    }

    FILE *fp = fopen(arg2, "r");
    long int size, nread;
    char Fsize[100], buffer[256];
    ssize_t n;

    if(fp) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        sprintf(Fsize, "%ld", size);
        constructUserMessage("PST", UID, GID, Tsize, arg1, arg2, Fsize, NULL);
        strcat(message, " ");
        sendTCPMessage(fd, message);
        
        fseek(fp, 0, SEEK_SET);

        while(nread < size) {
            n = fread(buffer, 1, 256, fp);
            printf("nread_file: %ld\n", n);
            printf("'%s'\n", buffer);
            nread += n;
            printf("nread: %ld\n", nread);
            sendTCPMessage(fd, buffer);
        }
    }
    fclose(fp);

    readFromTCPSocket(fd);
    closeTCPSocket(fd);
}

int retrieve(char *arg1) {
    strcpy(MID, arg1);
    // constructUserMessage("RTV",UID,GID,MID,NULL,NULL,NULL,NULL);
    // sendTCPMessage(message);
    //printf("retrieve - MID: %s\n", MID);
    return SUCCESS;
}
