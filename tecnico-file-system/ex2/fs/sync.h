#ifndef SYNC_H
#define SYNC

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void lock_init(pthread_rwlock_t* rwlock);
void lock_destroy(pthread_rwlock_t* rwlock);
void lock_rdlock(pthread_rwlock_t* rwlock);
void lock_wrlock(pthread_rwlock_t* rwlock);
void lock_unlock(pthread_rwlock_t* rwlock);
void mutex_init(pthread_mutex_t* mutex);
void mutex_destroy(pthread_mutex_t* mutex);
void mutex_lock(pthread_mutex_t* mutex);
void mutex_unlock(pthread_mutex_t* mutex);
void cond_init(pthread_cond_t* cond);
void cond_destroy(pthread_cond_t* cond);

#endif /* SYNC_H */