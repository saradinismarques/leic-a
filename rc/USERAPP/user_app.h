#ifndef USER_APP
#define USER_APP

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>

#define DEFAULT_DS_PORT "58028" /* 58000 + group name */

#define IP_SIZE 50
#define PORT_SIZE 6

#define BUFFER_SIZE 512
#define MESSAGE_SIZE 512
#define LIST_SIZE 4000

#define UID_SIZE 6
#define PASS_SIZE 9
#define GID_SIZE 3
#define GNAME_SIZE 25
#define MID_SIZE 5
#define TEXT_SIZE 241
#define TEXT_DIGITS 4
#define FNAME_SIZE 25
#define FILE_DIGITS 11
#define EXT_SIZE 4
#define ARG1_SIZE 241
#define ARG2_SIZE 25

#define COMMAND_SIZE 12

#define SUCCESS 1
#define ERROR 0
#define CONTINUE 1
#define EXIT 0

char DSIP[IP_SIZE], DSport[PORT_SIZE];

char UID_login[UID_SIZE];
char pass_login[PASS_SIZE];
int isLoggedIn;
char GID_selected[GID_SIZE];

char *getIPAddress();
void parseArguments(int argc, char *argv[]);

int applyCommands();
void constructMessage(char *message, char *arg1, char *arg2, char *arg3, char *arg4, char *arg5, char *arg6, char *arg7);
void checkReceivedMessage(int fd, char *message);

void reg(char *arg1, char *arg2);
void unregister(char *arg1, char *arg2);
void login(char *arg1, char *arg2);
void logout();
void showuid();
void groups();
void subscribe(char *arg1, char *arg2);
void unsubscribe(char *arg1);
void my_groups();
void select_(char *arg1);
void showgid();
void ulist();
void post(char *arg1, char *arg2);
void retrieve(char *arg1); 

#endif