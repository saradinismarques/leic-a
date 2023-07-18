#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#include "directory_functions.c"

#define PORT "58011"
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define MAX_CLIENTS 15

// gcc -o server server_main.c

// GLOBALS
//int tcp_server_connections[MAX_CLIENTS]; 

char server_buffer[300]; // message received from user
char server_message[300]; // message sent
int n_user = 0; // nº of registered users

// FUNCTIONS
int serverUDPMessage();
void closeUDPServer();
void checkReceivedMessage();
void constructServerMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6, char* arg7);
void constructGroupMessage(char* dest, char* GID, char* GName, char* MID);
void reg(char* arg1, char* arg2);
void unr(char* arg1, char* arg2);
void login(char* arg1, char* arg2);
void logout();
void ext();
void groups();
void subscribe(char* arg1, char* arg2, char* arg3);
void unsubscribe(char* arg1, char* arg2);
void my_groups();
void ulist();
void post();
void retrieve();

// GLOBAL VARIABLES
char UID[6];
int currentGID; // contador de grupos presentes atualmente no servidor
// têm de ser globais porque são usadas na função serverUDPMessage e na closeUDP
int fd_udp;

GROUPLIST list; // alocação de memória necessária aqui
GROUPLIST *list_ptr = &list; 

int serverUDPMessage(int fd) { // receive and send
    ssize_t n, nread;
    socklen_t addrlen;
    struct sockaddr_in addr;
    
    memset(server_buffer, 0, sizeof(server_buffer));

    addrlen=sizeof(addr);
    nread=recvfrom(fd,server_buffer,300,0,(struct sockaddr*)&addr,&addrlen);
    if(nread==-1)
        /*error*/exit(1);
    write(1,"RECEIVED: ",10);
    write(1,server_buffer,nread);
    
    memset(server_message,0,sizeof(server_message));
    checkReceivedMessage(NULL);

    n=sendto(fd,server_message,sizeof(server_message),0,(struct sockaddr*)&addr,addrlen);
    if(n==-1)
        /*error*/exit(1);
    //}
    return 0;
}

void serverTCPMessage(int fd) {
    ssize_t n;
    //char buffer[128];
    memset(server_buffer, 0, sizeof(server_buffer));

    n=read(fd,server_buffer,4);
    if(n==-1)/*error*/exit(1);
    write(1,"received: ",10);
    write(1,server_buffer,n);
    return;
}
   // checkReceivedMessage(fd);

void sendTCPMessage(int fd, char *message, int nbytes) {
    ssize_t nleft, nwritten;
    char *ptr;
    printf("SERVER SENDS: '%s'\n", message);

    nleft = nbytes;
    ptr = message;

    while(nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if(nwritten <= 0)
            /*error*/ exit(1);
        nleft -= nwritten;
        ptr += nwritten;
    }

    return;
}



// void closeUDPServer(){
//     // freeaddrinfo(res_udp); ? é preciso alocar memória, onde?
//     close(fd_udp);
// }

void checkReceivedMessage(int fd){ // retorna 1 se fizermos exit

    char command[20] = {'\0'};
    char arg1[300] = {'\0'};
    char arg2[30] = {'\0'};
    char arg3[30] = {'\0'};

    sscanf(server_buffer, "%s %s %s %s", command,arg1,arg2,arg3);
    if(strcmp(command, "REG")==0) 
        reg(arg1,arg2);

    else if(strcmp(command, "UNR")==0)
        unr(arg1,arg2);

    else if(strcmp(command, "LOG")==0) {
        login(arg1,arg2);
    }

    else if(strcmp(command, "OUT")==0) 
        logout(arg1,arg2);

    // else if(strcmp(command, "exit")==0) 
    //     // logout todos users!!!
    //     ext();
    
    else if(strcmp(command, "GLS")==0 ) 
        groups();
    
    else if(strcmp(command, "GSR")==0 ) 
        subscribe(arg1,arg2,arg3);
    
    else if(strcmp(command, "GUR")==0 ) 
        unsubscribe(arg1,arg2);
    
    else if(strcmp(command, "GLM") == 0 ) 
        my_groups(arg1);
    
    else if(strcmp(command, "ULS")==0) {
        ulist(fd);
    }
    else if(strcmp(command, "PST")==0) {
        post(fd);
    }
    else if(strcmp(command, "RTV")==0) {
        retrieve(fd);
    }
}
    /*
    } else if(strcmp(command, "retrieve")==0 || strcmp(command, "r")==0) {
        if(validMID(arg1))
            retrieve(arg1);
    } 
    return SUCCESS;
} */
    

