#include "AS_UDP_Client.h"
#include <errno.h>
#include <stdio.h>

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
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    as_udp_client_fd=socket(AF_INET,SOCK_DGRAM,0);    //UDP socket
    if(as_udp_client_fd==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Client: ");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(as_udp_client_fd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        perror("Error: Couldn't set timeout!");
        killServer();
    }
    
    memset(&hints_as_udp_client,0,sizeof(hints_as_udp_client)); 
    hints_as_udp_client.ai_family=AF_INET;    //IPv4 
    hints_as_udp_client.ai_socktype=SOCK_DGRAM;   //UDP socket

    errcode_as_udp_client=getaddrinfo(IP,PORT,&hints_as_udp_client,&res_as_udp_client); 
    if(errcode_as_udp_client!=0){
        /*error*/
        fprintf(stderr, " UDP Client getaddrinfo: %s\n", gai_strerror(errcode_as_udp_client));
        exit(EXIT_FAILURE);
    }
}

void sendMessageByUDPSocket(char* ip, char* port, char* message) {
    n_as_udp_client=sendto(as_udp_client_fd,message,strlen(message),0,res_as_udp_client->ai_addr,res_as_udp_client->ai_addrlen); 
    if(n_as_udp_client==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Client: ");
        exit(EXIT_FAILURE);
    }

    if(isVerbose) {
        printf("Message Sent: %s\tUDP Client\tto IP: %s\tto PORT: %s\n", message,
             ip, port);
    }
}

void closeUDPClientSocket() {
    freeaddrinfo(res_as_udp_client); 
    close(as_udp_client_fd);
}

void readFromUDPClientSocket(char* ip, char* port){
    addrlen_as_udp_client=sizeof(addr_as_udp_client);

    n_as_udp_client=recvfrom(as_udp_client_fd,buffer_client,128,0,(struct sockaddr*)&addr_as_udp_client,&addrlen_as_udp_client); 
    if(n_as_udp_client==-1) {
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Client recvfrom: ");
        //meter aqui a flag
        timeoutflag = 1;
        return;
    }

    if(isVerbose) {
        printf("Message Recieve: %s\tUDP Client Socket Response\tfrom IP: %s\tin PORT: %s\n", buffer_client,
             ip, port);
    }

}