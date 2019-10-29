/**
 * Copyright (c) 2017, Afuiot Tech, Co., Ltd
 * All rights reserverd.
 * File name: lib_fifo.c
 * Description: linux kernel fifo source file.
 * Other: none
 * Version: V1.0
 * History: Version, Writer, modifier, modify time, modify content
             V1.0     wucb     wucb    2018.11.15   first version
             V1.01    wucb     wucb    2019.01.29   add signal lock
 */
#include "lib_fifo.h"

#include <string.h>

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)>(b))?(b):(a))

/**
 * Description: Initialize the fifo struct.
 * Parameter: fifo - fifo struct that you want to be initialized.
              buffer - fifo saving space.
              size - fifo size.
 * Return: None
 * Others: Buffer must be power of 2.
 */
void KfifoInit(Kfifo *fifo, unsigned char *buffer, unsigned int size)
{
	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = 0;
	fifo->out = 0;
	PTHREAD_LOCK_INIT(fifo->lock);
}

unsigned int KfifoSize(Kfifo *fifo)
{
	return (fifo->in - fifo->out);
}

/**
 * Description: Save data in fifo.
 * Parameter: fifo - fifo struct that you want to operate.
              buffer - the data that you want to saving.
              len - the size that you want to save.
 * Return: None
 * Others: None
 */
unsigned int KfifoPut(Kfifo *fifo, unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	PTHREAD_LOCK(fifo->lock);

	len = min(len, ( fifo->size + fifo->out - fifo->in ) );

	/*
	* Ensure that we sample the fifo->out index -before- we
	* start putting bytes into the kfifo.
	*/

	/* first put the data starting from fifo->in to buffer end */
	l = min( len, ( fifo->size - ( fifo->in & ( fifo->size - 1 ) ) ) );
	memcpy( fifo->buffer + ( fifo->in & ( fifo->size - 1 ) ), buffer, l );

	/* then put the rest (if any) at the beginning of the buffer */
	memcpy( fifo->buffer, buffer + l, len - l );

	/*
	* Ensure that we add the bytes to the kfifo -before-
	* we update the fifo->in index.
	*/

	fifo->in += len;

	PTHREAD_UNLOCK(fifo->lock);

	return len;

}

/**
 * Description: Read data in fifo.
 * Parameter: fifo - fifo struct that you want to operate.
              buffer - fifo saving space.
              len - the size that you want to read.
 * Return: None
 * Others: None
 */
unsigned int KfifoGet(Kfifo *fifo, unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	PTHREAD_LOCK(fifo->lock);

	len = min( len, ( fifo->in - fifo->out ) );

	/*
	 * Ensure that we sample the fifo->in index -before- we
	 * start removing bytes from the kfifo.
	 */

	/* first get the data from fifo->out until the end of the buffer */
	l = min( len, ( fifo->size - ( fifo->out & ( fifo->size - 1 ) ) ) );
	memcpy( buffer, fifo->buffer + ( fifo->out & ( fifo->size - 1 ) ), l );

	/* then get the rest (if any) from the beginning of the buffer */
	memcpy( buffer + l, fifo->buffer, len - l );

	/*
	 * Ensure that we remove the bytes from the kfifo -before-
	 * we update the fifo->out index.
	 */

	fifo->out += len;

	PTHREAD_UNLOCK(fifo->lock);

	return len;
}
