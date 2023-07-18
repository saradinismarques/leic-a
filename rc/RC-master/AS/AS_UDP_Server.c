#include "AS_UDP_Server.h"
#include "constants.h"

void openUDPServer(char* PORT) {
    udp_server_fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(udp_server_fd==-1){
        /*error*/
        //perror("ERROR creating UDPServerSocket!\n");
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Server: ");
        exit(EXIT_FAILURE);
    }

    memset(&hints_udp_server,0,sizeof(hints_udp_server)); 
    hints_udp_server.ai_family=AF_INET; 
    hints_udp_server.ai_socktype=SOCK_DGRAM; // UDP socket 
    hints_udp_server.ai_flags=AI_PASSIVE;

    errcode_udp=getaddrinfo(NULL, PORT, &hints_udp_server, &res_udp_server); 
    if(errcode_udp!=0){
        /*error*/
        fprintf(stderr, " UDP Server getaddrinfo: %s\n", gai_strerror(errcode_udp));
        exit(EXIT_FAILURE);
    }

    n_server_udp=bind(udp_server_fd,res_udp_server->ai_addr, res_udp_server->ai_addrlen); 
    if(n_server_udp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Server: ");
        exit(EXIT_FAILURE);
    }
}

void readFromUDPSocket() {
    addrlen_udp_server=sizeof(addr_udp_server); 
    n_server_udp=recvfrom(udp_server_fd,buffer_udp_server,128,0,(struct sockaddr*)&addr_udp_server,&addrlen_udp_server); 
    if(n_server_udp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Server: ");
        exit(EXIT_FAILURE);
    }

    if(isVerbose) {
        printf("Message Received: %s\tUDP Server\tfrom IP: %s\tin PORT: %d\n", buffer_udp_server,
             inet_ntoa(addr_udp_server.sin_addr), ntohs(addr_udp_server.sin_port));
    }
}

void writeToUDPSocket(char * message) {
    n_server_udp=sendto(udp_server_fd,message,strlen(message),0,(struct sockaddr*)&addr_udp_server,addrlen_udp_server);
    if(n_server_udp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error UDP Server: ");
        exit(EXIT_FAILURE);
    }
    if(isVerbose) {
        printf("Message Sent: %s\tUDP Server Reply\tto IP: %s\tto PORT: %d\n", message,
             inet_ntoa(addr_udp_server.sin_addr), ntohs(addr_udp_server.sin_port));
    }
}

void closeUDPServer() {
    freeaddrinfo(res_udp_server); 
    close(udp_server_fd);
}
