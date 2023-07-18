#ifndef _FS_TCP_Server_h
#define _FS_TCP_Server_h

#include "constants.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

int tcp_server_fd, errcode_tcp;
int tcp_server_connection;
int tcpConnectionsCounter;

int tcp_server_connections[MAX_CLIENTS];
int n_bytes_upload;

ssize_t n_server_tcp;
socklen_t addrlen_tcp_server;
struct addrinfo hints_tcp_server, *res_tcp_server;
struct sockaddr_in addr_tcp_server;
char buffer_server_tcp[128];


int getPortFromFD(int fd);
char* getIPFromFD(int fd);

void openTCPServer(char* PORT);
void acceptNewTCPConnection(int index);
ssize_t readFromTCPConnection(int fd);
void writeToTCPConnection(int fd, char* message);
void closeAllTCPConnections();

#endif