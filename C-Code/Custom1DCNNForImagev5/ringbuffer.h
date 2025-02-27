/*
 * ringbuffer.h
 *
 *  Created on: 05.02.2025
 *      Author: student1
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define BUFFER_SIZE 3072


typedef struct {
    int16_t buffer[BUFFER_SIZE];
    int16_t *read;
    int16_t *write;
    bool full;
} RingBuffer;

void ringbuffer_init(RingBuffer *rb);
bool ringbuffer_is_empty(RingBuffer *rb);
bool ringbuffer_is_full(RingBuffer *rb);
bool ringbuffer_write(RingBuffer *rb, int16_t *data, size_t size);
bool ringbuffer_read_overlap(RingBuffer *rb, int16_t *data, size_t size);

#endif /* RINGBUFFER_H_ */
