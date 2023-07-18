#include "user_app.h"

#define UDP_BUFFER_SIZE 4000

struct addrinfo *res;

int connectToUDPSocket() {
    int fd, errcode;
    struct addrinfo hints;

    fd = socket(AF_INET, SOCK_DGRAM, 0);    //UDP socket
    if(fd == -1) {
        perror("Error creating new socket\n");
        exit(1);
    }
    memset(&hints, 0, sizeof(hints)); 
    hints.ai_family = AF_INET;    //IPv4 
    hints.ai_socktype = SOCK_DGRAM;   //UDP socket

    errcode = getaddrinfo(DSIP, DSport, &hints, &res); 
    if(errcode != 0) {
        perror("Error getting address information for socket\n");
        exit(1);
    }
    return fd;
}

int TimerON(int sd) {
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    tmout.tv_sec=15; /* Wait for 15 sec for a reply from server. */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout, sizeof(struct timeval)));
}

int TimerOFF(int sd) {
    struct timeval tmout;
    memset((char *)&tmout,0,sizeof(tmout)); /* clear time structure */
    return(setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tmout,sizeof(struct timeval)));
}

void sendUDPMessage(int fd, char *message) {
    ssize_t n;

    n = sendto(fd, message, strlen(message), 0, res->ai_addr, res->ai_addrlen); 
    
    if(n == -1) {
        perror("Error sending message to socket\n");
        exit(1);
    }
}

int readUDPMessage(int fd, char *buffer) {
    ssize_t nread;
    socklen_t addrlen;
    struct sockaddr_in addr;
    
    memset(buffer, 0, strlen(buffer));

    addrlen = sizeof(addr);

    TimerON(fd);
    nread = recvfrom(fd, buffer, LIST_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    if(nread < 0) {
        printf("Error time out reached reading from socket\n");
        /* if time out is reached when wainting for reply sends ERROR and user will send udp message again */
        TimerOFF(fd);
        return ERROR; 
    }
    TimerOFF(fd);
    return SUCCESS;
}

void closeUDPSocket(int fd) {
    freeaddrinfo(res);
    close(fd);    
}
