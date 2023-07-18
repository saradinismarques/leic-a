#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "FS_TCP_Server.h"
#include <errno.h> 



int getPortFromFD(int fd) {
    struct sockaddr_in addr;
    int n;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    n = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    if (n==-1) {
        
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error Getting Port: ");
        killServer();
    }
    return ntohs(addr.sin_port);
}


char* getIPFromFD(int fd) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int n;
    n = getpeername(fd, (struct sockaddr *)&addr, &addr_size);
    if (n==-1) {

        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error Getting IP: ");
        killServer();
    }
    strcpy(workingIP, inet_ntoa(addr.sin_addr));
    return workingIP;
}

/*Initializes the TCP Server*/
void openTCPServer(char* PORT) {
    tcp_server_fd=socket(AF_INET,SOCK_STREAM,0);     //TCP socket
    if (tcp_server_fd==-1) {
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error TCP Socket Server: ");
        killServer();
    }

    memset(&hints_tcp_server,0,sizeof(hints_tcp_server));
    hints_tcp_server.ai_family=AF_INET;         //IPv4
    hints_tcp_server.ai_socktype=SOCK_STREAM;     //TCP socket
    hints_tcp_server.ai_flags=AI_PASSIVE; 

    errcode_tcp=getaddrinfo(NULL,PORT,&hints_tcp_server,&res_tcp_server);
    if((errcode_tcp)!=0){
        /*!error*/
        fprintf(stderr, " TCP Server getaddrinfo: %s\n", gai_strerror(errcode_tcp));
        exit(EXIT_FAILURE);
    }
    n_server_tcp=bind(tcp_server_fd,res_tcp_server->ai_addr,res_tcp_server->ai_addrlen);
    if(n_server_tcp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error TCP bind Server: ");
        exit(EXIT_FAILURE);
    }

    if(listen(tcp_server_fd,5)==-1){
        /*error*/
        perror("ERROR listen TCP Socket Server!\n"); 
        exit(EXIT_FAILURE);
    }
}

/*Server recieved a new connection, accepts it. Returns the index of the opened file descriptor*/
void acceptNewTCPConnection(int index) {
    if((tcp_server_connections[index]=accept(tcp_server_fd,(struct sockaddr*)&addr_tcp_server,
        &addrlen_tcp_server))==-1) {
            /*error*/
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error TCP Server: ");
            killServer();
    }
}


ssize_t readFromTCPConnection(int fd) {
    n_server_tcp= read(fd,buffer_server_tcp,128);
    n_bytes_upload = n_server_tcp;
    if(n_server_tcp==-1){
        /*error*/
        if(errno ==32){
            return n_server_tcp;
        }
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error TCP Server: ");
        killServer();
    }
    if (n_server_tcp == 0) {
        return 0;
    }
   if(isVerbose) {
        printf("Message Received: %s\tin TCP Server\tfrom IP: %s\tin PORT: %d\n", buffer_server_tcp,
             getIPFromFD(fd), getPortFromFD(fd));
    }
   
    /*write para debugging, escreve o que recebeu na socket*/
    write(1,"received: ",10);
    write(1,buffer_server_tcp,n_server_tcp);
    return n_server_tcp;
}

void writeToTCPConnection(int fd, char* message) {
    if(isVerbose) {
        printf("Message Sent: %s\tto TCP Connection\tto IP: %s\tto PORT: %d\n", message,
        getIPFromFD(fd), getPortFromFD(fd));
    }
    n_server_tcp=write(fd,message,strlen(message));
    if(n_server_tcp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error TCP Server: ");
        killServer();
    }

}

void closeAllTCPConnections() {
     for (int i = 0; i < MAX_CLIENTS; i++) {
        if(tcp_server_connections[i]!=-1)
            n_server_tcp = close(tcp_server_connections[i]);
        if(n_server_tcp==-1){
            /*error*/
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error closing TCP connections: ");
            killServer();
        }
    }

    freeaddrinfo(res_tcp_server);
    n_server_tcp = close (tcp_server_fd);
    if(n_server_tcp==-1){
        /*error*/
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error closing TCP Server: ");
        killServer();
    }
}
