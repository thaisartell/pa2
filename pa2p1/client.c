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
/*
cond_available: a chunk is available for processing 
cond_incoming: queue has items
*/
pthread_cond_t cond_available = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_incoming = PTHREAD_COND_INITIALIZER;

// Queue implementation functions
queue_t* create_queue(int capacity) {
    // Implement queue creation
    queue_t* q = malloc(sizeof(queue_t));
    q->capacity = capacity;
    q->size = 0;
    q->head = 0;
    q->tail = 0;
    q->data = malloc(capacity * sizeof(chunk_t*));  // each data index holds chunk_t*
    return q;
}

void destroy_queue(queue_t *q) {
    // TODO: Free queue memory
    if (q == NULL) return;
    free(q->data);
    free(q);
}

int enqueue(queue_t *q, chunk_t *chunk) {
    // TODO: Add chunk to queue
    if (((q->tail + 1) % q->capacity == q->head) && q->size > 0) { // wrapping around non-empty array: can't enqueue
        return -1;
    }
    q->data[q->tail] = chunk; // placement of new item
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    return 0;
}

chunk_t* dequeue(queue_t *q) {
    // TODO: Remove and return chunk from queue    
    if (q->size == 0) {
        return NULL;
    }
    chunk_t* bye = q->data[q->head];
    q->data[q->head] = NULL;   // remove dequeued item
    q->head = (q->head + 1) % q->capacity;
    q->size--;    
    return bye;
}

// Worker thread function
void* worker_thread(void *arg) {
    int thread_id = *(int*)arg;
    printf("Thread %d started\n", thread_id);
    // TODO: Main worker loop
    while (1) {
        pthread_mutex_lock(&mutex);
        while (chunk_queue->size == 0 && !done) {
            pthread_cond_wait(&cond_incoming, &mutex);
        }
        if (chunk_queue->size == 0 && done) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        chunk_t* chunk = dequeue(chunk_queue);
        pthread_cond_signal(&cond_available);
        pthread_mutex_unlock(&mutex);

        int i;
        unsigned char digit;
        pthread_mutex_lock(&mutex);
        for (i = 0; i < chunk->count; i++) {
            digit = chunk->data[i];
            if (digit >= '0' && digit <= '9') {
                digit_counts[digit - '0']++;
            }
        }
        pthread_mutex_unlock(&mutex);
        free(chunk);
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

    pthread_mutex_init(&mutex, NULL);
    done = false;
    
    // TODO: Initialize synchronization primitives
    pthread_mutex_init(&mutex, NULL);
    // TODO: Create queue
    chunk_queue = create_queue(qsize);

    // TODO: Create worker threads
    pthread_t threads[nthreads];
    int thread_ids[nthreads];
    for (int i = 0; i < nthreads; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker_thread, &thread_ids[i]);
    }

    // TODO: Open and read file in chunks
    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("The file failed to open");
        return 1;
    }
    // TODO: Read file in 1024-byte chunks
    // TODO: Operates as the producer
    while (1) {
        chunk_t *chunk = malloc(sizeof(chunk_t));
        if (!chunk) break;
        chunk->count = fread(chunk->data, sizeof(unsigned char), 1024, fp);
        if (chunk->count == 0) {
            free(chunk);
            break;
        }

        pthread_mutex_lock(&mutex);
        while (chunk_queue->size == chunk_queue->capacity) {
            pthread_cond_wait(&cond_available, &mutex);
        }
        enqueue(chunk_queue, chunk);
        pthread_cond_signal(&cond_incoming);
        pthread_mutex_unlock(&mutex);
    }

    // TODO: When all bytes have been read from the file and placed into the
    // queue, Signal completion and wait for workers
    pthread_mutex_lock(&mutex);
    done = true;
    pthread_cond_broadcast(&cond_incoming);
    pthread_mutex_unlock(&mutex);

    for (int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }
    // TODO: Print results
    printf("Digit counts for client %d:\n", client_id);
    for (int i = 0; i < 10; i++) {
        printf("%d: %d\n", i, digit_counts[i]);
    }
    // TODO: Cleanup
    fclose(fp);
    // Destroy mutexes and condition variables
    // Free allocated memory
    destroy_queue(chunk_queue);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_available);
    pthread_cond_destroy(&cond_incoming);
    
    return 0;
}
