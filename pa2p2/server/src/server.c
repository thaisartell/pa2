#include "include/server.h"

typedef struct ClientNode {
    int client_id;
    struct ClientNode *next;
} ClientNode;

ClientNode *clients_head = NULL;
pthread_mutex_t digit_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

// Helpers
void add_client(int client_id) {
    pthread_mutex_lock(&client_mutex);
    ClientNode *new_node = malloc(sizeof(ClientNode));
    new_node->client_id = client_id;
    new_node->next = clients_head;
    clients_head = new_node;
    pthread_mutex_unlock(&client_mutex);
}

void remove_client(int client_id) {
    pthread_mutex_lock(&client_mutex);
    ClientNode **curr = &clients_head;
    while (*curr) {
        if ((*curr)->client_id == client_id) {
            ClientNode *tmp = *curr;
            *curr = (*curr)->next;
            free(tmp);
            break;
        }
        curr = &(*curr)->next;
    }
    pthread_mutex_unlock(&client_mutex);
}

int client_exists(int client_id) {
    pthread_mutex_lock(&client_mutex);
    ClientNode *curr = clients_head;
    while (curr) {
        if (curr->client_id == client_id) {
            pthread_mutex_unlock(&client_mutex);
            return 1;
        }
        curr = curr->next;
    }
    pthread_mutex_unlock(&client_mutex);
    return 0;
}

void *clientHandler(void *args) {
    int connfd = *(int*)args;
    free(args);

    req_message_t req;
    resp_message_t resp;

    while(1) {
        // receive request from the client
        ssize_t n = recv(connfd, &req, sizeof(req), 0);
        if (n <= 0) break;
        // Depending on the type of request take action
        int command = req.reqcode;
        int client_id = req.clientid;

        if (command == REGISTER) {
            printf("Client [%d] registering with Server\n", client_id);
            if (client_exists(client_id)) {
                resp.reqcode = REGISTER;
                resp.respcode = RSP_NOK;
                memset(resp.data, 0, sizeof(resp.data));
                send(connfd, &resp, sizeof(resp), 0);
                printf("Client [%d] failed registering with Server\n", client_id);
                break;
            }
            add_client(client_id);
            printf("Client [%d] registered with Server\n", client_id);
            resp.reqcode = REGISTER;
            resp.respcode = RSP_OK;
            memset(resp.data, 0, sizeof(resp.data));
            resp.data[0] = client_id;
            send(connfd, &resp, sizeof(resp), 0);

        } else if (command == DIGITCOUNT) {
            printf("Client [%d] shared digit count with Server\n", client_id);
            pthread_mutex_lock(&digit_mutex);
            for (int i = 0; i < DIGIT; ++i) {
                digits[i] += req.data[i];
            }
            pthread_mutex_unlock(&digit_mutex);
            resp.reqcode = DIGITCOUNT;
            resp.respcode = RSP_OK;
            memset(resp.data, 0, sizeof(resp.data));
            resp.data[0] = client_id;
            send(connfd, &resp, sizeof(resp), 0);

        } else if (command == LATESTCOUNT) {
            printf("Client [%d] requests current digit count\n", client_id);
            resp.reqcode = LATESTCOUNT;
            resp.respcode = RSP_OK;
            pthread_mutex_lock(&digit_mutex);
            for (int i = 0; i < DIGIT; ++i) {
                resp.data[i] = digits[i];
            }
            pthread_mutex_unlock(&digit_mutex);
            send(connfd, &resp, sizeof(resp), 0);

        } else if (command == DEREGISTER) {
            printf("Client [%d] de-registering\n", client_id);
            remove_client(client_id);
            resp.reqcode = DEREGISTER;
            resp.respcode = RSP_OK;
            memset(resp.data, 0, sizeof(resp.data));
            resp.data[0] = client_id;
            send(connfd, &resp, sizeof(resp), 0);
            break;
        }
    }

    // close the connection
    close(connfd);
    return NULL;
}

int main(int argc, char *argv[]) {

    int port = atoi(argv[1]);

    // create TCP socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        exit(1);
    }
    // bind server information to the TCP socket
    struct sockaddr_in servaddr = {0};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(listenfd);
        exit(1);
    }
    // start listening on the socket with given backlog
    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen");
        close(listenfd);
        exit(1);
    }
    printf("Listening on port %d\n", port);

    while(1) {
        // accept a connection from a client
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int *connfd = malloc(sizeof(int));
        *connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len);
        if (*connfd < 0) {
            perror("accept");
            free(connfd);
            continue;
        }
        // create a thread that calls clientHandler with argument as socket returned by accept to handle the client request
        pthread_t tid;
        if (pthread_create(&tid, NULL, clientHandler, connfd) != 0) {
            perror("pthread_create");
            close(*connfd);
            free(connfd);
        } else {
            pthread_detach(tid);
        }
        // Detach the thread
    }

    // close the socket
    close(listenfd);
    return 0;
}
