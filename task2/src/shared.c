#include "shared.h"
#include <stdio.h>

int shared_init(shared_single_buffer_t *buf) {
    buf->value = 0;
    buf->has_value = 0;

    if (pthread_mutex_init(&buf->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        return -1;
    }
    if (pthread_cond_init(&buf->cond, NULL) != 0) {
        perror("pthread_cond_init");
        pthread_mutex_destroy(&buf->mutex);
        return -1;
    }

    return 0;
}

void shared_destroy(shared_single_buffer_t *buf) {
    pthread_cond_destroy(&buf->cond);
    pthread_mutex_destroy(&buf->mutex);
}
