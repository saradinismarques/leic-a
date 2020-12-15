#include "sync.h"

extern pthread_mutex_t mutexlock;
extern pthread_rwlock_t rwlock;
extern int syncFlag;

void lock_init()
{
    int ret;
    switch (syncFlag)
    {
    case 0:
        break;
    case 1:
        ret = pthread_mutex_init(&mutexlock, NULL);
        if (ret != 0)
        {
            perror("mutex_init error\n");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        ret = pthread_rwlock_init(&rwlock, NULL);
        if (ret != 0)
        {
            perror("rwlock_init error\n");
            exit(EXIT_FAILURE);
        }
        break;
    }
};

void lock_destroy()
{
    int ret;
    switch (syncFlag)
    {
    case 0:
        break;
    case 1:
        ret = pthread_mutex_destroy(&mutexlock);
        if (ret != 0)
        {
            perror("mutex_destroy error\n");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        ret = pthread_rwlock_destroy(&rwlock);
        if (ret != 0)
        {
            perror("rwlock_destroy error\n");
            exit(EXIT_FAILURE);
        }
        break;
    }
};

void lock_rdlock()
{
    int ret;
    switch (syncFlag)
    {
    case 0:
        break;
    case 1:
        ret = pthread_mutex_lock(&mutexlock);
        if (ret != 0)
        {
            perror("mutex_lock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        ret = pthread_rwlock_rdlock(&rwlock);
        if (ret != 0)
        {
            perror("rwlock_rdlock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    }
};

void lock_wrlock()
{
    int ret;
    switch (syncFlag)
    {
    case 0:
        break;
    case 1:
        ret = pthread_mutex_lock(&mutexlock);
        if (ret != 0)
        {
            perror("mutex_lock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        ret = pthread_rwlock_wrlock(&rwlock);
        if (ret != 0)
        {
            perror("rwlock_wrlock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    }
};

void lock_unlock()
{
    int ret;
    switch (syncFlag)
    {
    case 0:
        break;
    case 1:
        ret = pthread_mutex_unlock(&mutexlock);
        if (ret != 0)
        {
            perror("mutex_unlock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    case 2:
        ret = pthread_rwlock_unlock(&rwlock);
        if (ret != 0)
        {
            perror("rwlock_unlock error\n");
            exit(EXIT_FAILURE);
        }
        break;
    }
};
