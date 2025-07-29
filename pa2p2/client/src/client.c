#include "include/client.h"

// For phase 2, you may either process the given file using multiple threads or directly. If you are going to directly parse the file and store the digit count, feel free to remove processChunks
// Phase 1 code will not influence phase 2 grades

// Function executed by each thread
void *processChunks(void *args) {
    
    while(1){
        // wait for main to add chunks (condvar)

        // done condition (a variable and condvar)
        
        // pick chunk from queue and signal main about available free spot

        // store the per digit count 

        // Critical sections which are independent should be handled independently
    }
    return NULL;
}

// main thread
int main(int argc, char *argv[]) {
    // Argument checking
    if (argc != 6) {
        printf("ERROR: Usage: ./client <filepath> <qsize> <nthreads> <clientid> <serverport>\n");
        exit(EXIT_FAILURE);
    }

    char *filepath = argv[1];
    int qsize = atoi(argv[2]);
    int nthreads = atoi(argv[3]);
    int clientid = atoi(argv[4]);
    int serverport = atoi(argv[5]);
    // Create queue

    // Create threads
    
    // Partition file and push to queue only if there is space in the queue
    
    // Graceful exit condition for threads?
    
    // Join threads

    // print per digit count
    
    // establish TCP connection with server
    
    // register

    // digit
    
    // latestcount
    // print latest per digit count

    // De-register

    return 0;
}
