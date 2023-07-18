#ifndef _AS_UDP_Client_h
#define _AS_UDP_Client_h

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "constants.h"

int as_udp_client_fd,errcode_as_udp_client;
ssize_t n_as_udp_client;
struct addrinfo hints_as_udp_client,*res_as_udp_client;
char buffer_client[128];
socklen_t addrlen_as_udp_client;
struct sockaddr_in addr_as_udp_client;

void openUDPClientSocket(char * IP, char * PORT);
void sendMessageByUDPSocket(char* ip, char* port, char* message);
void closeUDPClientSocket();
void readFromUDPClientSocket(char* ip, char* port);

#endif