// Maxine Mayor
// 825551461

#ifndef CONSUMER_H
#define CONSUMER_H

#include <chrono>
#include <thread>
#include <fstream>
#include "shared.h"
#include "report.h"

using namespace std::this_thread;
using namespace std::chrono;

// Structure representing a Consumer.
typedef struct {
    ConsumerType type; // Type of consumer (Blockchain X or Y).
    milliseconds ms;   // Time it takes to execute (consume) a request.
    SHARED_DATA *sd;   // Pointer to shared data used by all threads.
} Consumer;

// Function to wait at a barrier until all threads reach this point.
void barrier_wait() {
    pthread_mutex_lock(&barrier.mutex);
    barrier.crossing++;
    if (barrier.crossing < barrier.count) {
        pthread_cond_wait(&barrier.cond, &barrier.mutex);
    } else {
        barrier.crossing = 0; // Reset barrier for potential reuse.
        pthread_cond_broadcast(&barrier.cond);
    }
    pthread_mutex_unlock(&barrier.mutex);
}

// Consumer thread function.
void *consume(void *v) {
    Consumer *c = (Consumer *)v;
    RequestType requestType;

    while (true) {
        pthread_mutex_lock(&(c->sd->mutexBuffer));

        // Wait until the buffer has a request to consume.
        while (c->sd->buffer.empty()) {
            pthread_cond_wait(&(c->sd->condNotEmpty), &(c->sd->mutexBuffer));
        }

        // Consume the request from the front of the buffer.
        requestType = c->sd->buffer.front();
        c->sd->buffer.pop();
        c->sd->consumed[c->type][requestType]++;

        // Calculate the state of the buffer after consuming.
        unsigned int bufferState[RequestTypeN] = {0};
        queue<RequestType> tempBuffer = c->sd->buffer;
        while (!tempBuffer.empty()) {
            bufferState[tempBuffer.front()]++;
            tempBuffer.pop();
        }

        // Pass the correct consumed array segment for this consumer.
        unsigned int consumedForThisConsumer[RequestTypeN];
        for (int i = 0; i < RequestTypeN; ++i) {
            consumedForThisConsumer[i] = c->sd->consumed[c->type][i];
        }

        // Log the request removal.
        report_request_removed(c->type, requestType, consumedForThisConsumer, bufferState);

        // Signal conditions based on buffer state.
        if (c->sd->buffer.size() < BUFFER_SIZE) {
            pthread_cond_signal(&(c->sd->condNotFull));
        }
        if (requestType == Bitcoin && c->sd->inRequestQueue[Bitcoin] < BITCOIN_MAX) {
            pthread_cond_signal(&(c->sd->condNotMaxBitcoin));
        }

        pthread_mutex_unlock(&(c->sd->mutexBuffer));

        // Simulate consumption processing time.
        sleep_for(c->ms);

        // Calculate total consumed requests for this consumer.
        unsigned int totalConsumed = 0;
        for (int i = 0; i < RequestTypeN; ++i) {
            totalConsumed += c->sd->consumed[c->type][i];
        }

        // Check if production is complete and buffer is empty.
        if (c->sd->totalProduced >= c->sd->productionLimit && c->sd->buffer.empty()) {
            // Signal the barrier and exit the loop to terminate the thread.
            barrier_wait();
            break;
        }
    }
    return 0;
}

#endif
