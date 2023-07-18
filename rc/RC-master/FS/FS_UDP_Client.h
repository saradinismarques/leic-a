#ifndef _FS_UDP_Client_h
#define _FS_UDP_Client_h

#include "constants.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>

int fs_udp_client_fd,errcode_fs_udp_client;
ssize_t n_fs_udp_client;
struct addrinfo hints_fs_udp_client,*res_fs_udp_client;
socklen_t addrlen_fs_udp_client;
struct sockaddr_in addr_fs_udp_client;

char buffer_udp_client[SIZE];


void openUDPClientSocket(char * IP, char * PORT);
void sendMessageByUDPSocket(char* ip, char* port, char* message);
void closeUDPClientSocket();
void readFromUDPClientSocket(char* ip, char* port);

#endif