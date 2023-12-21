// Maxine Mayor
// 825551461

#ifndef PRODUCER_H
#define PRODUCER_H

#include <chrono>
#include <thread>
#include <iostream>
#include <fstream>
#include <map>
#include "shared.h"
#include "report.h"

using namespace std::this_thread;
using namespace std::chrono;

// Structure representing a Producer.
typedef struct {
    RequestType type; // Type of request to produce (Bitcoin or Ethereum).
    milliseconds ms;  // Time it takes to produce a request.
    SHARED_DATA *sd;  // Shared data accessible by all threads.
    unsigned int productionLimit; // Maximum number of requests to produce.
} Producer;

// Function executed by producer threads.
void *produce(void *v) {
    Producer *p = (Producer *)v;

    // Continue producing until reaching the production limit.
    while (p->sd->totalProduced < p->productionLimit) {
        // Simulate the time taken to produce a request.
        sleep_for(p->ms);

        pthread_mutex_lock(&(p->sd->mutexBuffer));

        // Break if production limit is reached while waiting for the mutex.
        if (p->sd->totalProduced >= p->productionLimit) {
            pthread_mutex_unlock(&(p->sd->mutexBuffer));
            break;
        }

        // Bitcoin-specific check for the maximum number of requests.
        if (p->type == Bitcoin && p->sd->totalBitcoinRequests >= BITCOIN_MAX) {
            pthread_cond_wait(&(p->sd->condNotMaxBitcoin), &(p->sd->mutexBuffer));
        }

        // Wait if the buffer is full.
        while (p->sd->buffer.size() >= BUFFER_SIZE) {
            pthread_cond_wait(&(p->sd->condNotFull), &(p->sd->mutexBuffer));
        }

        // Add the produced request to the buffer.
        p->sd->buffer.push(p->type);
        p->sd->produced[p->type]++;
        p->sd->totalProduced++; // Increment total count of produced requests.

        // Update the count of Bitcoin or Ethereum requests, as appropriate.
        if (p->type == Bitcoin) {
            p->sd->totalBitcoinRequests++;
        } else if (p->type == Ethereum) {
            p->sd->totalEthereumRequests++;
        }

        // Calculate the current state of the buffer.
        unsigned int bufferState[RequestTypeN] = {0};
        queue<RequestType> tempBuffer = p->sd->buffer;
        while (!tempBuffer.empty()) {
            bufferState[tempBuffer.front()]++;
            tempBuffer.pop();
        }

        // Log the addition of the request.
        report_request_added(p->type, p->sd->produced, bufferState);

        // Signal that the buffer is not empty.
        pthread_cond_signal(&(p->sd->condNotEmpty));
        pthread_mutex_unlock(&(p->sd->mutexBuffer));
    }

    return 0;
}

#endif
