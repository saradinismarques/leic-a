/**
 * 1 Projeto Sistemas Operativos 2020/2021 1 Semestre
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
#include <sys/time.h>
#include <pthread.h>

#define MAX_COMMANDS 150000
#define MAX_INPUT_SIZE 100

char *inputFile = NULL;
char *outputFile = NULL;
int numberThreads = 0;
pthread_mutex_t commandlock;

/* nosync : 0 ;mutex = 1 ; rwlock = 2*/
int syncFlag = 0;

char inputCommands[MAX_COMMANDS][MAX_INPUT_SIZE];
int numberCommands = 0;
int headQueue = 0;

void processArgs(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Invalid Format!\n");
        printf("The correct usage would be: %s inputFile outputFile numThreads synchStrategy\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inputFile = argv[1];
    outputFile = argv[2];
    numberThreads = atoi(argv[3]);

    if (numberThreads < 1)
    {
        fprintf(stderr, "Invalid number of threads\n");
        printf("Number of threads must be an integer higher than 1\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(argv[4], "nosync") == 0 && numberThreads == 1)
        syncFlag = 0;
    else if(strcmp(argv[4], "nosync") == 0 && numberThreads != 1)
    {
        fprintf(stderr, "Invalid nosync!\n");
        printf("nosync is used for 1 thread only\n");
        exit(EXIT_FAILURE);
    }    
    else if (strcmp(argv[4], "mutex") == 0)
        syncFlag = 1;
    else if (strcmp(argv[4], "rwlock") == 0)
        syncFlag = 2;
    else
    {
        fprintf(stderr, "Invalid Sync Type!\n");
        printf("The correct options are: nosync, mutex or rwlock\n");
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

int insertCommand(char *data)
{
    if (numberCommands != MAX_COMMANDS)
    {
        strcpy(inputCommands[numberCommands++], data);
        return 1;
    }
    return 0;
}

char *removeCommand()
{
    if (numberCommands > 0)
    {
        numberCommands--;
        return inputCommands[headQueue++];
    }
    return NULL;
}

void errorParse()
{
    fprintf(stderr, "Error: command invalid\n");
    exit(EXIT_FAILURE);
}

void processInput()
{
    FILE *fp = openFile(inputFile, "r");
    char line[MAX_INPUT_SIZE];

    /* break loop with ^Z or ^D */
    while (fgets(line, sizeof(line) / sizeof(char), fp))
    {
        char token, type;
        char name[MAX_INPUT_SIZE];

        int numTokens = sscanf(line, "%c %s %c", &token, name, &type);

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
            return;

        case 'l':
            if (numTokens != 2)
                errorParse();
            if (insertCommand(line))
                break;
            return;

        case 'd':
            if (numTokens != 2)
                errorParse();
            if (insertCommand(line))
                break;
            return;

        case '#':
            break;

        default:
        { /* error */
            errorParse();
        }
        }
    }
    if(fclose(fp) != 0){
        perror("fclose error\n");
        exit(EXIT_FAILURE);
    } 
}

void* applyCommands()
{
    while (numberCommands > 0)
    {
        int ret = pthread_mutex_lock(&commandlock);
        if (ret != 0)
        {
            perror("mutex_lock error\n");
            exit(EXIT_FAILURE);
        }

        const char *command = removeCommand();
        ret = pthread_mutex_unlock(&commandlock);
        if (ret != 0)
        {
            perror("mutex_unlock error\n");
            exit(EXIT_FAILURE);
        }

        if (command == NULL)
        {
            continue;
        }

        char token, type;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        if (numTokens < 2)
        {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
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
            searchResult = lookup(name, 1);
            if (searchResult >= 0)
                printf("Search: %s found\n", name);
            else
                printf("Search: %s not found\n", name);
            break;
        case 'd':
            printf("Delete: %s\n", name);
            delete (name);
            break;
        default:
        { /* error */
            fprintf(stderr, "Error: command to apply\n");
            exit(EXIT_FAILURE);
        }
        }
    }
    return NULL;
}

void* asyncApplyCommands()
{
    while (numberCommands > 0)
    {
        const char *command = removeCommand();
        
        if (command == NULL)
        {
            continue;
        }

        char token, type;
        char name[MAX_INPUT_SIZE];
        int numTokens = sscanf(command, "%c %s %c", &token, name, &type);
        if (numTokens < 2)
        {
            fprintf(stderr, "Error: invalid command in Queue\n");
            exit(EXIT_FAILURE);
        }

        int searchResult;
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
            searchResult = lookup(name, 1);
            if (searchResult >= 0)
                printf("Search: %s found\n", name);
            else
                printf("Search: %s not found\n", name);
            break;
        case 'd':
            printf("Delete: %s\n", name);
            delete (name);
            break;
        default:
        { /* error */
            fprintf(stderr, "Error: command to apply\n");
            exit(EXIT_FAILURE);
        }
        }
    }
    return NULL;
}

void createThreads()
{
    if (syncFlag != 0)
    {
        pthread_t *threads = (pthread_t *)malloc(numberThreads * sizeof(pthread_t));
        int i = 0;
        for (i = 0; i < numberThreads; i++)
        {
            int err = pthread_create(&threads[i], NULL, applyCommands, NULL);
            if (err != 0)
            {
                perror("Error creating thread");
                exit(EXIT_FAILURE);
            }
        }
        for (i = 0; i < numberThreads; i++)
        {
            if (pthread_join(threads[i], NULL))
            {
                perror("Error joining thread");
            }
        }
        free(threads);
    }
    else
        asyncApplyCommands();
}

int main(int argc, char *argv[])
{
    struct timeval begin, end;
    double elapsedTime;
    int ret;

    gettimeofday(&begin, NULL);
    processArgs(argc, argv);
    
    FILE *fp = openFile(outputFile, "w");

    /* init filesystem */
    init_fs();

    /* process input and print tree */
    processInput();

    if(syncFlag != 0){
        ret = pthread_mutex_init(&commandlock, NULL);

        if (ret != 0)
        {
            perror("mutex_init error\n");
            exit(EXIT_FAILURE);
        }
    }

    createThreads();

    if(syncFlag != 0){
        ret = pthread_mutex_destroy(&commandlock);
        if (ret != 0)
        {
            perror("mutex_destroy error\n");
            exit(EXIT_FAILURE);
        }
    }

    print_tecnicofs_tree(fp);

    if(fflush(fp) != 0){
        perror("fflush error\n");
        exit(EXIT_FAILURE);
    }    
    if(fclose(fp) != 0){
        perror("fclose error\n");
        exit(EXIT_FAILURE);
    }   

    /* release allocated memory */
    destroy_fs();
    
    gettimeofday(&end, NULL);
    elapsedTime = (end.tv_sec - begin.tv_sec) + 
              ((end.tv_usec - begin.tv_usec)/1000000.0);
    
    printf("TecnicoFS completed in %.4f seconds.\n", elapsedTime);

    exit(EXIT_SUCCESS);
}
