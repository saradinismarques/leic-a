#ifndef SERVER_APP
#define SERVER_APP

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>

#define DEFAULT_DS_PORT "58028"

#define PORT_SIZE 6

#define BUFFER_SIZE 512
#define MESSAGE_SIZE 512
#define LIST_SIZE 4000

#define MAX_CONNECTIONS 10
#define MAX_USERS 1000
#define MAX_GROUPS 99
#define MAX_MSGS 9999

#define UID_SIZE 6
#define PASS_SIZE 9
#define GID_SIZE 3
#define GNAME_SIZE 25
#define MID_SIZE 5
#define TEXT_SIZE 241
#define TEXT_DIGITS 4
#define FNAME_SIZE 25
#define FILE_DIGITS 11
#define RTV_MSGS 20
#define PATH_MAX_SIZE 1000
#define PATH_SIZE 50
#define UID_FILE 10

#define ARG1_SIZE 6
#define ARG2_SIZE 9
#define ARG3_SIZE 25

#define COMMAND_SIZE 4

#define SUCCESS 1
#define ERROR 0

#define max(x, y) (((x) > (y)) ? (x) : (y))

/* struct that saves a group id, name and number of messages */ 
typedef struct grouplist {
    char gid[GID_SIZE];
    char gname[GNAME_SIZE];
    int no_msgs; 
} GROUPLIST;

char DSport[PORT_SIZE];
int verboseMode;
char workingIP[30];

int no_users;
int no_groups;
GROUPLIST list[MAX_GROUPS];

void parseArguments(int argc, char * argv[]);
int checkReceivedMessage(int fd, char *message);
void constructMessage(char *message, char *arg1, char *arg2, char *arg3, char *arg4);

void convertMIDChar(char *MID, int mid);
void convertGIDChar(char *GID, int gid); 

void reg(int fd, char *UID, char *pass); 
void unregister(int fd, char *UID, char *pass);
void login(int fd, char *UID, char *pass);
void logout(int fd, char *UID, char *pass);
void groups(int fd);
void subscribe(int fd, char *UID, char *GID, char *GName);
void unsubscribe(int fd, char *UID, char *GID);
void my_groups(int fd, char *UID);
void ulist(int fd);
void post(int fd);
void retrieve(int fd);

#endif
