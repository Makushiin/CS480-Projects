
//Zizhao Zheng 825807834
//Maxine Mayor 825551461

#define STANDALONE

#include <stdio.h>
#include <stdlib.h>  // Include the standard library header for the 'exit' function
#include "vaddr_tracereader.h"

/*
 * If you are using this program on a big-endian machine (something
 * other than an Intel PC or equivalent) the unsigned longs will need
 * to be converted from little-endian to big-endian.
 */
uint32_t swap_endian(uint32_t num) {
    return (((num << 24) & 0xff000000) | ((num << 8) & 0x00ff0000) |
            ((num >> 8) & 0x0000ff00) | ((num >> 24) & 0x000000ff));
}

/* determine if the system is big- or little-endian */
ENDIAN endian() {
    /* Allocate a 32 bit character array and pointer which will be used
     * to manipulate it.
     */
    uint32_t *a;
    unsigned char p[4];
    a = (uint32_t *) p; /* Let a point to the character array */
    *a = 0x12345678; /* Store a known bit pattern to the array */
    /* Check the first byte. If it contains the high order bits,
     * it is big-endian, otherwise little-endian.
     */
    if (*p == 0x12)
        return BIG;
    else
        return LITTLE;
}

/* int NextAddress(FILE *trace_file, p2AddrTr *Addr)
 * Fetch the next address from the trace.
 *
 * trace_file must be a file handle to an trace file opened
 * with fopen. The user provides a pointer to an address structure.
 *
 * Populates the Addr structure and returns non-zero if successful.
 */
int NextAddress(FILE *trace_file, p2AddrTr *addr_ptr) {
    int readN; /* number of records stored */
    static ENDIAN byte_order = UNKNOWN; /* don't know machine format */
    if (byte_order == UNKNOWN) {
        /* First invocation. Determine if this is a little- or
         * big-endian machine so that we can convert bit patterns
         * if needed that are stored in little-endian format
         */
        byte_order = endian();
    }
    /* Read the next address record. */
    readN = fread(addr_ptr, sizeof(p2AddrTr), 1, trace_file);
    if (readN) {
        if (byte_order == BIG) {
            /* records stored in little endian format, convert */
            addr_ptr->addr = swap_endian(addr_ptr->addr);
            addr_ptr->time = swap_endian(addr_ptr->time);
        }
    }
    return readN;
}

/* void AddressDecoder(p2AddrTr *addr_ptr, FILE *out)
 * Decode a Pentium II BYU address and print it to the specified
 * file handle (opened by fopen in write mode)
 */
void AddressDecoder(p2AddrTr *addr_ptr, FILE *out) {
    fprintf(out, "%08lx ", (unsigned long) addr_ptr->addr); /* address */
    /* what type of address request */
    switch (addr_ptr->reqtype) {
        case FETCH:
            fprintf(out, "FETCH\t\t");
            break;
        case MEMREAD:
            fprintf(out, "MEMREAD\t");
            break;
        case MEMREADINV:
            fprintf(out, "MEMREADINV\t");
            break;
        case MEMWRITE:
            fprintf(out, "MEMWRITE\t");
            break;
        case IOREAD:
            fprintf(out, "IOREAD\t\t");
            break;
        case IOWRITE:
            fprintf(out, "IOWRITE\t");
            break;
        case DEFERREPLY:
            fprintf(out, "DEFERREPLY\t");
            break;
        case INTA:
            fprintf(out, "INTA\t\t");
            break;
    }
}
