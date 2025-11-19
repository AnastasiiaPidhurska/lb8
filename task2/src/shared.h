#ifndef SHARED_H
#define SHARED_H

#include <pthread.h>


typedef struct {
    int value;             
    int has_value;          

    pthread_mutex_t mutex;  
    pthread_cond_t  cond;    
} shared_single_buffer_t;

int  shared_init(shared_single_buffer_t *buf);
void shared_destroy(shared_single_buffer_t *buf);

#endif 
