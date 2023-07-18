#ifndef _constants_h
#define _constants_h

#define DEFAULT_AS_PORT "58010"
#define MAX_CLIENTS 20

/*Variavel de controlo para o Verbose mode*/
int isVerbose;
char workingIP[30];

char pdip[20];
char pdport[20];
int timeoutflag;

int getPortFromFD(int fd);
char* getIPFromFD(int fd);
void killServer();

#endif