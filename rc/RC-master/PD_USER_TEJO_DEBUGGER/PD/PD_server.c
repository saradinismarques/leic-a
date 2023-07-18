#include "PD_server.h"

void connectServer(char* PORT){
    fd_server=socket(AF_INET,SOCK_DGRAM,0); 
    if(fd_server==-1){ /*error*/
        puts("socket failed");
        exit(1);   //UDP socket
    }
    memset(&hints_server,0,sizeof(hints_server)); 
    hints_server.ai_family=AF_INET; 
    hints_server.ai_socktype=SOCK_DGRAM; // UDP socket 
    hints_server.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,PORT,&hints_server,&res_server); 
    if(errcode!=0){ /*error*/ 
        puts("getaddrinfo failed");
        exit(1);
    }

    n_server=bind(fd_server,res_server->ai_addr, res_server->ai_addrlen); 
    if(n_server==-1){ /*error*/ 
        puts("bind failed");
        exit(1);
    }

}

void closeServer(){
    freeaddrinfo(res_server); 
    close(fd_server);
}

void receiveMessage(){

    addrlen_server=sizeof(addr_server); 
    n_server=recvfrom(fd_server,buffer_server,128,0,(struct sockaddr*)&addr_server,&addrlen_server); 
    if(n_server==-1)/*error*/
    exit(1);

    sscanf(buffer_server, "%s %s %s %s %s", command_as, uid_as, vlc, fop, fname);
    write(1,buffer_server,n_server);
}

void sendStatusReply(char * message){
    n_server=sendto(fd_server,message,strlen(message),0,(struct sockaddr*)&addr_server,addrlen_server);
    if(n_server==-1) {
        /*error*/
        perror("Erro: envio de mensagem!\n");
        printf("Value of sendto() return: %ld\n", n_server);
        exit(1);
    }
}