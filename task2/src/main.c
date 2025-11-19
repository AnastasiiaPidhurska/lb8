#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "shared.h"

static shared_single_buffer_t g_buf;
static volatile sig_atomic_t g_stop = 0;

void *producer_thread(void *arg) {
    (void)arg;
    unsigned int seed = (unsigned int)(time(NULL) ^ 0x1234u);

    while (!g_stop) {
        sleep(1); 

        int value = rand_r(&seed) % 1000;

        pthread_mutex_lock(&g_buf.mutex);

     
        while (g_buf.has_value && !g_stop) {
            pthread_cond_wait(&g_buf.cond, &g_buf.mutex);
        }

        if (g_stop) {
            pthread_mutex_unlock(&g_buf.mutex);
            break;
        }

        g_buf.value = value;
        g_buf.has_value = 1;
        printf("[Producer] produced %d\n", value);

        pthread_cond_signal(&g_buf.cond);
        pthread_mutex_unlock(&g_buf.mutex);
    }

    printf("[Producer] exiting\n");
    return NULL;
}

void *consumer_thread(void *arg) {
    (void)arg;

    while (!g_stop) {
        pthread_mutex_lock(&g_buf.mutex);

        while (!g_buf.has_value && !g_stop) {
            pthread_cond_wait(&g_buf.cond, &g_buf.mutex);
        }

        if (g_stop) {
            pthread_mutex_unlock(&g_buf.mutex);
            break;
        }

        int value = g_buf.value;
        g_buf.has_value = 0;

        printf("   [Consumer] got %d\n", value);

     
        pthread_cond_signal(&g_buf.cond);
        pthread_mutex_unlock(&g_buf.mutex);

       
        usleep(500 * 1000); 
    }

    printf("   [Consumer] exiting\n");
    return NULL;
}

int main(void) {
    pthread_t producer, consumer;

    if (shared_init(&g_buf) != 0) {
        fprintf(stderr, "Failed to init shared buffer\n");
        return EXIT_FAILURE;
    }

    printf("Task 2: producer/consumer with condition variables started.\n");

    if (pthread_create(&producer, NULL, producer_thread, NULL) != 0) {
        perror("pthread_create(producer)");
        shared_destroy(&g_buf);
        return EXIT_FAILURE;
    }
    if (pthread_create(&consumer, NULL, consumer_thread, NULL) != 0) {
        perror("pthread_create(consumer)");
        g_stop = 1;
        pthread_cancel(producer);
        pthread_join(producer, NULL);
        shared_destroy(&g_buf);
        return EXIT_FAILURE;
    }

  
    sleep(10);
    g_stop = 1;

   
    pthread_mutex_lock(&g_buf.mutex);
    pthread_cond_broadcast(&g_buf.cond);
    pthread_mutex_unlock(&g_buf.mutex);

    pthread_join(producer, NULL);
    pthread_join(consumer, NULL);

    shared_destroy(&g_buf);

    printf("Task 2 finished.\n");
    return EXIT_SUCCESS;
}
