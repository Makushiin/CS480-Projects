//Name : Aya Abdul Hasan
//RedID : 824080459

//Name : Maxine Mayor
//RedID : 825551461

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include <string>
#include <fstream>
#include <iostream>
#include <cstring> 
#include <getopt.h>
#include <cstdlib>

#include "shared_data.h"
#include "tree.h"
#include "readvocab.h"
#include "readlines.h"
#include "countvocabstrings.h"

// Defining a constant for exit status
#define NORMAL_EXIT 0

using namespace std; 

// Main function where the program execution starts
int main(int argc, char **argv)
{
    // Create an instance of the SHARED_DATA struct
    SHARED_DATA sharedData;

    // Skip the first two command line arguments because those are the mandatory arguments and getopt starts with the optional arguments
    int option;
    optind = optind + 2;

    // Parse command-line arguments
    while ((option = getopt(argc, argv, "p:m:v:")) != -1) {
        switch (option) {
            case 'p':
                // Set the number of progress marks from the command line
                sharedData.numOfProgressMarks = atoi(optarg);
                if (sharedData.numOfProgressMarks < 10) {
                    cerr <<"Number of progress marks must be a number and at least 10" << endl;
                    exit(NORMAL_EXIT);
                }
                break;
            case 'm':
                // Set the hash mark interval from the command line
                sharedData.hashmarkInterval = atoi(optarg);
                if (sharedData.hashmarkInterval <= 0 || sharedData.hashmarkInterval > 10) {
                    cerr <<"Hash mark interval must be a number thats greater than 0 and less than or equal to 10" << endl;
                    exit(NORMAL_EXIT);
                }
                break;
            case 'v':
                // Set the minimum number of vocab strings for printing
                sharedData.minNumOfVocabStringsContainedForPrinting = atoi(optarg);
                break;
            default:
                exit(NORMAL_EXIT);
        }
    }

    // Reset optind for mandatory argument parsing
    optind = optind - 2;

    // Check if the correct number of arguments is provided
    if (argc < 3) {
        cerr <<"Wrong number of arguments given" << endl;
        exit(NORMAL_EXIT);
    }

    // Set the file names from command-line arguments
    sharedData.fileName[VOCABFILEINDEX] = argv[1];
    sharedData.fileName[TESTFILEINDEX] = argv[2];

    // Initialize line counts for both files to 0
    sharedData.lineCountInFile[VOCABFILEINDEX] = 0;
    sharedData.lineCountInFile[TESTFILEINDEX] = 0;

    // Open the vocabulary file
    ifstream vocabFile(sharedData.fileName[VOCABFILEINDEX]);
    if (!vocabFile.is_open()) {
        cerr << "Unable to open " << sharedData.fileName[VOCABFILEINDEX] << endl;
        exit(NORMAL_EXIT);    }


    // Calculate the total number of characters in the vocabulary file for use later
    string line;
    while (getline(vocabFile, line)) {
        sharedData.totalNumOfCharsInVocabFile += line.length() + 1;  }

    // Open the test file
    ifstream testFile(sharedData.fileName[TESTFILEINDEX]);
    if (!testFile.is_open()) {
        cerr << "Unable to open " << sharedData.fileName[TESTFILEINDEX] << endl;
        exit(NORMAL_EXIT); }


    // Initialize data structures for shared data
    sharedData.vocabVect = new vector<string>();
    sharedData.lineQueue = new queue<string>();

    // Initialize thread IDs and a mutex
    pthread_t readvocabThread;
    pthread_t readlinesThread;
    pthread_t countvocabstringsThread;


    pthread_mutex_init(&sharedData.queue_mutex, NULL);

    // Initialize task completion flags to false
    sharedData.taskCompleted[VOCABFILEINDEX] = false;
    sharedData.taskCompleted[TESTFILEINDEX] = false;

    // Create threads for reading vocabulary, reading lines, and counting vocab strings
    if (pthread_create(&readvocabThread, NULL,
                        &readvocab, &sharedData) ||
        pthread_create(&readlinesThread, NULL,
                        &readlines, &sharedData) ||
        pthread_create(&countvocabstringsThread, NULL,
                        &countvocabstrings, &sharedData)) {
    cerr << "Unable to create the thread" << endl;
    exit(NORMAL_EXIT);
    }

    // initialize diffNum to keep track of there's a difference in bar
    int diffNum = 0;
    // initialize i to keep track of progress Mark we are on
    int i = 0;
    // progress bar display while reading the vocabulary file
    while (sharedData.numOfCharsReadFromVocabFile < sharedData.totalNumOfCharsInVocabFile) {
        // Calculate progress and display a progress bar
        double progress = static_cast<double>(sharedData.numOfCharsReadFromVocabFile) / static_cast<double>(sharedData.totalNumOfCharsInVocabFile);
        // add 0.5 to round up the number
        int barLength = static_cast<int>((progress * sharedData.numOfProgressMarks) + 0.5);
        //if theres an update for the bar
        if (barLength > diffNum) {

                //find if there's a hashmark needed
                if ((i + 1) % sharedData.hashmarkInterval == 0) {
                    cout << "#"; 
                    i++;
                } else {
                    cout << "-";
                    i++;
                }

            //set to current mark for next loop
            diffNum = barLength;
        } 

        fflush(stdout);
        usleep(10); 
    }

    cout << endl;
    cout << "There are " << sharedData.lineCountInFile[VOCABFILEINDEX] << " lines in vocabulary.txt." << endl;

    // Wait for the readvocab and readlines threads to finish
    if (pthread_join(readvocabThread, NULL)) {
        cerr << "Error joining the readvocab thread." << endl;
        exit(NORMAL_EXIT); 
    }

    if (pthread_join(readlinesThread, NULL)) {
        cerr << "Error joining the readvocab thread." << endl;
        exit(NORMAL_EXIT); 
    }

    // initialize diffLine to keep track of there's a difference in bar
    int diffLine = 0;
    // initialize j to keep track of progress Mark we are on
    int j = 0;
    // progress bar display while processing lines from the test file
    while (!sharedData.lineQueue->empty()) {
        // Calculate progress and display a progress bar
        double progress = static_cast<double>(sharedData.numOfProcessedLines) / static_cast<double>(sharedData.lineCountInFile[TESTFILEINDEX]);
        // add 0.5 to round up the number
        int barLength = static_cast<int>((progress * sharedData.numOfProgressMarks) + 0.5);
        //if theres an update for the bar
        if (barLength > diffLine) {

                //find if there's a hashmark needed
                if ((j + 1) % sharedData.hashmarkInterval == 0) {
                    cout << "#"; 
                    j++;
                } else {
                    cout << "-";
                    j++;
                }
            //set to current mark for next loop
            diffLine = barLength;
        } 

        fflush(stdout);
        usleep(10); 
    }

    cout << endl;
    cout << "There are " << sharedData.lineCountInFile[TESTFILEINDEX] << " lines in testfile.txt." << endl;

    // Wait for the countvocabstrings thread to finish
    if (pthread_join(countvocabstringsThread, NULL)) {
        cerr << "Error joining the readvocab thread." << endl;
        exit(NORMAL_EXIT);
    }

// Exit the program with a normal status code
exit(NORMAL_EXIT); 
}


