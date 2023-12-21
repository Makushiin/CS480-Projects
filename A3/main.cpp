
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "pageTable.h"
#include "log_helpers.h"
#include "vaddr_tracereader.h"
#include "WSClock.h"
using namespace std;

int main (int argc, char **argv){
    if (argc < 4) { // Minimum requirement is the program name, trace file, readwrites file, and one level of bits
        cerr << "Usage: " << argv[0] << " trace_file readswrites_file level_bits..." << std::endl;
        return 1;
    }
    int nFlag = -1;  // Default value for -n
    int fFlag = 999999;  // Default value for -f
    int aFlag = 10;  // Default value for -a
    std::string lFlag = "summary";  // Default value for -l
    bool ifWSClock = false;
    int option;

    LogOptionsType* LogOptions = (LogOptionsType*) malloc(sizeof(LogOptionsType));
    LogOptions -> pagetable_bitmasks = false;
    LogOptions -> addressTranslation = false;
    LogOptions -> vpns_pfn = false;
    LogOptions -> vpn2pfn_with_pagereplace = false;
    LogOptions -> offset = false;
    LogOptions -> summary = true;

    /*still need to verfied*/
    PageFrame* pf = (PageFrame*)malloc(sizeof(PageFrame));
    pf -> lastAccess = 0;
    /*----*/

    while ((option = getopt(argc, argv, "n:f:a:l:")) != -1) {

        switch (option) {
            case 'n':
                if (atoi(optarg)< 1) {
                    cout << "Number of memory accesses must be a number, greater than 0" << endl;
                    exit(2);
                }
                nFlag = atoi(optarg);
                break;
            case 'f':
                if (atoi(optarg) < 1) {
                    cout << "Number of available frames must be a number, greater than 0" << endl;
                    exit(2);
                }
                fFlag = atoi(optarg);
                ifWSClock = true;
                break;
            case 'a':
                if (atoi(optarg) < 1) {
                    cout << "Age of last access considered recent must be a number, greater than 0" << endl;
                    exit(2);
                }
                aFlag = atoi(optarg);
                break;
            case 'l':
                if (strcmp(optarg, "bitmasks") == 0) {
                    LogOptions -> pagetable_bitmasks = true;
                    LogOptions -> summary = false;
                }
                if (strcmp(optarg, "va2pa") == 0) {
                    LogOptions -> addressTranslation = true;
                    LogOptions -> summary = false;
                }
                if (strcmp(optarg, "vpns_pfn") == 0) {
                    LogOptions -> vpns_pfn = true;
                    LogOptions -> summary = false;
                }
                if (strcmp(optarg, "vpn2pfn_pr") == 0) {
                    LogOptions -> vpn2pfn_with_pagereplace = true;
                    LogOptions -> summary = false;
                }
                if (strcmp(optarg, "offset") == 0) {
                    LogOptions -> offset = true;
                    LogOptions -> summary = false;
                }
                if (strcmp(optarg, "summary") == 0) {
                    LogOptions -> summary = true;
                    break;
                    default:
                        cout << "Incorrect optional input" << endl;
                    exit(2);
                }
        }
    }

    int* bitInputArray = new int[MAX_NUM_OF_LEVELS];
    int totalBitSize = 0;

    PageTable* pageTable = (PageTable*)malloc(sizeof(PageTable));
    WSClock wsclock(fFlag, aFlag);

    /* PageTable-> levelCount initialization */
    pageTable -> levelCount = 0;
    int idx = optind;
    char* tracerFile;
    int fullMask;

    /* Process positional arguments */
    if (idx < argc) {
        tracerFile = argv[idx];

        ifstream checkFile (tracerFile);
        if (!checkFile.is_open()) {
            cout << "Unable to open <<" << tracerFile << ">>" << endl;
            exit(2);
        }
        checkFile.close();
    }

    // Process level bits
    std::vector<int> levelBits;
    for (int i = optind + 2; i < argc; i++) {
        int levelBit = std::atoi(argv[i]);
        if (levelBit < 1) {
            cout << "Level " << (i - optind - 2) << " page table must be at least 1 bit" << endl;
            return 1;  // Exit the program
        }
        totalBitSize += levelBit;
        levelBits.push_back(levelBit);
    }
    if(checkTotalBitSize(totalBitSize)) {
        cout << "Too many bits used in page tables" << endl;
        exit(2);
    }
    // Debug output for level bits
//    std::cout << "Number of levels: " << levelBits.size() << std::endl;
    for (size_t i = 0; i < levelBits.size(); ++i) {

        pageTable -> levelCount+=1;
    }
    int* levelBitsArray = levelBits.data();
    /* Initialize bitShift[] */
    pageTable -> bitShift = bitShiftArray(levelBitsArray, pageTable -> levelCount);

    /* Initialize bitMask[] */
    pageTable -> bitMask = bitMaskArray(levelBitsArray, pageTable -> bitShift, pageTable -> levelCount);

    /* Initialize entryCount[] */
    pageTable -> entryCount = entryCountArray(levelBitsArray, pageTable -> levelCount);


    if (LogOptions -> pagetable_bitmasks == true) {
        log_bitmasks(pageTable->levelCount, (uint32_t *) pageTable->bitMask);
        return 0;
    }

    if (pageTable -> levelCount == 1) {
        pageTable -> root = createLevel(pageTable -> entryCount[0], true);
    } else {
        pageTable -> root = createLevel(pageTable -> entryCount[0], false);
    }

    pageTable -> root -> PageTablePtr = pageTable;

    FILE *ifp;	        /* trace file */
    unsigned long vAddr;  /* instructions processed */
    p2AddrTr trace;	/* traced address */
    int victimFrame = 0;
    Map* PFN = (Map*)malloc(sizeof(Map));

    ifp = fopen(tracerFile, "r");

    int fullShift = 32 - fullMask;
    /*still need verifcaiton and modification*/
    int readCount = 0;
    int numofPageReplace = 0;
    int pageHit = 0;
    bool ptHitCheck;
    unsigned int bytes = sizeof(pageTable);


    while (!feof(ifp)) {
        unsigned int fullVPN;
        ptHitCheck = false;

        if (nFlag != 0 && readCount == nFlag)
            break;

        if (NextAddress(ifp, &trace)) {
            vAddr = trace.addr;
        }


        if (ifWSClock == true) {
            fullVPN = getVPNFromVirtualAddress(vAddr, fullMask, fullShift);
            victimFrame = wsclock.performPageReplacement(fullVPN);
            pf->lastAccess++;
            PFN = findVpn2PfnMapping(pageTable, vAddr);

            if (!PFN) {
                insertVpn2PfnMapping(pageTable, vAddr, victimFrame);
                PFN =  findVpn2PfnMapping(pageTable, vAddr);
            } else {
                ptHitCheck = true;
            }

            if (victimFrame != -1) {
                numofPageReplace++;  // Increment when a page replacement occurs.
            }

            wsclock.updateAccessTime(PFN->frame, pf->lastAccess);
        } else {
            pf->lastAccess++;
            insertVpn2PfnMapping(pageTable, vAddr,victimFrame);
            PFN =  findVpn2PfnMapping(pageTable, vAddr);
        }

        int physicalAddr = grabPhysicalAddress(PFN -> frame, MAX_BIT_SIZE-totalBitSize, vAddr);
        if (LogOptions -> addressTranslation)
            log_va2pa((uint32_t)vAddr, (uint32_t)physicalAddr);


        if(LogOptions -> vpn2pfn_with_pagereplace){
            log_mapping((uint32_t) vAddr, (uint32_t) physicalAddr, -1,ptHitCheck);
        }
        if (LogOptions -> offset) {
            unsigned int mask = bit_Mask(MAX_BIT_SIZE - totalBitSize, 0);
            print_num_inHex(vAddr & mask);
        }
        if (LogOptions -> vpns_pfn) {
            pageTable -> pages = pagesArray(pageTable -> bitMask, pageTable -> bitShift, pageTable -> levelCount, vAddr);
            log_vpns_pfn(pageTable -> levelCount, (uint32_t*)pageTable -> pages, PFN -> frame);
        }

        /* printf("%08X => %08X\n", vAddr, physicalAddr); */

        readCount++;
    }

    if (LogOptions -> summary) {
        countTotalBytes(pageTable -> root, bytes, pageTable);
        log_summary(MAX_BIT_SIZE - totalBitSize, numofPageReplace,pageHit, readCount, PFN -> frame, bytes);
    }

    fclose(ifp);
    cout << "Complete" << endl;
    return 0;
}
