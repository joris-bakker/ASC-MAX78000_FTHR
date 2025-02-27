/*
 * ringbuffer.c
 *
 *  Created on: 05.02.2025
 *      Author: ChatGPT

 Prompt: Generiert mit code für einen Ringbuffer, der int16_t Werte Speichert . Beim Lesen des Ringbuffers soll der zeiger nur um die hälfte der gelesenen daten verändert werden, also bei 1024 samples nur 512 weiter, damit ein Overlap entsteht.

 */


#include "ringbuffer.h"
#include <stdio.h>

void ringbuffer_init(RingBuffer *rb) {
    rb->read = rb->buffer;
    rb->write = rb->buffer;
    rb->full = false;

}

bool ringbuffer_is_empty(RingBuffer *rb) {
    return (!rb->full && (rb->read == rb->write));
}

bool ringbuffer_is_full(RingBuffer *rb) {
    return rb->full;
}

bool ringbuffer_write(RingBuffer *rb, int16_t *data, size_t size) {
    if (size > BUFFER_SIZE) return false; // Invalid size

    for (size_t i = 0; i < size; i++) {
        if (rb->full) return false; // Buffer full

        *rb->write = data[i];
        rb->write++;

        if (rb->write >= rb->buffer + BUFFER_SIZE) {
            rb->write = rb->buffer; // Wrap-around
        }

        if (rb->write == rb->read) {
            rb->full = true;
        }
    }
    return true;
}

bool ringbuffer_read_overlap(RingBuffer *rb, int16_t *data, size_t size) {
    if (size > BUFFER_SIZE) return false; // Ungültige Größe

    


    // Kopiere alle 'size' Samples in das Zielarray, ohne den eigentlichen Pointer zu verändern.
    int16_t *temp = rb->read;
    for (size_t i = 0; i < size; i++) {
        data[i] = *temp;
        temp++;
        if (temp >= rb->buffer + BUFFER_SIZE) {
            temp = rb->buffer; // Wrap-around am Ende des Puffers
        }
    }

    // Verschiebe den Lesezeiger nur um die Hälfte der gelesenen Samples (Überlappung)
    size_t advance = size / 2;
    for (size_t i = 0; i < advance; i++) {
        rb->read++;
        if (rb->read >= rb->buffer + BUFFER_SIZE) {
            rb->read = rb->buffer; // Wrap-around
        }
    }

    // Durch das "Konsumieren" von advance Samples ist der Puffer nicht mehr voll.
    rb->full = false;
    return true;
}
