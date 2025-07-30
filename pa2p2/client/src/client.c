#include "include/client.h"

// For phase 2, you may either process the given file using multiple threads or directly. If you are going to directly parse the file and store the digit count, feel free to remove processChunks
// Phase 1 code will not influence phase 2 grades

// Function executed by each thread
void *processChunks(void *args) {
    while(1){
        char local_chunk[1024];
        int local_digits[DIGIT] = {0};
        pthread_mutex_lock(&mutex);
        
        // Wait for chunks to be available or for processing to be done
        while(q->empty && !done) {
            pthread_cond_wait(&cond_wait_qempty, &mutex);
        }
        
        // Check exit condition
        if(done && q->empty) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        // Get chunk from queue
        memcpy(local_chunk, q->chunk, 1024);
        
        // Update queue state
        q->full = false;
        q->empty = true;
        
        // Signal that queue has space
        pthread_cond_signal(&cond_wait_qfull);
        pthread_mutex_unlock(&mutex);
        
        // Process the chunk (count digits)
        for(int i = 0; i < 1024; i++) {
            if ((q->chunk[i] != ' ') && (q->chunk[i] != '\0')) {
                pthread_mutex_lock(&mutex);
                local_digits[q->chunk[i] - '0']++;
                pthread_mutex_unlock(&mutex);
            }
        }
        
        // Update global digit count
        pthread_mutex_lock(&chunkmutex);
        for (int i = 0; i < DIGIT; i++) {
            digits[i] += local_digits[i];
        }
        pthread_mutex_unlock(&chunkmutex);
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
    
    // Initialize variables
    int ret;
    done = false;
    char buffer[1024];
    size_t bytes;
    req_message_t req;
    resp_message_t resp;
    
    // Initialize mutexes and condition variables
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&clientmutex, NULL);
    pthread_mutex_init(&chunkmutex, NULL);
    pthread_cond_init(&cond_wait_qempty, NULL);
    pthread_cond_init(&cond_wait_qfull, NULL);
    
    // Create queue
    q = malloc(sizeof(struct QueueStruct));
    q->size = 0; q->empty = true; q->full = false;

    // Create threads
    pthread_t threads[nthreads];
    for(int i = 0; i < nthreads; i++) {
        pthread_create(&threads[i], NULL, processChunks, NULL);
    }

    // Partition file and push to queue only if there is space in the queue
    FILE* file = fopen(filepath, "r");
    if (!file) {
        perror("fopen error");
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    
    while ((bytes = fread(buffer, sizeof(char), 1024, file)) > 0) {
        pthread_mutex_lock(&mutex);
        while (q->full) {
            pthread_cond_wait(&cond_wait_qfull, &mutex);
        }
        memcpy(q->chunk, buffer, bytes);
        pthread_cond_signal(&cond_wait_qempty);
        pthread_mutex_unlock(&mutex);
        q->size++;
        q->full = true;
        q->empty = false;
    }
    done=true;
    fclose(file);

    // Graceful exit condition for threads?

    // Join threads
    for(int i = 0; i < nthreads; i++) {
        pthread_join(threads[i], NULL);
    }

    // print per digit count
    for(int i = 0; i < DIGIT; i++) {
        printf("Count of digit %d: %d\n", i, digits[i]);
    }

    // establish TCP connection with server
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket initialization error");
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverport);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ret = connect(sockfd, &serveraddr, sizeof(serveraddr));
    if (ret == -1) {
        perror("Socket connection error");
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }

    // register
    printf("Client %d registering with Server\n", clientid);
    req.reqcode = REGISTER;
    req.clientid = clientid;
    for (int i = 0; i < DIGIT; i++) {
        req.data[i] = 0;
    }
    ret = send(sockfd, &req, sizeof(req), 0);
    if (ret == -1) {
        perror("send error");
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);   
    }
    ret = recv(sockfd, &resp, sizeof(resp), 0);
    if (ret == -1) {
        perror("recv error");
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    if (resp.respcode == RSP_NOK) {
        printf("Client %d failed registering with Server\n", clientid);
        close(sockfd);
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    printf("Client %d registered with Server\n", clientid);

    // digit
    printf("Client %d sending digit count to Server\n", clientid);
    req.reqcode = DIGITCOUNT;
    req.clientid = clientid;
    for (int i = 0; i < DIGIT; i++) {
        req.data[i] = digits[i];
    }
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    if (resp.respcode == RSP_NOK) {
        printf("Client %d failed sending digit count to Server\n", clientid);
        close(sockfd);
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    printf("Client %d shared digit count with Server\n", clientid);

    // latestcount
    printf("Client %d sending the request for the current digit count\n", clientid);
    req.reqcode = LATESTCOUNT;
    req.clientid = clientid;
    for (int i = 0; i < DIGIT; i++) {
        req.data[i] = 0;
    }
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    if (resp.respcode == RSP_NOK) {
        printf("Client %d failed receiving current digit count from Server\n", clientid);
        close(sockfd);
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }

    // print latest per digit count
    printf("Client %d received current digit count from Server\n", clientid);
    for (int i = 0; i < DIGIT; i++) {
        printf("Count of digit %d: %d\n", i, resp.data[i]);
    }

    // De-register
    printf("Client %d sending de-registration request to Server\n", clientid);
    req.reqcode = DEREGISTER;
    req.clientid = clientid;
    for(int i = 0; i < DIGIT; i++) {
        req.data[i] = 0;
    }
    send(sockfd, &req, sizeof(req), 0);
    recv(sockfd, &resp, sizeof(resp), 0);
    if(resp.respcode == RSP_NOK) {
        printf("Client %d failed to de-register\n", clientid);
        close(sockfd);
        free(q);
        pthread_mutex_destroy(&mutex);
        pthread_mutex_destroy(&clientmutex);
        pthread_mutex_destroy(&chunkmutex);
        pthread_cond_destroy(&cond_wait_qempty);
        pthread_cond_destroy(&cond_wait_qfull);
        exit(EXIT_FAILURE);
    }
    
    // Cleanup
    close(sockfd);
    free(q);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&clientmutex);
    pthread_mutex_destroy(&chunkmutex);
    pthread_cond_destroy(&cond_wait_qempty);
    pthread_cond_destroy(&cond_wait_qfull);

    return 0;
}
