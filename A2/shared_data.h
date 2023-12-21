//Name : Aya Abdul Hasan
//RedID : 824080459

//Name : Maxine Mayor
//RedID : 825551461

#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <vector>
#include <queue>

#define NUMOFFILES 2
#define VOCABFILEINDEX 0
#define TESTFILEINDEX 1
/* default number of progress marks for representing 100% progress */
#define DEFAULT_NUMOF_MARKS 50
/* minimum number of progress marks for representing 100% progress */
#define MIN_NUMOF_MARKS 10
/* place hash marks in the progress bar every N marks */
#define DEFAULT_HASHMARKINTERVAL 10
/* default minimum number of vocab strings contained in a request string for printing or writing
to the output */
#define DEFAULT_MINNUM_OFVOCABSTRINGS_CONTAINED 0

using namespace std;
/* Common data shared between threads */
typedef struct {
/**
* parameters for printing progress bar
*/
unsigned int numOfProgressMarks = DEFAULT_NUMOF_MARKS;
unsigned int hashmarkInterval = DEFAULT_HASHMARKINTERVAL;
/**
* print the string count to the output file only if the
* number of vocab strings contained in the line
* is equal to or greater than this number
*/
unsigned int minNumOfVocabStringsContainedForPrinting = DEFAULT_MINNUM_OFVOCABSTRINGS_CONTAINED;
/**
* fileName[0] - file path for the vocabulary file
* fileName[1] - file path for the test file providing lines for testing
*/
const char *fileName [NUMOFFILES];
/** store total number of characters in files
*/
unsigned long totalNumOfCharsInVocabFile = 0;
/**
* Use numOfCharsReadFromVocabFile to track ongoing progress of
* number of characters read in from vocab file.
*/
long numOfCharsReadFromVocabFile = 0;
/**
* - number of total lines in the test file.
*/
long lineCountInFile[NUMOFFILES];
/**
* number of lines that have been removed and processed from
* the line queue,
*/
long numOfProcessedLines = 0;
/**
* store strings read from vocabulary file
*/
vector <string> *vocabVect;
/**
* buffer lines read from test file
*/
queue <string> *lineQueue;
/**
* pthread mutex for accessing the line Queue
*/
pthread_mutex_t queue_mutex;
/**
* completed flags indicate the completion of the thread logic, one
* for the readvocab thread, one for readlines thread
*/
bool taskCompleted[NUMOFFILES];
} SHARED_DATA;

#endif