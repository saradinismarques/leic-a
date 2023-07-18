#ifndef _AS_UDP_Server_h
#define _AS_UDP_Server_h

#include "constants.h"

#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <netdb.h> 
#include <errno.h> 

void openUDPServer(char* PORT);
void readFromUDPSocket();
void writeToUDPSocket(char * message);
void closeUDPServer();

int udp_server_fd,errcode_udp;
ssize_t n_server_udp;
socklen_t addrlen_udp_server;
struct addrinfo hints_udp_server,*res_udp_server;
struct sockaddr_in addr_udp_server;
char buffer_udp_server[128];
#endif