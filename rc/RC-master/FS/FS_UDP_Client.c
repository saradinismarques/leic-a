#include "FS_UDP_Client.h"

/**
 * Quando quiser mandar uma mensagem 
 * abre-se uma socket UDP com a PORT e o IP
 * do user que se está a servir
 * logo de seguida manda a mensagem e depois das verificações
 * aplicáveis, fecha a socket
 * 
 * Volta a abir quando precisar de mandar outra mensagem
*/

void openUDPClientSocket(char * IP, char * PORT) {
    fs_udp_client_fd=socket(AF_INET,SOCK_DGRAM,0);    //UDP socket
    if(fs_udp_client_fd==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Client: ");
        killServer();
    }
    
    memset(&hints_fs_udp_client,0,sizeof(hints_fs_udp_client)); 
    hints_fs_udp_client.ai_family=AF_INET;    //IPv4 
    hints_fs_udp_client.ai_socktype=SOCK_DGRAM;   //UDP socket

    errcode_fs_udp_client=getaddrinfo(IP,PORT,&hints_fs_udp_client,&res_fs_udp_client); 
    if(errcode_fs_udp_client!=0){
        /*error*/
        perror("ERROR getaddrinfo UDPClientSocket!\n"); 
        killServer();
    }
}

void sendMessageByUDPSocket(char* ip, char* port,char* message) {
    n_fs_udp_client=sendto(fs_udp_client_fd,message,strlen(message),0,res_fs_udp_client->ai_addr,res_fs_udp_client->ai_addrlen); 
    if(n_fs_udp_client==-1){
        /*error*/
        fprintf(stderr, " UDP Client getaddrinfo: %s\n", gai_strerror(errcode_fs_udp_client));
        killServer();
    }
    if(isVerbose) {
        printf("Message Sent: %s\tUDP Client\tto IP: %s\tto PORT: %s\n", message,
               ip, port);
    }
}

void closeUDPClientSocket() {
    freeaddrinfo(res_fs_udp_client); 
    close(fs_udp_client_fd);
}


void readFromUDPClientSocket(char* ip, char* port){

    addrlen_fs_udp_client=sizeof(addr_fs_udp_client);
    n_fs_udp_client=recvfrom(fs_udp_client_fd,buffer_udp_client,128,0,(struct sockaddr*)&addr_fs_udp_client,&addrlen_fs_udp_client); 

    if(n_fs_udp_client==-1) {
        /*error*/
        perror("Erro: recepção de mensagem!\n");
        printf("Value of recvfrom() return: %ld\n", n_fs_udp_client);
        killServer();
    }
    if(isVerbose) {
        printf("Message Recieve: %s\tUDP Client Socket Response\tfrom IP: %s\tin PORT: %s\n", buffer_udp_client,
               ip, port);
    }

}

