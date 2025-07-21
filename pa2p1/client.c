#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "client.h"

// Global variables
queue_t *chunk_queue;
int digit_counts[10] = {0};

// Queue implementation functions
queue_t* create_queue(int capacity) {
    // Implement queue creation
    queue_t* q = malloc(sizeof(queue_t));
    q->capacity = capacity;
    q->size = 0;
    q->head = 0;
    q->tail = 0;
    pthread_mutex_lock(&mutex);
    q->data = malloc(capacity * sizeof(chunk_t*));  // each data index holds chunk_t*
    pthread_mutex_unlock(&mutex);
    return q;
}

void destroy_queue(queue_t *q) {
    // TODO: Free queue memory
    if (q == NULL) {
        return;
    }
    free(q->data);
    free(q);
}

int enqueue(queue_t *q, chunk_t *chunk) {
    // TODO: Add chunk to queue
    pthread_mutex_lock(&mutex);

    if (((q->tail + 1) % q->capacity == q->head) && q->size > 0) { // wrapping around non-empty array: can't enqueue
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    
    q->data[q->tail] = chunk; // placement of new item
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;

    pthread_mutex_unlock(&mutex);
    
    return 0;
}

chunk_t* dequeue(queue_t *q) {
    // TODO: Remove and return chunk from queue
    pthread_mutex_lock(&mutex);
    
    if (q->size == 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    chunk_t* bye = q->data[q->head];
    q->data[q->head] = NULL;   // remove dequeued item
    q->head = (q->head + 1) % q->capacity;
    q->size--;

    pthread_mutex_unlock(&mutex);
    
    return bye;
}

// Worker thread function
void* worker_thread(void *arg) {
    int thread_id = *(int*)arg;
    
    // TODO: Main worker loop
    while (1) {

    }
    
    return NULL;
}

// Main function
int main(int argc, char *argv[]) {
    // Parse command line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: %s [filepath] [qsize] [nthreads] [client_id]\n", argv[0]);
        return 1;
    }
    
    char *filepath = argv[1];
    int qsize = atoi(argv[2]);
    int nthreads = atoi(argv[3]);
    int client_id = atoi(argv[4]);
    
    // TODO: Initialize synchronization primitives
        
    // TODO: Create queue
    chunk_queue = create_queue(qsize);

    // TODO: Create worker threads
        
    // TODO: Open and read file in chunks
    // TODO: Read file in 1024-byte chunks
    // TODO: Operates as the producer
    
    // TODO: When all bytes have been read from the file and placed into the
    // queue, Signal completion and wait for workers
        
    // TODO: Print results
        
    // TODO: Cleanup
    // Destroy queue
    // Destroy mutexes and condition variables
    // Free allocated memory
    
    return 0;
}
