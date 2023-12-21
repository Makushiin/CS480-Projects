// Maxine Mayor
// 825551461

#define BADFLAG 1
#define NORMALEXIT 0

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <pthread.h>

#include "shared.h"     // Shared data structures and constants
#include "producer.h"   // Producer thread functionality
#include "consumer.h"   // Consumer thread functionality
#include "report.h"     // Logging and reporting functions

using namespace std;

// Forward declarations for producer and consumer thread functions
void *produce(void *args);
void *consume(void *args);

// Function to initialize shared data structures and synchronization primitives
void initializeSharedData(SHARED_DATA &sharedData) {
    // Initialize mutexes and condition variables for shared data synchronization
    pthread_mutex_init(&sharedData.mutexBuffer, NULL);
    pthread_cond_init(&sharedData.condNotEmpty, NULL);
    pthread_cond_init(&sharedData.condNotFull, NULL);
    pthread_cond_init(&sharedData.condNotMaxBitcoin, NULL);

    // Initialize counters for total produced requests and type-specific requests
    sharedData.totalProduced = 0;
    sharedData.totalBitcoinRequests = 0;
    sharedData.totalEthereumRequests = 0;

    // Initialize the array tracking the number of consumed requests
    for (int i = 0; i < ConsumerTypeN; ++i) {
        for (int j = 0; j < RequestTypeN; ++j) {
            sharedData.consumed[i][j] = 0;
        }
    }
}

// Global barrier used for synchronizing thread completion
Barrier barrier;

// Function to initialize the barrier
void init_barrier(int count) {
    // Initialize mutex and condition variable for the barrier
    pthread_mutex_init(&barrier.mutex, NULL);
    pthread_cond_init(&barrier.cond, NULL);
    barrier.count = count;
    barrier.crossing = 0;
}

// Function to clean up barrier resources
void destroy_barrier() {
    // Destroy mutex and condition variable
    pthread_mutex_destroy(&barrier.mutex);
    pthread_cond_destroy(&barrier.cond);
}

// Function to clean up shared data resources
void destroySharedData(SHARED_DATA &sharedData) {
    // Destroy mutexes and condition variables used for shared data
    pthread_mutex_destroy(&sharedData.mutexBuffer);
    pthread_cond_destroy(&sharedData.condNotEmpty);
    pthread_cond_destroy(&sharedData.condNotFull);
    pthread_cond_destroy(&sharedData.condNotMaxBitcoin);
}

int main(int argc, char **argv) {
    // Initialize shared data structure
    SHARED_DATA sharedData;
    initializeSharedData(sharedData);

    // Parse command-line arguments to configure the simulation
    int n = 120; // Default total number of trade requests
    int x = 0, y = 0, b = 0, e = 0; // Initialize other arguments for delay timings

    int opt;
    while ((opt = getopt(argc, argv, "n:x:y:b:e:")) != -1) {
        switch (opt) {
            case 'n': n = atoi(optarg); break;
            case 'x': x = atoi(optarg); break;
            case 'y': y = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 'e': e = atoi(optarg); break;
            default:
                cerr << "Usage: " << argv[0] << " [-n N] [-x N] [-y N] [-b N] [-e N]" << endl;
                exit(BADFLAG);
        }
    }

    // Initialize the barrier for thread synchronization
    init_barrier(2); // Assuming 2 consumer threads

    // Zero-initialize the produced array
    for (int i = 0; i < RequestTypeN; ++i) {
        sharedData.produced[i] = 0;
    }

    // Create and start producer and consumer threads
    pthread_t producerBitcoinThread, producerEthereumThread;
    pthread_t consumerXThread, consumerYThread;

    // Initialize producer and consumer thread structures
    Producer pBitcoin = {Bitcoin, milliseconds(b), &sharedData, static_cast<unsigned int>(n)};
    Producer pEthereum = {Ethereum, milliseconds(e), &sharedData, static_cast<unsigned int>(n)};
    Consumer cBlockchainX = {BlockchainX, milliseconds(x), &sharedData};
    Consumer cBlockchainY = {BlockchainY, milliseconds(y), &sharedData};

    // Start producer and consumer threads
    pthread_create(&producerBitcoinThread, NULL, produce, &pBitcoin);
    pthread_create(&producerEthereumThread, NULL, produce, &pEthereum);
    pthread_create(&consumerXThread, NULL, consume, &cBlockchainX);
    pthread_create(&consumerYThread, NULL, consume, &cBlockchainY);

    // Wait for producer and consumer threads to finish
    pthread_join(producerBitcoinThread, NULL);
    pthread_join(producerEthereumThread, NULL);
    pthread_join(consumerXThread, NULL);
    pthread_join(consumerYThread, NULL);

    // Optionally cancel consumer threads (they will also exit when the main thread exits)
    pthread_cancel(consumerXThread);
    pthread_cancel(consumerYThread);

    // Log production and consumption history
    report_production_history(sharedData.produced, sharedData.consumed);

    // Clean up resources and destroy synchronization primitives
    destroySharedData(sharedData);
    destroy_barrier();

    return NORMALEXIT; // Exit with normal status code
}
