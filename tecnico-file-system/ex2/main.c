/**
 * 2 Projeto Sistemas Operativos 2020/2021 1 Semestre
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

#define TRUE 1
#define MAX_COMMANDS 10
#define MAX_INPUT_SIZE 100

char *inputFile = NULL;
char *outputFile = NULL;
int numThreads;
pthread_mutex_t commandlock;
pthread_cond_t canProduce, canConsume;


char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0; 
int tailQueue = 0;

struct timeval begin, end;
double elapsedTime;

void processArgs(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Invalid Format!\n");
        printf("The correct usage would be: %s inputFile outputFile maxThreads\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inputFile = argv[1];
    outputFile = argv[2];
    numThreads = atoi(argv[3]);

    if (numThreads < 1)
    {
        fprintf(stderr, "Invalid number of threads\n");
        printf("Number of threads must be an integer higher than 1\n");
        exit(EXIT_FAILURE);
    }
}

FILE *openFile(char *file, char *flag)
{
    FILE *fp;
    fp = fopen(file, flag);
    if (fp == NULL)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

void closeFile(FILE *file){
    if(fclose(file) != 0){
        perror("fclose error\n");
        exit(EXIT_FAILURE);
    }
}

int insertCommand(char *data)
{
    mutex_lock(&commandlock);

    while(numberCommands == MAX_COMMANDS){
        pthread_cond_wait(&canProduce, &commandlock);
    }
    strcpy(inputCommands[tailQueue], data);
    tailQueue++;
    if(tailQueue == MAX_COMMANDS)
        tailQueue = 0;
    numberCommands++;
    pthread_cond_signal(&canConsume);
    mutex_unlock(&commandlock);

    return 1;
}

char *removeCommand()
{
    char *input;
    mutex_lock(&commandlock);

    while(numberCommands == 0){
        pthread_cond_wait(&canConsume, &commandlock);
    }
    input = inputCommands[headQueue];
    headQueue++;
    if(headQueue == MAX_COMMANDS){
        headQueue = 0;
    }
    if (numberCommands > 0)
    {
        numberCommands--;
        return input;
    }
    return NULL;
}

void errorParse()
{
    fprintf(stderr, "Error: command invalid\n");
    exit(EXIT_FAILURE);
}

void* processInput()
{
    FILE *input = openFile(inputFile, "r");
    char line[MAX_INPUT_SIZE];
    gettimeofday(&begin, NULL);
    
    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line) / sizeof(char), input))
    {
        char token, type;
        char name[MAX_INPUT_SIZE], name2[MAX_INPUT_SIZE];
        int numTokens;

        sscanf(line, "%c", &token);
        if(token != 'm'){
            numTokens = sscanf(line, "%c %s %c", &token, name, &type);
        }
        else{
            numTokens = sscanf(line, "%c %s %s", &token, name, name2);
        }

        /* perform minimal validation */
        if (numTokens < 1)
        {
            continue;
        }
        switch (token)
        {
        case 'c':
            if (numTokens != 3)
                errorParse();
            if (insertCommand(line))
                break;
            return NULL;

        case 'l':
            if (numTokens != 2)
                errorParse();
            if (insertCommand(line))
                break;
            return NULL;

        case 'd':
            if (numTokens != 2)
                errorParse();
            if (insertCommand(line))
                break;
            return NULL;

        case 'm':
            if (numTokens != 3)
                errorParse();
            if (insertCommand(line))
                break;
            return NULL;

        case '#':
            break;

        default:
        { /* error */
            errorParse();
        }
        }
    }
    closeFile(input);
    for (size_t i = 0; i < numThreads; i++)
    {      
        insertCommand("q exit\n");
    }
    
    return NULL;
}

void* applyCommands()
{   
    while (TRUE)
    {
  
        const char *command = removeCommand();

        char token, type;
        char name[MAX_INPUT_SIZE], name2[MAX_INPUT_SIZE];
        int numTokens;
        
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
        pthread_cond_signal(&canProduce);
        mutex_unlock(&commandlock);
        int searchResult;

        int unlock_array[INODE_TABLE_SIZE];

        switch (token)
        {
        case 'c':
            switch (type)
            {
            case 'f':
                printf("Create file: %s\n", name);
                create(name, T_FILE);
                break;
            case 'd':
                printf("Create directory: %s\n", name);
                create(name, T_DIRECTORY);
                break;
            default:
                fprintf(stderr, "Error: invalid node type\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 'l':
            
            for(int i = 0; i < INODE_TABLE_SIZE; i++)
                unlock_array[i] = 0;
            searchResult = lookup(name, unlock_array, 0);
            if (searchResult >= 0)
                printf("Search: %s found\n", name);
            else
                printf("Search: %s not found\n", name);
            break;

        case 'd':
            printf("Delete: %s\n", name);
            delete(name);
            break;

        case 'm':
            printf("Move: %s to %s\n", name, name2);
            move(name, name2);
            break;

        case 'q': 
            return NULL;

        default:
        { /* error */
            fprintf(stderr, "Error: command to apply\n");
            exit(EXIT_FAILURE);
        }
        }
    }

    return NULL;
}

void init_sync(){
    mutex_init(&commandlock);
    cond_init(&canProduce);
    cond_init(&canConsume);
}

void destroy_sync(){
    mutex_destroy(&commandlock);
    cond_destroy(&canProduce);
    cond_destroy(&canConsume);
}

void createThreads()
{
       
    pthread_t *threads = (pthread_t *)malloc((numThreads + 1) * sizeof(pthread_t));
    int i, err;

    err = pthread_create(&threads[0], NULL, processInput, NULL);
    if (err != 0)
    {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }

    for (i = 1; i < numThreads + 1; i++)
    {
        err = pthread_create(&threads[i], NULL, applyCommands, NULL);
        if (err != 0)
        {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    
    for (i = 0; i < numThreads + 1 ; i++)
    {
        if (pthread_join(threads[i], NULL))
        {
            perror("Error joining thread");
        }
    }
    free(threads);

    gettimeofday(&end, NULL);
    elapsedTime = (end.tv_sec - begin.tv_sec) + ((end.tv_usec - begin.tv_usec)/1000000.0);
}

int main(int argc, char *argv[])
{
    processArgs(argc, argv);
    
    FILE *output = openFile(outputFile, "w");

    /* init filesystem */
    init_fs();
    init_sync();

    createThreads();

    destroy_sync();
 
    print_tecnicofs_tree(output);
   
    closeFile(output);

    /* release allocated memory */
    destroy_fs();
    
    printf("TecnicoFS completed in %.4f seconds.\n", elapsedTime);

    exit(EXIT_SUCCESS);
}