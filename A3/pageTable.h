
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <array>
#include <math.h>
#include <string>
#include <cstring>
#include <sstream>


/*
 *
 * MAX_BIT_SIZE : A 32-bit virtual address space is assumed.
 *
 * MAX_NUM_OF_LEVELS:  Total number of bits from all levels should be less than or equal to 28 (<=
28); if not, print to the standard error output (stderr) or standard output:
Too many bits used in page tables
then exit.
 *
 * */
#define MAX_BIT_SIZE 32
#define MAX_NUM_OF_LEVELS 28
#define VALID true
#define INVALID false

using namespace std;

struct PageTable {
    int levelCount;   // counts level currently on
    int* bitMask;     // hexadecimal array of what to mask
    int* bitShift;    // array amount to shift per bit, dependent on mask
    int* entryCount;  // amount of possible pages per level
    int* pages;

    struct LevelNode* root;
};

struct LevelNode{
    int depth;

    struct PageTable* PageTablePtr;
    struct Map* MapPtr;

    struct LevelNode** nextLevelPtr;
};

struct Map{
    unsigned int frame;
    bool status;
};


int* bitShiftArray(int*, int);
int* bitMaskArray(int* , int*, int);
int* entryCountArray(int*, int);
int* pagesArray(int*, int*, int, unsigned int);
int concatenate(int , int, int);
int grabPhysicalAddress(int, int, unsigned int);
unsigned int bit_Mask(unsigned int, unsigned int);
bool checkTotalBitSize(int);
LevelNode* createLevel(int, bool);

unsigned int getVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift);
Map* findVpn2PfnMapping(PageTable*, unsigned int);
void insertVpn2PfnMapping(PageTable*, unsigned int, int &);
void countTotalBytes(LevelNode*, unsigned int&, PageTable*);

#endif
