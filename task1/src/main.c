#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "buffer.h"


static shared_buffer_t g_buffer;


static volatile sig_atomic_t g_stop = 0;

void *producer_thread(void *arg) {
    long id = (long)arg;
    unsigned int seed = (unsigned int)(time(NULL) ^ (id * 1103515245u));

    while (!g_stop) {
      
        int value = rand_r(&seed) % 1000;

    
        if (sem_wait(&g_buffer.empty_slots) != 0) {
         
            break;
        }

       
        pthread_mutex_lock(&g_buffer.mutex);
        buffer_put(&g_buffer, value);
        printf("[Producer %ld] produced %d\n", id, value);
        pthread_mutex_unlock(&g_buffer.mutex);

      
        sem_post(&g_buffer.full_slots);

       
        usleep(100 * 1000); 
    }

    printf("[Producer %ld] exiting\n", id);
    return NULL;
}

void *consumer_thread(void *arg) {
    (void)arg;

    while (!g_stop) {
        
        if (sem_wait(&g_buffer.full_slots) != 0) {
            break;
        }

     
        pthread_mutex_lock(&g_buffer.mutex);
        int value = buffer_get(&g_buffer);
        printf("      [Consumer] consumed %d\n", value);
        pthread_mutex_unlock(&g_buffer.mutex);

       
        sem_post(&g_buffer.empty_slots);

        usleep(150 * 1000); 
    }

    printf("      [Consumer] exiting\n");
    return NULL;
}

int main(void) {
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    if (buffer_init(&g_buffer) != 0) {
        fprintf(stderr, "Failed to init buffer\n");
        return EXIT_FAILURE;
    }

    printf("Task 1: producer/consumer with semaphores started.\n");
    printf("Buffer size = %d, producers = %d, consumers = %d\n",
           BUFFER_SIZE, NUM_PRODUCERS, NUM_CONSUMERS);

    
    for (long i = 0; i < NUM_PRODUCERS; ++i) {
        if (pthread_create(&producers[i], NULL, producer_thread, (void *)i) != 0) {
            perror("pthread_create(producer)");
            g_stop = 1;
       
            for (long j = 0; j < i; ++j) {
                pthread_cancel(producers[j]);
                pthread_join(producers[j], NULL);
            }
            buffer_destroy(&g_buffer);
            return EXIT_FAILURE;
        }
    }

    for (long i = 0; i < NUM_CONSUMERS; ++i) {
        if (pthread_create(&consumers[i], NULL, consumer_thread, NULL) != 0) {
            perror("pthread_create(consumer)");
            g_stop = 1;
            for (long j = 0; j < NUM_PRODUCERS; ++j) {
                pthread_cancel(producers[j]);
                pthread_join(producers[j], NULL);
            }
            buffer_destroy(&g_buffer);
            return EXIT_FAILURE;
        }
    }

   
    sleep(RUN_SECONDS);
    g_stop = 1;

  
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        pthread_cancel(producers[i]);
    }
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        pthread_cancel(consumers[i]);
    }

    
    for (int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; ++i) {
        sem_post(&g_buffer.empty_slots);
        sem_post(&g_buffer.full_slots);
    }


    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        pthread_join(consumers[i], NULL);
    }

    buffer_destroy(&g_buffer);

    printf("Task 1 finished.\n");
    return EXIT_SUCCESS;
}
