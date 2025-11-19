#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>


#define BUFFER_SIZE 10


#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 1


#define RUN_SECONDS 10


typedef struct {
    int data[BUFFER_SIZE];      
    int head;                   
    int tail;                   

    sem_t empty_slots;          
    sem_t full_slots;            
    pthread_mutex_t mutex;       
} shared_buffer_t;


int buffer_init(shared_buffer_t *buf);
void buffer_destroy(shared_buffer_t *buf);


void buffer_put(shared_buffer_t *buf, int value);
int  buffer_get(shared_buffer_t *buf);

#endif 
