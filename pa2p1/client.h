#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include <stdbool.h>

#define CHUNK_SIZE 1024

pthread_mutex_t mutex;
bool done;

// Structure for a chunk of data
// Add any code you think necessary to implement your design
typedef struct chunk {
    /*
    count: how many actual bytes are in data
    data: holds acquired data chunks
    */
    int count;
    char data[CHUNK_SIZE];
} chunk_t;

// Structure for the queue
// Add any code you think necessary to implement your design
typedef struct queue {
    /*
    capacity: maximum possible queue size
    size: current amount of elemenets in queue
    head: index of the frontal element (oldest element, gets popped)
    tail: index of end element (most recent element)
    */
    int capacity;
    int size;
    int head;
    int tail;
    chunk_t** data;  // Array of pointers to chunks
} queue_t;

// Function prototypes
queue_t* create_queue(int capacity);
void destroy_queue(queue_t *q);
int enqueue(queue_t *q, chunk_t *chunk);
chunk_t* dequeue(queue_t *q);
void* worker_thread(void *arg);
#endif // CLIENT_H
