#ifndef SYNC_H
#define SYNC

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void lock_init();
void lock_destroy();
void lock_rdlock();
void lock_wrlock();
void lock_unlock();

#endif /* SYNC_H */