void constructServerMessage(char* command, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6, char* arg7) {

    strcpy(server_message, command);

    if(arg1!=NULL){
        strcat(server_message," ");
        strcat(server_message, arg1);
    }

    if(arg2!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg2);
    }
    if(arg3!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg3);
    }
    if(arg4!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg4);
    }
    if(arg5!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg5);
    }
    if(arg6!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg6);
    }
    if(arg7!=NULL){
        strcat(server_message, " ");
        strcat(server_message, arg7);
    }
    if ( strcmp(command,"RGM") != 0 && strcmp(command,"RGL") !=0 
    && strcmp(command, "RUL") != 0 && strcmp(command, "RRT") != 0) /* no caso do mgl ainda vamos adicionar ao server message */
        strcat(server_message, "\n"); /*as mensagens têm que acabar todas com "\n"*/
}

void constructGroupMessage(char* dest, char* GID, char* GName, char* MID){
    
    strcat(dest," ");
    strcat(dest,GID);
    strcat(dest," ");
    strcat(dest,GName);
    strcat(dest," ");
    strcat(dest,MID);

    return;
}

void reg(char* arg1, char* arg2){
    int ret = CreateUserDir(arg1);

    if (ret == 0){
        switch (errno) {
            case EEXIST:
                constructServerMessage("RRG","DUP",NULL,NULL,NULL,NULL,NULL,NULL);
                break;
            default:
                constructServerMessage("RRG","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
                break;
        }
    } else if(n_user == 1000) {
        constructServerMessage("RRG","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
    } else if(createFile("USERS",arg1,arg1,"pass",arg2)) {
        n_user++;
        constructServerMessage("RRG","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    } else {
        constructServerMessage("RRG","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
    }
}

void unr(char* arg1, char* arg2){
    int check_pass = checkPassword(arg1, arg2);
    char path[15];
    //VER SE ESTA LOGIN ??
    strcpy(path, constructFileAddress("USERS", arg1, NULL, NULL));
    int user_exists = existsDirectory(path);
    int pass = DelPassFile(arg1);
    int usr = DelUserDir(arg1);

    if (pass == 1 && usr == 1 && check_pass == 1 && user_exists == 1)
        constructServerMessage("RUN","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    else
        constructServerMessage("RUN","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
}

void login(char* arg1, char* arg2){
    FILE *fp;
    char pass[9];
    char address[30];
    strcpy(address, "USERS");
    strcat(address, "/");
    strcat(address, arg1);

    strcpy(UID,arg1);

    if (!existsDirectory(address)){ // user não está registado
        constructServerMessage("RLO","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
    }
    else if (checkPassword(arg1,arg2)) {
        if(createFile("USERS",UID,UID,"login","")) 
            constructServerMessage("RLO","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    } else 
        constructServerMessage("RLO","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
}

void logout(char* arg1, char* arg2){
    FILE *fp;
    char pass[9] = "";
    char address[30] = "";
    char file_name[16] = ""; 

    strcat(file_name,arg1);
    strcat(file_name,"_");
    strcat(file_name,"login.txt\0");

    strcpy(address, "USERS");
    strcat(address, "/");
    strcat(address, arg1);

    if (!existsDirectory(address)){ // user não está registado
        constructServerMessage("ROU","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
    }

    // fp = fopen(constructFileAddress("USERS/",arg1,arg1,"pass"), "r");
    // int res = fread(pass,sizeof(char),sizeof(pass),fp);

    // if (strcmp(pass,arg2) == 0){
    //     deleteFile("USERS",arg1,arg1,"login");
    //     constructServerMessage("ROU","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    // }
    else if(checkPassword(arg1,arg2)) {
        if(deleteFile("USERS",arg1,arg1,"login"))
            constructServerMessage("ROU","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    } else
        constructServerMessage("ROU","NOK",NULL,NULL,NULL,NULL,NULL,NULL);

    return;
}

// void ext(){
//     closeUDPServer();
//     return;
// }

void groups(){
    char ngroups[4];
    sprintf(ngroups,"%d",list_ptr->no_groups);
    
    if (list_ptr->no_groups == 0) // não há grupos
        constructServerMessage("RGL","0",NULL,NULL,NULL,NULL,NULL,NULL);
    else{
        constructServerMessage("RGL",ngroups,NULL,NULL,NULL,NULL,NULL,NULL);
        char MID[5];
        for (int i = 0; i < list_ptr->no_groups ; i++){
            strcpy(MID, convertMID(list_ptr->group_msg[i]));
            constructGroupMessage(server_message,list_ptr->group_no[i],list_ptr->group_name[i], MID);
        }
    }
    strcat(server_message,"\n");
}


void subscribe(char* arg1, char* arg2, char* arg3){
    // VER SE ESTA LOGIN ??
    char dir_path[12] = "";
    char group_path[12] = "";
    char ngroups[4] = "";
    int groupID;

    if(!existsDirectory(constructFileAddress("USERS", arg1, NULL, NULL))){
        constructServerMessage("RGS","E_USR",NULL,NULL,NULL,NULL,NULL,NULL);
        return;
    } else if(currentGID == 99){
        constructServerMessage("RGS","E_FULL",NULL,NULL,NULL,NULL,NULL,NULL);
        return;
    }
    strcat(dir_path,"GROUPS/");

    if (strcmp(arg2,"00") == 0){ // criar novo grupo
        currentGID++;

        if (currentGID < 10 )
            sprintf(ngroups,"0%d",currentGID);
        else
            sprintf(ngroups,"%d",currentGID);
        strcat(group_path,ngroups);

        strcat(dir_path,group_path);

        puts(dir_path);
        if(createDirectory(dir_path)) {
            if(createFile("GROUPS",group_path,group_path,"name", arg3) && createFile("GROUPS",group_path,NULL,UID,"")) {// ? porque é que não escreve logo arg3 no ficheiro novo)
                constructServerMessage("RGS","NEW",NULL,NULL,NULL,NULL,NULL,NULL);
                return;
            }
        }
    } else if(!existsDirectory(constructFileAddress("GROUPS", arg2, NULL, NULL))){
        constructServerMessage("RGS","E_GRP",NULL,NULL,NULL,NULL,NULL,NULL);
        return;
    } else if(!checkGname(arg2, arg3)) {
        constructServerMessage("RGS","E_GNAME",NULL,NULL,NULL,NULL,NULL,NULL);
        return;
    }else { // GID já existe
        if(createFile("GROUPS",arg2,NULL,UID, "")) {
            constructServerMessage("RGS","OK",NULL,NULL,NULL,NULL,NULL,NULL);
            return;
        }
    }

    constructServerMessage("RGS","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
    return;
}

void unsubscribe(char* arg1, char* arg2){

    //VER SE ESTA LOGIN??
    if(!existsDirectory(constructFileAddress("USERS", arg1, NULL, NULL))){
        constructServerMessage("RGU","E_USR",NULL,NULL,NULL,NULL,NULL,NULL);
    } else if(!existsDirectory(constructFileAddress("GROUPS", arg2, NULL, NULL))){
        constructServerMessage("RGU","E_GRP",NULL,NULL,NULL,NULL,NULL,NULL);
    } else if(deleteFile("GROUPS",arg2,NULL,UID)) {
        constructServerMessage("RGU","OK",NULL,NULL,NULL,NULL,NULL,NULL);
    } else 
        constructServerMessage("RGU","NOK",NULL,NULL,NULL,NULL,NULL,NULL);

}

void my_groups(char *arg1){
    FILE* fp;
    char buffer[50] = ""; // buffer para onde vamos ler o nome do grupo

    char users_groups[4000] = "";
    int n = 0; // nº de grupos em que user está subscrito 
    char N[3] = "";
    char gid_path[3] = "";
    char dir_path[30] = "";
    char MID[5];

    for (int i = 0; i < currentGID; i++){
        if (i < 10)
            sprintf(gid_path, "0%d", i+1);
        else
            sprintf(gid_path, "%d", i+1);

        strcpy(dir_path, constructFileAddress("GROUPS",gid_path,NULL,UID));
        if (fileExists(dir_path)) {
            n++;
            strcpy(buffer, getGname(gid_path));
            strcpy(MID, convertMID(list_ptr->group_msg[i]));
            constructGroupMessage(users_groups,gid_path,buffer,MID);
        }
    }
    if(n == 0){ // user não está subscrito a nenhum grupo
        constructServerMessage("RGM","0",NULL,NULL,NULL,NULL,NULL,NULL);
        strcat(server_message,"\n");
    }

    else{
        sprintf(N,"%d",n);
        constructServerMessage("RGM",N,NULL,NULL,NULL,NULL,NULL,NULL);
        strcat(server_message,users_groups);
        strcat(server_message,"\n");
    }

    return;
}

void readTCPMessage(int fd, char *buffer, int nbytes) {
    ssize_t nleft, nwritten, nread;
    char *ptr;

    nleft = nbytes;
    ptr = buffer; 

    while (nleft > 0) {
        nread = read(fd, ptr, nleft);
        if (nread == -1)
            /*error*/ exit(1);

        else if (nread == 0)
            break; // closed by peer

        nleft -= nread;
        ptr += nread;
    }
    //write(1, buffer, nbytes);
}

void ulist(int fd){
    char dir_path[30] = "";
    char file_name[15] = "";
    char name_path[30] = "";
    DIR *d;
    struct dirent *dir;
    int i=0;
    FILE *fp_name;
    char users[25] = "";
    char g_users[100] = ""; // tamanho?? 
    char arg1[3] = "";
    char Gname[25] = "";
    char file_address[50] = "";
    FILE *fp;
    char GIDname[30];

    readTCPMessage(fd, arg1, 2);

    printf("arg1: %s\n",arg1);

    strcat(dir_path,"GROUPS/");
    strcat(dir_path,arg1);

    strcat(file_name,arg1);
    strcat(file_name,"_name");

    if (!existsDirectory(dir_path)){ // user não está registado
        constructServerMessage("RUL","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
        sendTCPMessage(fd,server_message,7);
        return;
    }

    strcat(file_address, dir_path);
    strcat(file_address, "/");
    strcat(file_address, file_name);
    strcat(file_address, ".txt");

    fp_name = fopen(file_address, "r");
    int err = fread(Gname, sizeof(char), sizeof(Gname), fp_name);
    if(err < 0) 
        //err
    fclose(fp_name);
    d = opendir(dir_path);
    constructServerMessage("RUL","OK",Gname,NULL,NULL,NULL,NULL,NULL);
    sendTCPMessage(fd,server_message,7+err);

    sprintf(file_name,"%s.txt",file_name);

    while ((dir = readdir(d)) != NULL){
        sprintf(GIDname,"%s/%s",dir_path, dir->d_name);

        fp = fopen(GIDname, "r");
        if (fp != NULL){
            if(dir->d_name[0] == '.')
                continue;

            // if(strlen(dir->d_name) > 2)
            //     continue;

            if ( strcmp(dir->d_name,file_name) == 0 ){
                continue;
            }

            else{
                int size = strlen(dir->d_name);
                sprintf(users," %s",dir->d_name);
                users[size-3] = '\0';
                sendTCPMessage(fd,users,strlen(users));
            }
        }
    }
    sendTCPMessage(fd,"\n",1);
    return;
}

void post(int fd){ 
    char dir_name[30] = "";
    char msg_dir_name[30] = "";
    char msg[5] = "";
    char msg_dir_name_AUTHOR[50] = "";
    char msg_dir_name_TEXT[50] = "";
    char msg_dir_name_file[50] = ""; 
    char file_path[30] = ""; 
    char buffer[512] = "", aux[10] = "";
    char UID[6] = "";
    char GID[3] = "";

    int n;
    char c;
    int i = 0;

    readTCPMessage(fd,UID,5);
    printf("UID: '%s'\n",UID);
    readTCPMessage(fd,buffer,1);
    printf("space buffer: '%s\n",buffer);
    memset(buffer,0,128);

    readTCPMessage(fd,GID,2);
    printf("GID: '%s'\n",GID);
    readTCPMessage(fd,buffer,1);
    printf("space buffer: '%s\n",buffer);
    memset(buffer,0,512);


    while(1) {
        readTCPMessage(fd, buffer, 1);
        // printf("\nTsize: '%s'\n", buffer);

        if(strcmp(buffer, " ") == 0) {
            memset(buffer, 0, 512);
            break;
        }
        strcat(aux, buffer);
        memset(buffer, 0, 512);
    }
    
    int Tsize = atoi(aux);
    printf("Tsize: %d\n",Tsize);
    char text[241] = "";
    readTCPMessage(fd, text, Tsize);
    printf("text: '%s'\n",text);

    //-----------------------------------------------------------------------------------------------------

    sprintf(dir_name,"GROUPS/%s/MSG",GID);
    printf("dir_name: '%s'\n",dir_name);
    printf("atoi(GID): %d\n",atoi(GID));

    if (!existsDirectory(dir_name)){ // se a diretoria MSG ainda não tivesse sido criada = 1º post no grupo
        createDirectory(dir_name);
        list_ptr->group_msg[atoi(GID)-1] = 1; // vai ter 1 mensagem
    }
    // else{
    //     list_ptr->group_msg[atoi(GID)] = countMessageGroup(list_ptr,GID) + 1;
    // }
    
    if ( list_ptr->group_msg[atoi(GID)-1] == 9999 ){ // ? já não se pode adicionar mais mensagens ao grupo
        constructServerMessage("PST","NOK",NULL,NULL,NULL,NULL,NULL,NULL);
        sendTCPMessage(fd,server_message,7);
        return;
    }

    int num_msg = ++list_ptr->group_msg[atoi(GID)-1];

    strcpy(msg,convertMID(num_msg));

    sprintf(msg_dir_name,"GROUPS/%s/MSG/%s",GID,msg);
    printf("msg_dir_name: '%s'\n",msg_dir_name);

    createDirectory(msg_dir_name);

    strcat(msg_dir_name_AUTHOR,msg_dir_name);
    strcat(msg_dir_name_AUTHOR,"/A U T H O R.txt");

    FILE* fp_a = fopen(msg_dir_name_AUTHOR, "w");
    fputs(UID,fp_a);
    puts("passa");
    fclose(fp_a);

    strcat(msg_dir_name_TEXT,msg_dir_name);
    strcat(msg_dir_name_TEXT,"/T E X T.txt");

    FILE* fp_t = fopen(msg_dir_name_TEXT, "w");
    fputs(text,fp_t); 
    //fputs("\n",fp_t); ?? NO TEJO PARECE QUE POE SEMPRE \n no fim

    fclose(fp_t);

    memset(buffer, 0, 512);
    readTCPMessage(fd,buffer,1);
    printf("space_buffer: '%s'\n",buffer);

    char N[5] = "";
    strcpy(N,convertMID(list_ptr->group_msg[atoi(GID)-1]));
    // esta condição é sempre verificada ..
    if(strcmp(buffer, "\n") == 0) { // funciona com "" mas devia ser "\n" ?
        constructServerMessage("RPT",N,NULL,NULL,NULL,NULL,NULL,NULL);
        printf("MESSAGE: '%s'", server_message);
        sendTCPMessage(fd,server_message,strlen(server_message));
        return;
    }
    else{
        puts("else");
        memset(buffer, 0, 512);
        char Fname[25] = "";
        //'Fname '
        while(1) {
            readTCPMessage(fd, buffer, 1);

            if(strcmp(buffer, " ") == 0) {
                memset(buffer, 0, 512);
                break;
            }
            strcat(Fname, buffer);
            memset(buffer, 0, 512);
        }
        strcat(file_path,msg_dir_name);
        strcat(file_path,"/");
        strcat(file_path,Fname);
        printf("file_path: '%s'\n",file_path);
        FILE* fd_f = fopen(file_path,"w");

        strcpy(aux, "\0");
        //'Fsize '
        while(1) {
            readTCPMessage(fd, buffer, 1);

            if(strcmp(buffer, " ") == 0) {
                memset(buffer, 0, 512);
                break;
            }
            strcat(aux, buffer);
            memset(buffer, 0, 512);
        }
        long int Fsize = atoi(aux);

        //'data'
        long int nleft = Fsize;
        int nread = 512;

        while(nleft > 0) {
            if(nleft < 512)
                nread = nleft;

            readTCPMessage(fd, buffer, nread);
            printf("BUFFER:'%s'\n", buffer);
            fwrite(buffer,sizeof(char),nread,fd_f);
            //a medida que lê põe no ficheiro do servidor

            memset(buffer, 0, 512);
            nleft -= 512;
        }
        fclose(fd_f);

    }
    constructServerMessage("RPT",N,NULL,NULL,NULL,NULL,NULL,NULL);
    sendTCPMessage(fd,server_message,strlen(server_message));
    
    return;
}

void retrieve(int fd){

    puts("retrieve");

    char buffer[128] = "";
    char UID[6] = "";
    char GID[3] = "";
    char MID[5] = "";
    //'UID GID '

    readTCPMessage(fd,UID,5); 
    readTCPMessage(fd,buffer,1);
    readTCPMessage(fd,GID,2);
    readTCPMessage(fd,buffer,1);
    readTCPMessage(fd,MID,4);

    puts(UID);
    puts(GID);
    puts(MID);

    char text[241] = "";
    char data[256] = "";

    char dir_path[30] = "";
    char message_path[30] = "";
    char authour_path[30] = "";
    char text_path[30] = "";
    char file_path[30] = "";
    FILE *fd_t, *fd_a, *fd_f;
    DIR *d, *d_m;
    struct dirent *dir, *dir_m;
    sprintf(dir_path,"GROUPS/%s/MSG",GID);

    // calcula N
    int mid = atoi(MID);
    int gid = atoi(GID);

    printf("mid: %d\n",mid);
    int n = list_ptr->group_msg[gid-1] - mid + 1;
    printf("list_ptr: %d\n",list_ptr->group_msg[gid-1]);
    printf("n: %d\n",n);
    char N[3] = "";
    sprintf(N,"%d",n);

    if(list_ptr->group_msg[gid-1] == 0)
        constructServerMessage("RRT","EOF",N,NULL,NULL,NULL,NULL,NULL);


    memset(server_buffer,0,300);
    constructServerMessage("RRT","OK",N,NULL,NULL,NULL,NULL,NULL);

    sendTCPMessage(fd,server_message,strlen(server_message));

    if ( n == 0) // se não houver mensagens
        return;

    while (n > 0){
        strcpy(MID,convertMID(mid));
        
        sendTCPMessage(fd," ",1); // espaço 
        sendTCPMessage(fd,MID,4);
        sendTCPMessage(fd," ",1); // espaço 

        sprintf(authour_path,"%s/%s/A U T H O R.txt",dir_path,MID);
        fd_a = fopen(authour_path,"r");
        int r = fread(UID,sizeof(char),sizeof(UID),fd_a);
        printf("UID: '%s'\n",UID);
        sendTCPMessage(fd,UID,r);
        sendTCPMessage(fd," ",1);

        sprintf(text_path,"%s/%s/T E X T.txt",dir_path,MID);
        fclose(fd_a);
        fd_t = fopen(text_path,"r");
        r = fread(text,sizeof(char),sizeof(text),fd_t);
        printf("r: %d\n",r);
        printf("text: '%s'\n",text);

        char Tsize[4] = "";
        sprintf(Tsize,"%d",r);

        printf("Tsize: '%s'\n",Tsize);
        sendTCPMessage(fd,Tsize,strlen(Tsize));
        sendTCPMessage(fd," ",1);

        sendTCPMessage(fd,text,r);

        sprintf(message_path,"%s/%s",dir_path,MID);
        d_m = opendir(message_path);
        while ( (dir_m = readdir(d_m)) != NULL){
            printf("d_name: '%s'\n",dir_m->d_name);
            if(dir_m->d_name[0]=='.')
                continue;
            if (strcmp(dir_m->d_name,"A U T H O R.txt") != 0 && strcmp(dir_m->d_name,"T E X T.txt") != 0){
                puts("entrou");
                sendTCPMessage(fd," / ", 3);
                sendTCPMessage(fd,dir_m->d_name,strlen(dir_m->d_name));
                sendTCPMessage(fd," ",1);
                
                sprintf(file_path,"%s/%s",message_path,dir_m->d_name);
                
                fd_f = fopen(file_path,"r");
               // if(fd_f)
                fseek(fd_f, 0, SEEK_END);
                long int file_size = ftell(fd_f), ntotal = 0;
                char Fsize[11];
                sprintf(Fsize,"%ld", file_size);
                sendTCPMessage(fd,Fsize, strlen(Fsize));
                sendTCPMessage(fd," ",1);

                fseek(fd_f, 0, SEEK_SET);

                while(ntotal < file_size) {
                    r = fread(buffer, 1, 512, fd_f);
                    ntotal += r;
                    sendTCPMessage(fd, buffer, r);
                    memset(buffer, 0, 512);
                }
                fclose(fd_f);
            }
        }
        n--;
        printf("n: %d\n",n);
        mid += 1;
    }
    //MANDAR NOK
    puts("-----------------");
    sendTCPMessage(fd,"\n",1);
}

int main(){
    int tcpfd, udpfd, newfd, afd[10];
    fd_set rfds;
    enum {idle,busy} state;
    int maxfd, counter;
    int errcode;
    struct addrinfo hints_tcp, hints_udp, *res_tcp, *res_udp;
    ssize_t n;
	socklen_t addrlen;
	struct sockaddr_in addr;
    char buffer[128];

    currentGID = ListGroupsDir(list_ptr);
    n_user = countUsers();

    for(int i = 0; i < 10; i++) {
        afd[i] = -1;
    }

    //TCP socket
	tcpfd = socket(AF_INET,SOCK_STREAM,0);   //TCP socket
	if (tcpfd==-1) exit(1); //error

	memset(&hints_tcp,0,sizeof hints_tcp);
	hints_tcp.ai_family=AF_INET;            //IPv4
	hints_tcp.ai_socktype=SOCK_STREAM;      //TCP socket
	hints_tcp.ai_flags=AI_PASSIVE;

	errcode=getaddrinfo(NULL,PORT,&hints_tcp,&res_tcp);
	if((errcode)!=0)/*error*/exit(1);

	n=bind(tcpfd,res_tcp->ai_addr,res_tcp->ai_addrlen);
	if(n==-1) /*error*/ exit(1);

	if(listen(tcpfd,5)==-1)/*error*/exit(1);

    //UDP socket
    udpfd = socket(AF_INET,SOCK_DGRAM,0);       //UDP socket
    if(udpfd==-1) /*error*/exit(1);

    memset(&hints_udp,0,sizeof hints_udp);

    hints_udp.ai_family=AF_INET;      // IPv4
    hints_udp.ai_socktype=SOCK_DGRAM; // UDP socket
    hints_udp.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,PORT,&hints_udp,&res_udp);
    if(errcode!=0) /*error*/ exit(1);

    n=bind(udpfd,res_udp->ai_addr, res_udp->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    state=idle;

    while(1){
        FD_ZERO(&rfds);
        FD_SET(tcpfd, &rfds);
		FD_SET(udpfd, &rfds);
        maxfd = max(tcpfd, udpfd);
    
        //if(state == busy){
            for(int i = 0; i < 10; i++) {
                if(afd[i] != -1) {
                    FD_SET(afd[i], &rfds);
                    maxfd=max(maxfd, afd[i]);
                }
            }
        //}
        counter = select(maxfd+1,&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval *)NULL);
    
        if(counter==0) {
            /*error*/exit(1);
        }
        if(FD_ISSET(tcpfd, &rfds)){
            addrlen=sizeof(addr);
            if((newfd=accept(tcpfd,(struct sockaddr*)&addr,&addrlen))==-1)
                /*error*/exit(1);
            int i;
            switch(state){
                case idle:    
                    for(i = 0; i < 10; i++) {
                        if(afd[i] == -1) {
                            afd[i]=newfd;
                            break;
                        }
                    }
                    if(i == 10)             
                        state = busy;
                    break;
                case busy: /* ... *///write “busy\n” in newfd
                    close(newfd); 
                    break;
            }
        }
        for(int i = 0; i < 10; i++) {
            if(FD_ISSET(afd[i], &rfds)){
                serverTCPMessage(afd[i]);
                checkReceivedMessage(afd[i]);
                close(afd[i]);
                afd[i] = -1;
                state = idle;
            }//connection closed by peer
        }
        if (FD_ISSET(udpfd, &rfds)) {
            serverUDPMessage(udpfd);
        }
    }
    close(udpfd);
    freeaddrinfo(res_tcp);
    freeaddrinfo(res_udp);

    return 0;
}
