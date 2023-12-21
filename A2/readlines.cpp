//Name : Aya Abdul Hasan
//RedID : 824080459

//Name : Maxine Mayor
//RedID : 825551461

#include <iostream>
#include <fstream>
#include <string>
#include "readlines.h"

using namespace std;

// Function that reads lines from a test file and populates shared data
void* readlines(void* arg) {
    // Cast the argument back to the SHARED_DATA structure
    SHARED_DATA* sharedData = static_cast<SHARED_DATA*>(arg);

    // Open the test file
    ifstream testFile(sharedData->fileName[TESTFILEINDEX]);

    string line;
    // Read lines from the test file
    while (getline(testFile, line)) {

        // lock access to shared data
        pthread_mutex_lock(&(sharedData->queue_mutex));

        // Add the line to the line queue in shared data
        sharedData->lineQueue->push(line);
        // Increment the line count for the test file
        sharedData->lineCountInFile[TESTFILEINDEX]++;

        // unlock after updating shared data
        pthread_mutex_unlock(&(sharedData->queue_mutex));
    }

    // Close the test file
    testFile.close();

    // Mark the test file reading task as completed
    sharedData->taskCompleted[TESTFILEINDEX] = true;

    // Exit the thread
    pthread_exit(NULL);
}
