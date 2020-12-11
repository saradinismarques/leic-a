#include "tecnicofs-client-api.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#define MAX_INPUT_SIZE 100
int sockfd;
socklen_t servlen, clilen;
struct sockaddr_un serv_addr, client_addr;
char buffer[1024];
char cliente[100];


int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

int tfsCreate(char *filename, char nodeType) {

  char in_buffer[MAX_INPUT_SIZE];
  char out_buffer[MAX_INPUT_SIZE];
  int ret;
  
  sprintf(in_buffer,"c %s %c",filename,nodeType);
  if (sendto(sockfd, in_buffer, strlen(in_buffer), 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  } 

  if (recvfrom(sockfd, out_buffer, sizeof(out_buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  } 
  ret = atoi(out_buffer);
  return ret;
}

int tfsDelete(char *path) {
  char in_buffer[MAX_INPUT_SIZE];
  char out_buffer[MAX_INPUT_SIZE];
  int ret;

  sprintf(in_buffer,"d %s",path);
  if (sendto(sockfd, in_buffer, strlen(in_buffer), 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  } 

  if (recvfrom(sockfd, out_buffer, sizeof(out_buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  } 
  ret = atoi(out_buffer);
  return ret;
}

int tfsMove(char *from, char *to) {
  char in_buffer[MAX_INPUT_SIZE];
  char out_buffer[MAX_INPUT_SIZE];
  int ret;

  sprintf(in_buffer,"m %s %s",from,to);
  if (sendto(sockfd, in_buffer, strlen(in_buffer), 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  } 

  if (recvfrom(sockfd, out_buffer, sizeof(out_buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  } 
  ret = atoi(out_buffer);
  return ret;
}

int tfsLookup(char *path) {
  char in_buffer[MAX_INPUT_SIZE];
  char out_buffer[MAX_INPUT_SIZE];
  int ret;

  sprintf(in_buffer,"l %s",path);
  if (sendto(sockfd, in_buffer, strlen(in_buffer), 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  } 

  if (recvfrom(sockfd, out_buffer, sizeof(out_buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  } 
  ret = atoi(out_buffer);
  return ret;
}

int tfsPrint(char * path){
  char in_buffer[MAX_INPUT_SIZE];
  char out_buffer[MAX_INPUT_SIZE];
  int ret;

  sprintf(in_buffer,"p %s",path);
  if (sendto(sockfd, in_buffer, strlen(in_buffer), 0, (struct sockaddr *) &serv_addr, servlen) < 0) {
    perror("client: sendto error");
    exit(EXIT_FAILURE);
  } 

  if (recvfrom(sockfd, out_buffer, sizeof(out_buffer), 0, 0, 0) < 0) {
    perror("client: recvfrom error");
    exit(EXIT_FAILURE);
  } 
  ret = atoi(out_buffer);
  return ret;
}

int tfsMount(char * serverName) {
  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0) ) < 0) {
    perror("client: can't open socket");
    exit(EXIT_FAILURE);
  }
  unlink(cliente);
  clilen = setSockAddrUn (cliente, &client_addr);
  if (bind(sockfd, (struct sockaddr *) &client_addr, clilen) < 0) {
    perror("client: bind error");
    exit(EXIT_FAILURE);
  }  

  servlen = setSockAddrUn(serverName, &serv_addr);
  
  return 0;
}

int tfsUnmount() {
  int ret = close(sockfd);
  if(ret < 0){
    perror("client: close error");
    exit(EXIT_FAILURE);
  }
  unlink(cliente);
  return 1;
}
