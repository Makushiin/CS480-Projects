//Name : Aya Abdul Hasan
//RedID : 824080459

//Name : Maxine Mayor
//RedID : 825551461


#include <iostream>
#include <fstream>
#include <sstream>
#include "countvocabstrings.h"
#include "tree.h"

using namespace std;

// Function to count vocabulary strings contained in each line of the test file
void* countvocabstrings(void* arg) {
    // Cast the argument back to the SHARED_DATA structure
    SHARED_DATA* sharedData = static_cast<SHARED_DATA*>(arg);

    // Create an output file for counts
    ofstream outputFile("countNumOfContainedVocab.txt");
    if (!outputFile) {
        cerr << "Unable to open outputFile" << endl;
        pthread_exit(NULL);
    }

    // Wait for the readvocab thread to complete
    while (!sharedData->taskCompleted[VOCABFILEINDEX]) {
    }

    // Continue processing lines from the test file
    while (true) {
        // lock access to shared data
        pthread_mutex_lock(&(sharedData->queue_mutex));

        // Check if the line queue is empty, and exit the loop if it is
        if (sharedData->lineQueue->empty()) {
            pthread_mutex_unlock(&(sharedData->queue_mutex));
            break;
        }

        // Get the next line from the line queue and update processed lines count
        string line = sharedData->lineQueue->front();
        sharedData->lineQueue->pop();
        sharedData->numOfProcessedLines++;

        // lock after reading a line
        pthread_mutex_unlock(&(sharedData->queue_mutex));

        // Create a root node for the tree structure
        Tree *root = getNode();
        root->child['\0'] = NULL;

        // lock access to insert the words from the line into the tree
        pthread_mutex_lock(&(sharedData->queue_mutex));
        insert(root, line.c_str());
        pthread_mutex_unlock(&(sharedData->queue_mutex));

        // Count the number of vocabulary strings contained in the line
        int count = 0;
        int vocabularyLen = sharedData->vocabVect->size();
        for (int i = 0; i < vocabularyLen; ++i) {
            if (search(root, sharedData->vocabVect->at(i).c_str()) == true) {
                ++count;
            } 
        }

        // Delete the tree to free memory
        deleteTree(root);

        // If the count meets the minimum threshold, write it to the output file
        if (count >= sharedData->minNumOfVocabStringsContainedForPrinting) {
            // lock access to output to the output file
            pthread_mutex_lock(&(sharedData->queue_mutex));
            outputFile << count << endl;
            // unlock after writing to the output file
            pthread_mutex_unlock(&(sharedData->queue_mutex));
        }
    }

    // Close the output file
    outputFile.close();

    // Exit the thread
    pthread_exit(NULL);
}
