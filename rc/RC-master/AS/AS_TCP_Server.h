#ifndef _AS_TCP_Server_h
#define _AS_TCP_Server_h
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h> 
#include "constants.h"

void openTCPServer(char* PORT);
void acceptNewTCPConnection(int index);
ssize_t readFromTCPConnection(int fd, int index);
void writeToTCPConnection(int fd, char* message);
void closeAllTCPConnections();
void incrementTCPConnectionsCounter();


int tcp_server_fd, errcode_tcp;
int tcp_server_connections[MAX_CLIENTS];

int tcpConnectionsCounter;

ssize_t n_server_tcp;
struct addrinfo hints_tcp_server, *res_tcp_server;
struct sockaddr_in addr_tcp_server[MAX_CLIENTS];
char buffer_server_tcp[128];

#endif