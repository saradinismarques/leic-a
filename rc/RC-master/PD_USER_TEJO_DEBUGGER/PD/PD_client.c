#include "PD_client.h"


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
    char options[3][2] = {"\0", "\0", "\0"}; /*guarda as opçoes passadas na inicializaçao do programa 0=d; 1=p; 2=n*/
    if(argc <= 1){
        perror("Machine IP needed\n");
        exit(1);
    }

    strcpy(pdip, argv[1]);
    while( (option = getopt(argc, argv, "d:n:p:")) != -1) {
        //optarg e gerado e guarda a palavra apos a flag
        switch(option){
            case 'd':
                strcpy(pdPORT, optarg);
                strcpy(options[0], "d");
                break;
            case 'p':
                strcpy(asip, optarg);
                strcpy(options[1], "p");
                break;
            case 'n':
                strcpy(asPORT, optarg);
                strcpy(options[2], "n");
                break;
            default:
                fprintf(stderr, "Problem parsing arguments!\n");
                exit(1); 
        }
    }

    /*verifica quais foram as opçoes apanhadas e completa as que nao foram dadas */
    if ( strcmp(options[0], "d") != 0 ) {
        strcpy(pdPORT, DEFAULT_PD_PORT);
    }
    if ( strcmp(options[1], "p") != 0 ) {
        strcpy(asip, pdip); /*mete o IP da maquina actual*/
    }
    if ( strcmp(options[2], "n") != 0 ) {
        strcpy(asPORT, DEFAULT_AS_PORT);
    }
    //printf("pdPORT: %s\tASIP: %s\tasPORT: %s\n", pdPORT, asip, asPORT);
}

char * constructPDMessage(char* command, char* UID, char* pass){

    strcpy(message, command);
    strcat(message, " ");
    strcat(message, UID);
    strcat(message, " ");
    strcat(message, pass);

    if(strcmp(command, "REG") == 0) {
        strcat(message, " ");
        strcat(message, pdip);
        strcat(message, " ");
        strcat(message, pdPORT);
    }

    strcat(message, "\n"); /*as mensagens têm que acabar todas com "\n"*/
    return message;
}

char * parsingSTDinArguments() {
    char buffer[100];
    char uid[20];
    char pass[20];

    fgets(buffer, SIZE, stdin);

    sscanf(buffer, "%s %s %s", command, uid, pass);

    //printf("command: %s\tUID: %s\tpass: %s\n", command, uid, pass);

    if (strcmp(command, "reg") == 0) {
        strcpy(UID, uid);
        strcpy(password, pass);
        return constructPDMessage("REG", UID, password);
    }
    else if (strcmp(command, "exit") == 0) {
        return constructPDMessage("UNR", UID, password);
    }
    else {
        perror("ERROR command received in stdin invalid\n");
        exit(1);
    }
}

void openSocket(char* IP, char* PORT) {
    fd=socket(AF_INET,SOCK_DGRAM,0);    /*UDP socket*/
    if(fd==-1) {
        /*error*/
        perror("Erro: iniciação da socket!\n");
        printf("Value of errno: %d\n", errno);
        exit(1);
    }

    memset(&hints,0,sizeof hints); 
    hints.ai_family=AF_INET;    /*IPv4*/ 
    hints.ai_socktype=SOCK_DGRAM;   /*UDP socket*/
    printf("IP: %s\n", IP);
    errcode=getaddrinfo(IP,PORT,&hints,&res); 
    if(errcode!=0) {
        /*error*/
        perror("Erro: conexão da socket!\n");
        printf("Value of getaddrinfo() return: %d\n", errcode);
        exit(1);
    }

}

void closeSocket() {
    freeaddrinfo(res); 
    if (close(fd) == -1) {
        /*ERROR*/
        perror("Erro: fecho da socket!\n");
        printf("Value of errno: %d\n", errno);
        exit(1);
    }
}

void reg(char * message){ //VERIFICAR QUANDO MANDA PARA O SERVIDORcd 
    
    n=sendto(fd,message,strlen(message),0,res->ai_addr,res->ai_addrlen); 
    if(n==-1) {
        /*error*/
        perror("Erro: envio de mensagem!\n");
        printf("Value of sendto() return: %ld\n", n);
        exit(1);
    }
}

int readFromSocket(){
    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer_client,128,0,(struct sockaddr*)&addr,&addrlen); 
    if(n==-1) {
        /*error*/
        perror("Erro: recepção de mensagem!\n");
        printf("Value of recvfrom() return: %ld\n", n);
        exit(1);
    }
    //write(1,"echo: ",6);
    write(1,buffer_client,n);

    int flag = strcmp(buffer_client, "RUN OK\n");

    return flag;
}