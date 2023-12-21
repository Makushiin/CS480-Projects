//Name : Aya Abdul Hasan
//RedID : 824080459

//Name : Maxine Mayor
//RedID : 825551461

#include <iostream>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "readvocab.h"

using namespace std;

// Function that reads vocabulary data from a file and populates shared data
void* readvocab(void* arg) {
    // Cast the argument back to the SHARED_DATA structure
    SHARED_DATA* sharedData = static_cast<SHARED_DATA*>(arg);

    // Open the vocabulary file
    ifstream vocabFile(sharedData->fileName[VOCABFILEINDEX]);

    string line;
    // Read lines from the vocabulary file
    while (getline(vocabFile, line)) {

        // lock access to shared data
        pthread_mutex_lock(&(sharedData->queue_mutex));

        // Add line to vocabulary vector
        sharedData->vocabVect->push_back(line);
        // Update count of chars read from the vocabulary file
        sharedData->numOfCharsReadFromVocabFile += line.length() + 1;
        // Increment the line count for the vocabulary file
        sharedData->lineCountInFile[VOCABFILEINDEX]++;

        // unlock after updating shared data
        pthread_mutex_unlock(&(sharedData->queue_mutex));
        
        // short delay
        usleep(10); 
    }

    // Close the vocabulary file
    vocabFile.close();

    // Mark the vocabulary file reading task as completed
    sharedData->taskCompleted[VOCABFILEINDEX] = true;

    // Exit the thread
    pthread_exit(NULL);
}
