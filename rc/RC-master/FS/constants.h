#ifndef _constants_h
#define _constants_h

#define DEFAULT_FS_PORT "59010"
#define DEFAULT_AS_PORT "58010"
#define SIZE 100
#define MAX_CLIENTS 20

void killServer();

char pdip[20];
char pdport[20];

/*Variavel de controlo para o Verbose mode*/
int isVerbose;
char workingIP[30];

#endif