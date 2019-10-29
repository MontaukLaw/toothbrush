/**
 * Copyright (c) 2017, Afuiot Tech, Co., Ltd
 * All rights reserverd.
 * File name: lib_fifo.h
 * Description: linux kernel fifo header file.
 * Other: none
 * Version: V1.0
 * History: Version, Writer, modifier, modify time, modify content
             V1.0     wucb     wucb    2018.11.15   first version
             V1.01    wucb     wucb    2019.01.29   add signal lock
 */
#ifndef _LIB_FIFO_H_
#define _LIB_FIFO_H_

#define  LOCK_ENABLE  0

#if 1 == LOCK_ENABLE
#include <pthread.h>
#define LOCK_TYPE				pthread_mutex_t
#define PTHREAD_LOCK_INIT(lock)	pthread_mutex_init(&(lock), NULL)
#define PTHREAD_LOCK(lock)		pthread_mutex_lock(&(lock))
#define PTHREAD_UNLOCK(lock)	pthread_mutex_unlock(&(lock))
#else
#define LOCK_TYPE				int
#define PTHREAD_LOCK_INIT(lock)
#define PTHREAD_LOCK(lock)
#define PTHREAD_UNLOCK(lock)
#endif //1 == LOCK_ENABLE

typedef struct
{
    unsigned char *buffer;
    unsigned int in;
    unsigned int out;
    unsigned int size;
    LOCK_TYPE lock;
} Kfifo;

void KfifoInit(Kfifo *fifo, unsigned char *buffer, unsigned int size);
unsigned int KfifoSize(Kfifo *fifo);
unsigned int KfifoPut(Kfifo *fifo, unsigned char *buffer, unsigned int len);
unsigned int KfifoGet(Kfifo *fifo, unsigned char *buffer, unsigned int len);

#endif

