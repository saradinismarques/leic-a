#ifndef USER_TCP
#define USER_TCP

#include "user_app.h"

struct addrinfo *res;

int connectToTCPSocket();
void sendTCPMessage(int fd, char *message, int nbytes);
void readFromTCPSocket(int fd);
void readTCPMessage(int fd, char *buffer, int nbytes);
void closeTCPSocket(int fd);

#endif