/**
 * 3 Projeto Sistemas Operativos 2020/2021 1 Semestre
 * Grupo A64:
 * Eduardo Santos 92458
 * Sara Marques 93342
 */

#include <stdio.h>
#include <stdlib.h> 
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include "fs/operations.h"
#include "fs/sync.h"
#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define TRUE 1
#define MAX_COMMANDS 10
#define MAX_INPUT_SIZE 100

char *path;
int sockfd;
int numThreads;
pthread_mutex_t commandlock;

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];

struct timeval begin, end;
double elapsedTime;

int setSockAddrUn(char *path, struct sockaddr_un *addr) {

  if (addr == NULL)
    return 0;

  bzero((char *)addr, sizeof(struct sockaddr_un));
  addr->sun_family = AF_UNIX;
  strcpy(addr->sun_path, path);

  return SUN_LEN(addr);
}

void processArgs(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid Format!\n");
        printf("The correct usage would be: %s inputFile outputFile maxThreads\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numThreads = atoi(argv[1]);
    path = argv[2];


    if (numThreads < 1)
    {
        fprintf(stderr, "Invalid number of threads\n");
        printf("Number of threads must be an integer higher than 1\n");
        exit(EXIT_FAILURE);
    }
}

char *removeCommand(struct sockaddr_un* client_addr, socklen_t* addrlen, char * in_buffer)
{
    int c;
    *addrlen=sizeof(struct sockaddr_un);
    c = recvfrom(sockfd, in_buffer, strlen(in_buffer)-1, 0,
        (struct sockaddr *)client_addr, addrlen);

    //Preventivo, caso o cliente nao tenha terminado a mensagem em '\0', 
    in_buffer[c]='\0';
    return in_buffer;
}

void errorParse()
{
    fprintf(stderr, "Error: command invalid\n");
    exit(EXIT_FAILURE);
}

void* applyCommands()
{   
    while (TRUE)
    {
        struct sockaddr_un client_addr;
        socklen_t addrlen;
        char in_buffer[MAX_INPUT_SIZE];
        char out_buffer[MAX_INPUT_SIZE];
        memset(in_buffer,0,MAX_INPUT_SIZE);
        memset(out_buffer,0,MAX_INPUT_SIZE);
        const char *command = removeCommand(&client_addr,&addrlen, in_buffer);

        //printf("%s\n",in_buffer);

        char token, type;
        char name[MAX_INPUT_SIZE], name2[MAX_INPUT_SIZE];
        int numTokens;
        int unlock_array[INODE_TABLE_SIZE];
        FILE *output;
        int ret;
        
        sscanf(command, "%c", &token);
        if(token != 'm'){
            numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        }
        else{
            numTokens = sscanf(command, "%c %s %s", &token, name, name2);
        }
            
        if (numTokens < 2)
        {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        switch (token)
        {
        case 'c':
            switch (type)
            {
            case 'f':
                printf("Create file: %s\n", name);
                ret = create(name, T_FILE);
                break;
            case 'd':
                printf("Create directory: %s\n", name);
                ret = create(name, T_DIRECTORY);
                break;
            default:
                fprintf(stderr, "Error: invalid node type\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'l':
            
            for(int i = 0; i < INODE_TABLE_SIZE; i++)
                unlock_array[i] = 0;
            ret = lookup(name, unlock_array, 0);
            if (ret >= 0)
                printf("Search: %s found\n", name);
            else
                printf("Search: %s not found\n", name);
            break;

        case 'd':
            printf("Delete: %s\n", name);
            ret = delete(name);
            break;

        case 'm':
            printf("Move: %s to %s\n", name, name2);
            ret = move(name, name2);
            break;

        case 'p':
            output = fopen(name, "w");
            if (output == NULL)
            {
                perror("Error opening file");
                ret = -1;
                break;
            }
            printf("Print: %s\n",name);
            ret = print_tecnicofs_tree(output);
            if(fclose(output) != 0){
                perror("Error closing file\n");
                ret = -1;
                break;
            }           
            break;

        default:
        {  
            fprintf(stderr, "Error: command to apply\n");
            //exit(EXIT_FAILURE);
        }
        }
        sprintf(out_buffer,"%d",ret);
        if (sendto(sockfd, out_buffer, strlen(out_buffer), 0, (struct sockaddr *) &client_addr, addrlen) < 0) {
            perror("server: sendto error");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

void createThreads()
{
       
    pthread_t *threads = (pthread_t *)malloc((numThreads + 1) * sizeof(pthread_t));
    int i, err;

    for (i = 0; i < numThreads; i++)
    {
        err = pthread_create(&threads[i], NULL, applyCommands, NULL);
        if (err != 0)
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    for (i = 0; i < numThreads; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            perror("Error joining thread");
        }
    }
    free(threads);
}

void init_socket(){
    struct sockaddr_un server_addr;
    socklen_t addrlen;
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("server: can't open socket");
        exit(EXIT_FAILURE);
    }

    unlink(path);

    addrlen = setSockAddrUn (path, &server_addr);
    if (bind(sockfd, (struct sockaddr *) &server_addr, addrlen) < 0) {
        perror("server: bind error");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    
    processArgs(argc, argv);

    init_socket();

    init_fs();

    createThreads();
       
    /* release allocated memory */
    destroy_fs();
    
    gettimeofday(&end, NULL);
    elapsedTime = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
    
    printf("TecnicoFS completed in %.4f seconds.\n", elapsedTime);

    exit(EXIT_SUCCESS);
}