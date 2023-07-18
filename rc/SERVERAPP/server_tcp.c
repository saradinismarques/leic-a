#include "server_app.h"

struct addrinfo *res;

int connectToTCPSocket() {
    int fd, errcode;
    struct addrinfo hints;
    ssize_t n;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("Error creating new socket\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;        //IPv4
    hints.ai_socktype = SOCK_STREAM;  //TCP socket
	hints.ai_flags=AI_PASSIVE;

    errcode = getaddrinfo(NULL, DSport, &hints, &res); 
    if(errcode!=0) {
        perror("Error getting address information for socket\n");
        exit(1);
    }
	n = bind(fd, res->ai_addr, res->ai_addrlen);
	if(n == -1) {
        perror("Error on bind\n");
        exit(1);
    }
	if(listen(fd, MAX_CONNECTIONS) == -1) {
        perror("Error listening to connections\n");
        exit(1);
    }
    return fd;
}

void sendTCPMessage(int fd, char *message, ssize_t nbytes) {
    ssize_t nleft, nwritten;
    char *ptr;

    nleft = nbytes;
    ptr = message;

    while(nleft > 0) {
        nwritten = write(fd, ptr, nleft);
        if(nwritten <= 0) {
            perror("Error sending message to socket\n");
            exit(1);
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
}

void readTCPMessage(int fd, char *buffer, int nbytes) {
    ssize_t nleft, nread;
    char *ptr;

    memset(buffer, 0, strlen(buffer));

    nleft = nbytes;
    ptr = buffer; 

    while (nleft > 0) {
        nread = read(fd, ptr, nleft);
        if (nread == -1) {
            perror("Error reading message from socket\n");
            exit(1);
        } else if (nread == 0)
            break; // closed by peer

        nleft -= nread;
        ptr += nread;
    }
    buffer[nbytes] = '\0';
}

void closeTCPSocket(int fd) {
    freeaddrinfo(res);
    close(fd);    
}
