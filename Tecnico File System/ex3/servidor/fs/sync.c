#include "sync.h"
#include <errno.h>

void lock_init(pthread_rwlock_t* rwlock)
{
    int ret;
    
    ret = pthread_rwlock_init(rwlock, NULL);
    if (ret != 0)
    {
        perror("rwlock_init error\n");
        exit(EXIT_FAILURE);
    }

};

void lock_destroy(pthread_rwlock_t* rwlock)
{
    int ret;

    ret = pthread_rwlock_destroy(rwlock);
    if (ret != 0)
    {
        perror("rwlock_destroy error\n");
        exit(EXIT_FAILURE);
    }

};

void lock_rdlock(pthread_rwlock_t* rwlock)
{
    int ret;

    ret = pthread_rwlock_rdlock(rwlock);
    if (ret != 0)
    {
        perror("rwlock_rdlock error\n");
        exit(EXIT_FAILURE);
    }


};

void lock_wrlock(pthread_rwlock_t* rwlock)
{
    int ret;

    ret = pthread_rwlock_wrlock(rwlock);
    if (ret != 0)
    {
        perror("rwlock_wrlock error\n");
        exit(EXIT_FAILURE);
    }

};

void lock_unlock(pthread_rwlock_t* rwlock)
{
    int ret;

    ret = pthread_rwlock_unlock(rwlock);
    if (ret != 0)
    {
        perror("rwlock_unlock error\n");
        exit(EXIT_FAILURE);
    }

    
};

void mutex_init(pthread_mutex_t* mutex)
{
    int ret;
    
    ret = pthread_mutex_init(mutex, NULL);
    if (ret != 0)
    {
        perror("mutex_init error\n");
        exit(EXIT_FAILURE);
    }
};

void mutex_destroy(pthread_mutex_t* mutex)
{
    int ret;
    
    ret = pthread_mutex_destroy(mutex);

    if (ret != 0)
    {
        perror("mutex_init error\n");
        exit(EXIT_FAILURE);
    }
};

void mutex_lock(pthread_mutex_t* mutex){
    int ret;

    ret = pthread_mutex_lock(mutex);

    if (ret != 0)
    {
        perror("mutex_lock error\n");
        exit(EXIT_FAILURE);
    }
};

void mutex_unlock(pthread_mutex_t* mutex){
    int ret;

    ret = pthread_mutex_unlock(mutex);

    if (ret != 0)
    {
        perror("mutex_unlock error\n");
        exit(EXIT_FAILURE);
    }
};


