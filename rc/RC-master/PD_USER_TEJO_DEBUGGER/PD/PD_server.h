#ifndef _PD_server_h
#define _PD_server_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h> 
#include "constants.h"

int fd_server,errcode;
ssize_t n_server;
socklen_t addrlen_server;
struct addrinfo hints_server,*res_server;
struct sockaddr_in addr_server;
char buffer_server[128];
char command_as[20], uid_as[20], vlc[20], fop[20], fname[20];

void connectServer(char* PORT);
void closeServer();
void receiveMessage();
void sendStatusReply(char * message);

#endif