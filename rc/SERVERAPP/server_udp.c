#include "server_app.h"

struct addrinfo *res;
socklen_t addrlen;
struct sockaddr_in addr;

int connectToUDPSocket() {
    int fd, errcode;
    struct addrinfo hints;
    ssize_t n;

    fd = socket(AF_INET, SOCK_DGRAM, 0);       //UDP socket
    if(fd == -1) {
        perror("Error creating new socket\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);

    hints.ai_family=AF_INET;      // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL, DSport, &hints, &res); 
    if(errcode!=0) {
        perror("Error getting address information for socket\n");
        exit(1);
    }
    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if(n == -1) {
        perror("Error on bind\n");
        exit(1);
    }
    return fd;
}

void sendUDPMessage(int fd, char *message) {
    ssize_t nsent;

    nsent = sendto(fd, message, strlen(message), 0, (struct sockaddr*)&addr, addrlen);
    
    if(nsent == -1) {
        perror("Error sending message to socket\n");
        exit(1);
    }
}

void readUDPMessage(int fd, char *buffer) { 
    ssize_t nread;

    memset(buffer, 0, strlen(buffer));

    addrlen = sizeof(addr);
    nread = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    
    if(nread == -1) {
        perror("Error reading message from socket\n");
        exit(1);
    }
}

void closeUDPSocket(int fd) {
    freeaddrinfo(res);
    close(fd);    
}