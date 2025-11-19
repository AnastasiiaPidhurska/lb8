#include "buffer.h"
#include <stdio.h>

int buffer_init(shared_buffer_t *buf) {
    buf->head = 0;
    buf->tail = 0;

    if (sem_init(&buf->empty_slots, 0, BUFFER_SIZE) != 0) {
        perror("sem_init(empty_slots)");
        return -1;
    }
    if (sem_init(&buf->full_slots, 0, 0) != 0) {
        perror("sem_init(full_slots)");
        sem_destroy(&buf->empty_slots);
        return -1;
    }

    if (pthread_mutex_init(&buf->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        sem_destroy(&buf->empty_slots);
        sem_destroy(&buf->full_slots);
        return -1;
    }

    return 0;
}

void buffer_destroy(shared_buffer_t *buf) {
    sem_destroy(&buf->empty_slots);
    sem_destroy(&buf->full_slots);
    pthread_mutex_destroy(&buf->mutex);
}

void buffer_put(shared_buffer_t *buf, int value) {
    buf->data[buf->tail] = value;
    buf->tail = (buf->tail + 1) % BUFFER_SIZE;
}

int buffer_get(shared_buffer_t *buf) {
    int value = buf->data[buf->head];
    buf->head = (buf->head + 1) % BUFFER_SIZE;
    return value;
}
