#ifndef _PD_client_h
#define _PD_client_h
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

char buffer_client[128];
char command[20];
char UID[20];
char password[20];

int fd,errcode;

char IPBuffer[25]; /*guarda o IP da maquina*/
char hostbuffer[256]; 
struct hostent *host_entry; 
int hostname;

ssize_t n;
struct addrinfo hints,*res;
socklen_t addrlen;
struct sockaddr_in addr;


char message[SIZE];
char pdip[20], pdPORT[20], asip[20], asPORT[20];

void reg(char * message);
void parsingArguments(int argc, char * argv[]);
char * parsingSTDinArguments();
int readFromSocket();
void selection();
char* getIPAddress();
char * constructPDMessage(char* command, char* UID, char* pass);
void openSocket(char* IP, char* PORT);
void closeSocket();

#endif