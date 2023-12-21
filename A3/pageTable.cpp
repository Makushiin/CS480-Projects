
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#include "pageTable.h"
#include <iostream>
#include <unordered_map>

int* bitShiftArray(int* input, int size) {
    // Print a message at the start of the function
//    std::cout << "Entering bitShiftArray function, size: " << size << std::endl;

    // Allocate memory for the array
    int* array = (int*)malloc(size * sizeof(int));
    if (array == NULL) {
        std::cerr << "Memory allocation for array failed!" << std::endl;
        exit(1);  // Exit or handle error as appropriate for your application
    }

    int shift = 0;

    // Process each item in the input array
    for (int idx = 0; idx < size; idx++) {
        // You can also add a print statement here to check each input value
//        std::cout << "Processing index " << idx << " with value: " << input[idx] << std::endl;

        shift += input[idx];
        array[idx] = MAX_BIT_SIZE - shift;  // Assuming MAX_BIT_SIZE is a constant defined elsewhere

        // Print the calculated shift value for the current index
//        std::cout << "Calculated shift for index " << idx << " is: " << array[idx] << std::endl;
    }

    // Print a message at the end of the function
//    std::cout << "Exiting bitShiftArray function." << std::endl;

    return array;  // Return the resulting array
}

int* bitMaskArray(int* input, int* shift, int size) {

    int* array = (int*)malloc(size * sizeof(int));

    for (int idx = 0; idx < size; idx++) {
        array[idx] = bit_Mask(input[idx], shift[idx]);
    }

    return array;
}

int* entryCountArray(int* input, int size) {

    int* array = (int*)malloc(size * sizeof(int));

    for (int idx = 0; idx < size; idx++) {
        array[idx] = pow(2.0, input[idx]);
    }

    return array;

}

int* pagesArray(int* masks, int* shifts, int size, unsigned int vAddr) {

    int* array = (int*)malloc(size * sizeof(int));

    for (int idx = 0; idx < size; idx++) {
        array[idx] = getVPNFromVirtualAddress(vAddr, masks[idx], shifts[idx]);
    }

    return array;
}

int concatenate(int frame, int offset, int level) {
    // Ensure that the frame doesn't exceed the level
    if (level < 0 || level >= sizeof(int) * 8) {
        std::cerr << "Invalid level value." << std::endl;
        return -1;  // Return an error code
    }

    // Perform the concatenation
    frame = frame << level;
    int concat = frame + offset;
    return concat;
}

int grabPhysicalAddress(int frame, int level, unsigned int virtualAddress) {

    // Ensure that the level is non-negative
    if (level < 0) {
        std::cerr << "Invalid level value." << std::endl;
        return -1;  // Return an error code
    }

    unsigned int mask = bit_Mask(level, 0);


    int offset = (virtualAddress & mask);


    int dest = concatenate(frame, offset, level);


    return dest;
}

unsigned int bit_Mask(unsigned int bit_level, unsigned int shift) {
    unsigned mask = 0;
    for (unsigned int idx = 0; idx < bit_level; idx++) {
        mask |= (1 << idx);
    }

    return mask << shift;
}

bool checkTotalBitSize(int total) {
    return total > MAX_NUM_OF_LEVELS;
}

LevelNode* createLevel(int bitSize, bool leaf) {

    LevelNode* node = (LevelNode*)malloc(sizeof(LevelNode));
    if (leaf) { // check if creating a leaf node
        node -> MapPtr = (Map*)malloc(bitSize * (sizeof(Map) + 1));
        for (int idx = 0; idx < bitSize; idx++) {
            node -> MapPtr[idx].frame = -1;
            node -> MapPtr[idx].status = INVALID;
        }
    } else {
        node -> nextLevelPtr = (LevelNode**)malloc(bitSize * (sizeof(LevelNode*) + 1));
        for (int idx = 0; idx < bitSize; idx++) {
            node -> nextLevelPtr[idx] = NULL;
        }
    }

    node -> depth = 0;

    return node;
}

unsigned int getVPNFromVirtualAddress(unsigned int virtualAddress, unsigned int mask, unsigned int shift){
    unsigned int page;
    unsigned int VPN;
    page = (virtualAddress & mask);
    VPN = page >> shift;
    return VPN;
}


Map* findVpn2PfnMapping(PageTable* pageTable, unsigned int virtualAddress){
    LevelNode* temp = pageTable -> root;

    for (int idx = 0; idx < pageTable -> levelCount; idx++) {
        unsigned int vpn = getVPNFromVirtualAddress(virtualAddress, pageTable -> bitMask[idx], pageTable -> bitShift[idx]);

        if (idx != pageTable -> levelCount - 1) {

            if (!temp -> nextLevelPtr[vpn]) {
                return NULL;
            }

            temp = temp -> nextLevelPtr[vpn];

        } else {

            if (temp -> MapPtr[vpn].status) {

                return &(temp -> MapPtr[vpn]);
            }

            return NULL;
        }

    }

    return NULL;
}

void insertVpn2PfnMapping(PageTable* pageTable, unsigned int virtualAddress, int &frame){

    LevelNode* temp = pageTable -> root;

    for (int idx = 0; idx < pageTable -> levelCount; idx++) {

        unsigned int vpn = getVPNFromVirtualAddress(virtualAddress, pageTable -> bitMask[idx], pageTable -> bitShift[idx]);
        if (idx == pageTable -> levelCount -1) { // Is current node a leaf

            temp -> MapPtr[vpn].frame = frame;
            temp -> MapPtr[vpn].status = VALID;
            frame++;

            return;

        } else if (idx == pageTable -> levelCount -2){ // Check if next node is a leaf

            if (!temp -> nextLevelPtr[vpn]) {
                temp -> nextLevelPtr[vpn] = createLevel(pageTable -> entryCount[idx], true);
            }

        } else { // Next node is a Level

            if (!temp -> nextLevelPtr[vpn]) {
                temp -> nextLevelPtr[vpn] = createLevel(pageTable -> entryCount[idx], false);
            }
        }

        temp = temp -> nextLevelPtr[vpn];
        temp -> depth = idx +1;
        temp -> PageTablePtr = pageTable;

    }
}

void countTotalBytes(LevelNode* level, unsigned int& totalByte, PageTable* page) {

    totalByte += sizeof(level);

    for (int idx = 0; idx < page -> entryCount[level -> depth]; idx++) {
        printf("count: %d , index: %d", page -> entryCount[level -> depth], idx);
        if (level -> nextLevelPtr[idx] != NULL)
            countTotalBytes(level -> nextLevelPtr[idx], totalByte, page);
    }

}


