#ifndef CLIENT_H_
#define CLIENT_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define DIGIT 10

// Queue structure, feel free to modify accoding to your requirements
struct QueueStruct {
    /*
    Circular buffer implementation
    head: index of frontal element; consumers will grab from the head.
    tail: index of rear element; producers will add to the tail
    size: current number of elements in queue
    capacity: total possible size of queue
    empty: true if empty, else false
    full: true if full, else false
    chunks: array of data
     */
    int size;
    bool empty;
    bool full;
    char chunk[1024];
};

struct QueueStruct* q;

char* getChunk(struct QueueStruct*);

// Shared data structure to keep track of the digit count
int digits[DIGIT] = {0};

// Mutex variables
pthread_mutex_t mutex;
pthread_mutex_t clientmutex;
pthread_mutex_t chunkmutex;

// Condition variables
pthread_cond_t cond_wait_qempty;
pthread_cond_t cond_wait_qfull;

// Graceful exit
bool done;

// REQUEST CODE
#define REGISTER 1
#define DIGITCOUNT 2
#define LATESTCOUNT 3
#define DEREGISTER 4

// RESPONSE CODE
#define RSP_OK 5
#define RSP_NOK 6

// Request Message structure
typedef struct requestMessage {
    int reqcode;
    int clientid;
    int data[DIGIT];
} req_message_t;

// Response Message structure
typedef struct responseMessage {
    int reqcode;
    int respcode;
    int data[DIGIT];
} resp_message_t;

#endif
