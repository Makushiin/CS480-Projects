// Maxine Mayor
// 825551461

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>
#include <queue>
#include "tradecrypto.h"

// Define the maximum number of production requests.
const unsigned int PRODUCTION_LIMIT = 120;

// Set the maximum buffer size for storing requests.
#define BUFFER_SIZE 15
// Define the maximum number of Bitcoin requests that can be in the queue.
#define BITCOIN_MAX 6

using namespace std;

/* Common data structure shared between producer and consumer threads. */
typedef struct {
    pthread_mutex_t mutexBuffer;           // Mutex to protect access to the shared buffer.
    pthread_cond_t condNotEmpty;           // Condition variable to signal when the buffer is not empty.
    pthread_cond_t condNotFull;            // Condition variable to signal when the buffer is not full.
    pthread_cond_t condNotMaxBitcoin;      // Condition variable for managing Bitcoin request limit.

    queue<RequestType> buffer;             // Shared buffer for storing requests.

    int totalBitcoinRequests;              // Counter for the total number of Bitcoin requests in the queue.
    int totalEthereumRequests;             // Counter for the total number of Ethereum requests in the queue.
    int productionLimit;                   // Limit on the total number of requests to be produced.

    unsigned int inRequestQueue[RequestTypeN]; // Array to track the number of each type of request currently in the queue.
    unsigned int produced[RequestTypeN];       // Array to track the total number of each type of request produced.
    unsigned int consumed[ConsumerTypeN][RequestTypeN]; // Matrix to track the number of requests consumed by each consumer.
    bool productionComplete;                         // Flag to indicate if the production has been completed.
    unsigned int totalProduced;                      // Counter for the total number of requests produced.
} SHARED_DATA;

/* Barrier structure to synchronize the completion of threads. */
typedef struct {
    pthread_mutex_t mutex;  // Mutex for the barrier.
    pthread_cond_t cond;    // Condition variable for the barrier.
    int count;              // The total number of threads that must reach the barrier.
    int crossing;           // Counter for the number of threads that have reached the barrier.
} Barrier;

// Declaration of the barrier variable to be used for thread synchronization.
extern Barrier barrier;

#endif // SHARED_DATA_H
